#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

#define RED_LED_PIN 13
#define GREEN_LED_PIN 11
#define BLUE_LED_PIN 12

// Cria um alias STATUS para um tipo enum
typedef enum
{
    VERMELHO,
    VERDE,
    AMARELO,
    AMARELO_BOTAO
} status;

// Tempo do contador do semáforo (10000ms pra verde e vermelho; 3000ms pra amarelo)
volatile int time;

// Recebe o estado atual do semáforo
volatile status current_status;

void init()
{
    stdio_init_all();

    // Inicializa os pinos dos LEDs
    gpio_init(RED_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);

    gpio_init(GREEN_LED_PIN);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);

    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);
}

// Função de callback que será chamada a cada intervalo definido pelo temporizador.
// Esta função alterna o estado do LED de acordo com a ordem do semáforo.
bool semaphore_timer_callback(repeating_timer_t *t)
{
    gpio_put(RED_LED_PIN, 0);
    gpio_put(GREEN_LED_PIN, 0);
    gpio_put(BLUE_LED_PIN, 0);

    printf("Mudou a cor\n");

    switch (current_status)
    {
    case VERMELHO:
        // Acende o LED vermelho
        gpio_put(RED_LED_PIN, 1);

        // Altera a cor do LED para verde
        current_status = VERDE;
        break;
    case VERDE:
        // Acende o LED verde
        gpio_put(GREEN_LED_PIN, 1);

        // Altera a cor do LED para amarelo
        current_status = AMARELO;
        // Reduz o tempo de espera para alterar o estado do semáforo para 3 segundos
        time = 3000;
        break;
    case AMARELO:
        // Acende o LED amarelo
        gpio_put(RED_LED_PIN, 1);
        gpio_put(GREEN_LED_PIN, 1);

        // Altera a cor do LED para vermelho
        current_status = VERMELHO;
        // Altera o tempo de espera do semáforo para 10 segundos
        time = 10000;
        break;
    }
}

int main()
{
    init();

    current_status = VERMELHO;
    sleep_ms(3000);

    printf("Iniciando temporizador...\n");
    
    gpio_put(RED_LED_PIN, 1);

    // Inicia o temporizador de 10s com a função de callback definida acima
    time = 10000;

    // Declara uma estrutura para armazenar informações sobre o temporizador repetitivo.
    struct repeating_timer timer;

    add_repeating_timer_ms(time, semaphore_timer_callback, NULL, &timer);

    while (true)
    {
        tight_loop_contents();
    }
}
