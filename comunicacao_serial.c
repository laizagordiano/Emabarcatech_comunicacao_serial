#include <stdlib.h>
#include <stdio.h>
#include <pico/multicore.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "matriz_leds.h"
#include "hardware/clocks.h"

// arquivo .pio
#include "pio_matrix.pio.h"

// Definições dos pinos e configurações de hardware
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C // Endereço do display SSD1306

#define LED_PIN_RED 13  // Pino do LED vermelho
#define LED_PIN_GREEN 11  // Pino do LED verde
#define LED_PIN_BLUE 12  // Pino do LED azul

#define RED 191
#define GREEN 0
#define BLUE 191

#define BUTTON_A_PIN 5  // Pino do botão A
#define BUTTON_B_PIN 6  // Pino do botão B

#define DEBOUNCE_TIME_US 200000  // 200ms de debounce para os botões
#define TIME 100  // Tempo para controle de atualização


// Variáveis globais para controlar os LEDs e o estado dos botões
static char character;  // Caractere recebido do usuário
bool led_buffer[NUM_PIXELS] = {0};  // Buffer para a matriz de LEDs

static volatile uint32_t last_press_A = 0;  // Tempo do último pressionamento do botão A
static volatile uint32_t last_press_B = 0;  // Tempo do último pressionamento do botão B

static volatile uint8_t blue = 0;  // Estado do LED azul
static volatile uint8_t green = 0;  // Estado do LED verde

uint32_t last_time = 0;  // Variável para controle de tempo

// Função de configuração dos pinos
void setup() {
    // Configuração dos botões
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);  // Define como entrada
    gpio_pull_up(BUTTON_A_PIN);  // Habilita resistor de pull-up

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);  // Define como entrada
    gpio_pull_up(BUTTON_B_PIN);  // Habilita resistor de pull-up
    
    // Configuração dos LEDs (vermelho, verde e azul)
    gpio_init(LED_PIN_RED);
    gpio_set_dir(LED_PIN_RED, GPIO_OUT);  // Define como saída
    gpio_put(LED_PIN_RED, 0);  // Desliga o LED vermelho

    gpio_init(LED_PIN_GREEN);
    gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);  // Define como saída
    gpio_put(LED_PIN_GREEN, 0);  // Desliga o LED verde

    gpio_init(LED_PIN_BLUE);
    gpio_set_dir(LED_PIN_BLUE, GPIO_OUT);  // Define como saída
    gpio_put(LED_PIN_BLUE, 0);  // Desliga o LED azul
}

// Função de inicialização do I2C
void I2C_init() {
    i2c_init(I2C_PORT, 400 * 1000);  // Inicializa o I2C a 400kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);  // Configura o pino SDA para função I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);  // Configura o pino SCL para função I2C
    gpio_pull_up(I2C_SDA);  // Habilita pull-up no SDA
    gpio_pull_up(I2C_SCL);  // Habilita pull-up no SCL
}

// Função de inicialização do display SSD1306
void SSD1306_init(ssd1306_t* ssd) {
    ssd1306_init(ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);  // Inicializa o display
    ssd1306_config(ssd);  // Configura o display
    ssd1306_send_data(ssd);  // Envia os dados para o display
    ssd1306_fill(ssd, false);  // Limpa o display
    ssd1306_send_data(ssd);  // Envia novamente após limpar
}

// Função para ler caracteres digitados
void read_caracter() {
    while (true) {
        int ch = getchar();  // Lê um caractere do usuário
        if (ch != EOF) {  // Verifica se a entrada é válida
            character = (char)ch;  // Armazena o caractere
        }
        sleep_ms(500);  // Espera meio segundo antes de ler novamente
    }
}

// Função de interrupção dos botões
static void button_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());  // Obtém o tempo atual em microssegundos

    // Verifica o botão A (vermelho)
    if (gpio == BUTTON_A_PIN && (events & GPIO_IRQ_EDGE_FALL)) {
        if (current_time - last_press_A > DEBOUNCE_TIME_US) {  // Verifica debounce
            last_press_A = current_time;  // Atualiza o último tempo de pressionamento
            if (green == 0) {
                green = 1;
                gpio_put(LED_PIN_GREEN, 1);  // Acende o LED verde
            } else {
                green = 0;
                gpio_put(LED_PIN_GREEN, 0);  // Apaga o LED verde
            }
        }
    } 
    // Verifica o botão B (azul)
    else if (gpio == BUTTON_B_PIN && (events & GPIO_IRQ_EDGE_FALL)) {
        if (current_time - last_press_B > DEBOUNCE_TIME_US) {  // Verifica debounce
            last_press_B = current_time;  // Atualiza o último tempo de pressionamento
            if (blue == 0) {
                blue = 1;
                gpio_put(LED_PIN_BLUE, 1);  // Acende o LED azul
            } else {
                blue = 0;
                gpio_put(LED_PIN_BLUE, 0);  // Apaga o LED azul
            }
        }
    }
}

int main() {
    
    PIO pio = pio0;
    int sm = 0;
    ssd1306_t ssd;
    bool cor = true;

    stdio_init_all();  // Inicializa a comunicação serial
    setup();  // Configura os pinos e LEDs
    I2C_init();  // Inicializa a comunicação I2C
    SSD1306_init(&ssd);  // Inicializa o display SSD1306

    multicore_launch_core1(read_caracter);  // Lança a tarefa de leitura no núcleo 1

    uint offset = pio_add_program(pio, &pio_matrix_program);  // Adiciona o programa da matriz de LEDs
    pio_matrix_program_init(pio, sm, offset, PIN_MATRIX);  // Inicializa a matriz de LEDs

    // Configura interrupções para os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &button_handler);

    while (true) {
        // Limpa o display
        ssd1306_fill(&ssd, !cor);

        // Exibe informações sobre o caractere escolhido
        ssd1306_draw_string(&ssd, "Caractere", 12, 10); 
        ssd1306_draw_string(&ssd, "Escolhido", 12, 20); 
        ssd1306_draw_string(&ssd, (char[]){character, '\0'}, 99, 15);  // Exibe o caractere no display
        ssd1306_line(&ssd, 98, 25, 109, 25, 1);  // Desenha uma linha no display
        
        // Exibe o status dos LEDs verde e azul
        if (green == 0) {
            ssd1306_draw_string(&ssd, "Led verde  off ", 12, 35); 
            printf("Status do led verde: desligado\n");
        } else {
            ssd1306_draw_string(&ssd, "Led verde  on ", 12, 35); 
            printf("Status do led verde: ligado\n");
        }

        if (blue == 0) {
            ssd1306_draw_string(&ssd, "Led azul   off ", 12, 45); 
            printf("Status do led azul: desligado\n");
        } else {
            ssd1306_draw_string(&ssd, "Led azul   on ", 12, 45); 
            printf("Status do led azul: ligado\n");
        }

        // Desenha a borda do display
        ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); 

        // Atualização do display
        ssd1306_send_data(&ssd);

        // Desenha o caractere na matriz de LEDs
        set_character_matrix(character, led_buffer);  // Atualiza a matriz de LEDs com o caractere
        set_matrix_led(RED, GREEN, BLUE, led_buffer);  // Controla os LEDs conforme o buffer

        sleep_ms(1000);  // Aguarda 1 segundo antes de atualizar novamente
    }
}
