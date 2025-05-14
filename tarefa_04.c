#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

typedef enum
{
    VERMELHO,
    VERDE,
    AMARELO,
    AMARELO_BOTAO
} status;

// Tempo do contador do semáforo (10000ms pra verde e vermelho; 3000ms pra amarelo)
volatile int time = 10000;

// Recebe o estado atual do semáforo
volatile status current_status;

// Função de callback que será chamada a cada intervalo definido pelo temporizador.
// Esta função alterna o estado do LED de acordo com a ordem do semáforo.
bool semaphore_timer_callback(struct repeating_timer *t)
{
    switch (current_status)
    {
    case VERMELHO:
        break;
    case VERDE:
        break;
    case AMARELO:
        break;
    }
}

int main()
{
    stdio_init_all();

    current_status = VERMELHO;

    // Declara uma estrutura para armazenar informações sobre o temporizador repetitivo.
    struct repeating_timer timer;

    add_repeating_timer_ms(time, semaphore_timer_callback, NULL, &timer);

    while (true)
    {
        tight_loop_contents();
    }
}
