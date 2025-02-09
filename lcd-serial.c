#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <ctype.h>   
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "ws2812.pio.h"
#include "lib/ssd1306.h"
#include "lib/font.h"


//i2c display
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define OLED_ADDR 0x3C

#define BOTAO_A 5
#define BOTAO_B 6
#define WS2812_PINO 7
#define LED_VERDE 11
#define LED_AZUL 12
#define QTD_LEDS 25


absolute_time_t debounce_A;
absolute_time_t debounce_B;

volatile int numero = 0;
PIO pio = pio0;
int unused_sm = 0;

ssd1306_t ssd;
volatile bool led_green_state = false;
volatile bool led_blue_state  = false;
static char status_led_azul[32]  = "Led azul OFF";
static char status_led_verde[32]  = "Led verde OFF";
static char caractere[32]   = "Caractere";

void initialize_config();
void gpio_irq_handler(uint gpio, uint32_t events);
void update_display(void);
void apaga_matriz(void);

// mapeamento da ordem dos leds da matriz de led
static const uint8_t mapeamento_led[5][5] = {
    {24, 23, 22, 21, 20},
    {15, 16, 17, 18, 19},
    {14, 13, 12, 11, 10},
    {5, 6, 7, 8, 9},
    {4, 3, 2, 1, 0}};

// mapeamento dos numeros na matriz de leds
static const uint8_t numeros_matriz[10][QTD_LEDS] = {
    {// 0
     1, 1, 1, 1, 1,
     1, 0, 0, 0, 1,
     1, 0, 0, 0, 1,
     1, 0, 0, 0, 1,
     1, 1, 1, 1, 1},
    {// 1
     0, 0, 1, 0, 0,
     0, 1, 1, 0, 0,
     0, 0, 1, 0, 0,
     0, 0, 1, 0, 0,
     1, 1, 1, 1, 1},
    {// 2
     1, 1, 1, 1, 1,
     0, 0, 0, 0, 1,
     1, 1, 1, 1, 1,
     1, 0, 0, 0, 0,
     1, 1, 1, 1, 1},
    {// 3
     1, 1, 1, 1, 1,
     0, 0, 0, 0, 1,
     0, 1, 1, 1, 1,
     0, 0, 0, 0, 1,
     1, 1, 1, 1, 1},
    {// 4
     1, 0, 0, 1, 0,
     1, 0, 0, 1, 0,
     1, 1, 1, 1, 1,
     0, 0, 0, 1, 0,
     0, 0, 0, 1, 0},
    {// 5
     1, 1, 1, 1, 1,
     1, 0, 0, 0, 0,
     1, 1, 1, 1, 1,
     0, 0, 0, 0, 1,
     1, 1, 1, 1, 1},
    {// 6
     1, 1, 1, 1, 1,
     1, 0, 0, 0, 0,
     1, 1, 1, 1, 1,
     1, 0, 0, 0, 1,
     1, 1, 1, 1, 1},
    {// 7
     1, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 1,
     0, 0, 0, 1, 0,
     0, 0, 0, 1, 0},
    {// 8
     1, 1, 1, 1, 1,
     1, 0, 0, 0, 1,
     1, 1, 1, 1, 1,
     1, 0, 0, 0, 1,
     1, 1, 1, 1, 1},
    {// 9
     1, 1, 1, 1, 1,
     1, 0, 0, 0, 1,
     1, 1, 1, 1, 1,
     0, 0, 0, 0, 1,
     1, 1, 1, 1, 1}};

void initialize_config()
{

    // LEDs 
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);

    // inicia e configura o botão A
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);

    // inicia e configura o botão B
    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_B);

    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // configura a interrupção do botão e define o callback
    gpio_set_irq_enabled(BOTAO_B, GPIO_IRQ_EDGE_FALL, true);

    //config i2c
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    //inicializa display e configura
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, OLED_ADDR, I2C_PORT);
    ssd1306_fill(&ssd, false);
    ssd1306_config(&ssd); 
    ssd1306_send_data(&ssd);

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    update_display();

    // PIO para WS2812
    unused_sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, unused_sm, offset, WS2812_PINO, 800000, false); // configura o ws2812
    apaga_matriz();
    
}

