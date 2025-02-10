# Projeto Comunicação Serial com Display OLED SSD1306.

Este projeto implementa o controle de um display oled SSD1306, uma matriz de LEDs WS2812, um LED RGB e dois botões na placa **BitDogLab**. O código utiliza comunicação serial, I2C, PIO e interrupções para botões.

## **Vídeo de demonstração**
[Assista ao vídeo de demonstração aqui!](https://drive.google.com/file/d/1R69V0Gr5CIKLiAoz_22dj-IULyukn7aJ/view?usp=sharing)


## **Funcionalidades**
- O **display SSD1306** exibe o caractere digitado e o status dos LEDs.
- A **matriz de LEDs** exibe o caractere digitado (0-9).
- O **botão A** alterna o estado do LED verde.
- O **botão B** alterna o estado do LED azul.
- A comunicação serial permite entrada de caracteres para exibição.

##  **Componentes e Conexões**
- Matriz 5x5 de LEDs WS2812 - GPIO 7
- LED RGB - GPIOs 11, 12 e 13
- Botão A - GPIO 5
- Botão B - GPIO 6
- Display SSD1306 via I2C - GPIOs 14 e 15

## **Requisitos do Projeto**
Para o desenvolvimento, devem ser seguidos os seguintes requisitos:
1. **Uso de interrupções:** Todas as funcionalidades relacionadas aos botões devem ser implementadas utilizando rotinas de interrupção (IRQ).
2. **Debouncing:** É obrigatório implementar o tratamento do bouncing dos botões via software.
3. **Controle de LEDs:** O projeto deve incluir o uso de LEDs comuns e LEDs WS2812, demonstrando o domínio de diferentes tipos de controle.
4. **Utilização do Display:** A utilização de fontes maiúsculas e minúsculas demonstrará o domínio do uso de bibliotecas, o entendimento do princípio de funcionamento do display, bem como, a utilização do protocolo I2C.
5. **Envio de informação pela UART:** Visa observar a compreensão sobre a comunicação serial via UART.
6. **Organização do código:** O código deve estar bem estruturado e comentado para facilitar o entendimento.