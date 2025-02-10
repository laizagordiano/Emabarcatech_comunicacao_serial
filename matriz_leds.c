#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matriz_leds.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"

#define NUM_PIXELS 25

#define PIN_MATRIX 7  // Pino de saída da matriz WS2812



static inline uint32_t matrix_rgb(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t) (r) << 8) | ((uint32_t) (g) << 16) | (uint32_t) (b);
}


// Matrizes para números de 0 a 9
bool numero_0[NUM_PIXELS] = {
    0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0
};

bool numero_1[NUM_PIXELS] = {
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0,
    0, 1, 1, 0, 0,
    0, 0, 1, 0, 0
};

bool numero_2[NUM_PIXELS] = {
    0, 1, 1, 1, 0,
    0, 1, 0, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0
};

bool numero_3[NUM_PIXELS] = {
    0, 1, 1, 1, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0
};

bool numero_4[NUM_PIXELS] = {
    0, 1, 0, 0, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 0, 1, 0
};

bool numero_5[NUM_PIXELS] = {
    0, 1, 1, 1, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 1, 0, 0, 0,
    0, 1, 1, 1, 0
};

bool numero_6[NUM_PIXELS] = {
    0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 1, 0, 0, 0,
    0, 1, 1, 1, 0
};

bool numero_7[NUM_PIXELS] = {
    0, 0, 0, 1, 0,
    0, 1, 0, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0
};

bool numero_8[NUM_PIXELS] = {
    0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0
};

bool numero_9[NUM_PIXELS] = {
    0, 1, 1, 1, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0
};

// Array de ponteiros para os números
bool *sprites[10] = {numero_0, numero_1, numero_2, numero_3, numero_4, numero_5, numero_6, numero_7, numero_8, numero_9};

// Define a matriz de LEDs com base no caractere recebido
void set_character_matrix(char character, bool *led_buffer) {
    if (character >= '0' && character <= '9') {
        memcpy(led_buffer, sprites[character - '0'], NUM_PIXELS * sizeof(bool));
    } else {
        memset(led_buffer, 0, NUM_PIXELS * sizeof(bool)); // Desliga todos os LEDs
    }
}

static inline void set_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

void set_matrix_led(uint8_t r, uint8_t g, uint8_t b, bool *led_buffer) {
    uint32_t color = matrix_rgb(r * 0.5, g * 0.5, b * 0.5);  // Sem ajuste de intensidade para cores

    for (uint8_t i = 0; i < NUM_PIXELS; ++i) {
        if (led_buffer[i]) {
            set_pixel(color);  // Aplica a cor se o LED estiver aceso
        } else {
            set_pixel(0);  // Desliga o LED se o buffer estiver vazio
        }
    }
}