void atualizar_matriz_led()
{
    // Define as cores para os LEDs. 'cor_acesa' é a cor que o LED acende (verde),
    // e 'cor_apagada' é a cor quando o LED está apagado (preto).
    uint32_t cor_acesa = 0xFF0000 << 8;  // 0xFF0000 é a cor verde, e o << 8 faz um deslocamento de bits.
    uint32_t cor_apagada = 0x000000 << 8; // 0x000000 é a cor preta (desligado), com deslocamento de bits.

    // Cria um buffer para armazenar o estado de cada LED. O tamanho do buffer é QTD_LEDS,
    // que é o número total de LEDs na matriz.
    uint32_t buffer_led[QTD_LEDS];

    // Inicializa todos os LEDs com a cor "apagada" (preto).
    for (int i = 0; i < QTD_LEDS; i++)
    {
        buffer_led[i] = cor_apagada;
    }

    // Percorre as 5 linhas da matriz de LEDs.
    for (int linha = 0; linha < 5; linha++)
    {
        // Percorre as 5 colunas da matriz de LEDs.
        for (int coluna = 0; coluna < 5; coluna++)
        {
            // Calcula o índice lógico do LED na matriz de 5x5.
            int indice_logico = linha * 5 + coluna;

            // Verifica se o valor para o LED na posição [linha][coluna] na matriz 'numeros_matriz' é 1,
            // o que significa que o LED deve ser aceso.
            if (numeros_matriz[numero][indice_logico] == 1)
            {
                // Mapeia o índice lógico para o índice físico usando a tabela 'mapeamento_led'.
                int indice_fisico = mapeamento_led[linha][coluna];

                // Atualiza o valor no 'buffer_led' para acender o LED na posição mapeada.
                buffer_led[indice_fisico] = cor_acesa;
            }
        }
    }

    // Envia os valores de 'buffer_led' para o hardware de LED, acendendo ou apagando os LEDs.
    for (int i = 0; i < QTD_LEDS; i++)
    {
        // Envia cada valor do buffer para a máquina de estado de PIO (pico I/O) para atualização do display.
        pio_sm_put_blocking(pio, unused_sm, buffer_led[i]);
    }
}


// realiza o debounce
// só atualiza o estado do botão, caso o botão permaneça pressionado pelo tempo mínimo
bool debounce_bt(uint pino, absolute_time_t *ultimo_tempo)
{
    absolute_time_t agora = get_absolute_time();
    if (absolute_time_diff_us(*ultimo_tempo, agora) >= 200000)
    {
        *ultimo_tempo = agora;
        return (gpio_get(pino) == 0);
    }
    return false;
}

void gpio_irq_handler(uint gpio, uint32_t events) // callback da interrupção
{

    if (events & GPIO_IRQ_EDGE_FALL) // borda de decida
    {

        if (gpio == BOTAO_A)
        {
            if (debounce_bt(BOTAO_A, &debounce_A))
            {
               gpio_put(LED_VERDE, !gpio_get(LED_VERDE));
               printf("LED verde: %s\n", gpio_get(LED_VERDE) ? "aceso" : "apagado");
               update_display();
            }
        }

        else if (gpio == BOTAO_B)
        {
            if (debounce_bt(BOTAO_B, &debounce_B))
            {
                gpio_put(LED_AZUL, !gpio_get(LED_AZUL));
                printf("LED azul: %s\n", gpio_get(LED_AZUL) ? "aceso" : "apagado");
                update_display();
            }
        }
    }
}

//atualiza o display 
void update_display(void) {
    if (gpio_get(LED_VERDE)) {
        strcpy(status_led_verde, "Led verde ON");
    } else {
        strcpy(status_led_verde, "Led verde OFF");
    }
    
    if (gpio_get(LED_AZUL)) {
        strcpy(status_led_azul, "Led azul ON");
    } else {
        strcpy(status_led_azul, "Led azul OFF");
    }

    ssd1306_fill(&ssd, false); //limpa buffer
    ssd1306_draw_string(&ssd, status_led_azul, 0, 0);
    ssd1306_draw_string(&ssd, status_led_verde, 0, 20);
    ssd1306_draw_string(&ssd, caractere, 0, 40);
    ssd1306_send_data(&ssd); //envia
}

//apaga toda matriz
void apaga_matriz(void) {
    uint32_t color_off = 0x000000 << 8;
    uint32_t ledBuffer[QTD_LEDS];
    for (int i = 0; i < QTD_LEDS; i++) {
        ledBuffer[i] = color_off; // Apagado
    }
    // Enviar para os 25 LEDs
    for (int i = 0; i < QTD_LEDS; i++) {
        pio_sm_put_blocking(pio, unused_sm, ledBuffer[i]);
    }
}

int main()
{
    stdio_init_all();

    initialize_config();

    debounce_A = get_absolute_time();
    debounce_B = get_absolute_time();

    while (true) {
        int leitura = getchar(); 
        sprintf(caractere, "Caractere: %c", (char)leitura);
        printf("Recebido: %c\n", (char)leitura);
        update_display();
        printf("atualizado: %c\n", (char)leitura);
        
        if (leitura >= '0' && leitura <= '9') { //digito
            printf("numero: %c\n", (char)leitura);  
            numero = leitura - '0';
            atualizar_matriz_led();
            printf("atualizando matriz led \n", (char)leitura);
        }
        else {
            apaga_matriz();
             printf("apagando matriz led \n", (char)leitura);
        }

    }
}
