#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

#define RED_LED_PIN 13
#define GREEN_LED_PIN 11
#define BLUE_LED_PIN 12
#define BTN_PIN 5

// Cria um alias STATUS para um tipo enum
typedef enum
{
    RED,
    GREEN,
    YELLOW
} status;

// Declara uma estrutura para armazenar informações sobre o temporizador repetitivo.
struct repeating_timer timer;

// Tempo do contador do semáforo (10000ms pra GREEN e vermelho; 3000ms pra amarelo)
volatile int semaphore_interval_ms;

// Recebe o estado atual do semáforo
volatile status current_status;

// Condicional que informa quando o temporizador precisa ser resetado
volatile bool reset_timer = false;

// Condicional que verifica se o botão foi pressionado
volatile bool is_button_pressed = false;

// Condicional que compara o estado do botão atual com o anterior (pressionado ou não)
volatile bool button_last_state = false;

/**
 * Inicializa os pinos GPIO, comunicação UART, PWM e outros serviços iniciais *
 */
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

    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);
}

/**
 * Função de callback que será chamada a cada intervalo definido pelo temporizador.
 * Esta função alterna o estado do LED de acordo com a ordem do semáforo.
 */
bool semaphore_timer_callback(repeating_timer_t *t)
{
    gpio_put(RED_LED_PIN, 0);
    gpio_put(GREEN_LED_PIN, 0);
    gpio_put(BLUE_LED_PIN, 0);

    switch (current_status)
    {
    case RED:
        // Acende o LED vermelho
        gpio_put(RED_LED_PIN, 1);
        printf("Mudou a cor para vermelho\n");

        // Ativa a flag para alterar o temporizador
        reset_timer = true;

        // Aumenta o tempo de espera para alterar o estado do semáforo para 3 segundos
        semaphore_interval_ms = 10000;

        // Altera a cor do LED para GREEN
        current_status = GREEN;
        break;
    case GREEN:
        // Acende o LED GREEN
        gpio_put(GREEN_LED_PIN, 1);
        printf("Mudou a cor para GREEN\n");

        // Altera a cor do LED para amarelo
        current_status = YELLOW;
        break;
    case YELLOW:
        // Acende o LED amarelo
        gpio_put(RED_LED_PIN, 1);
        gpio_put(GREEN_LED_PIN, 1);

        printf("Mudou a cor amarelo\n");

        reset_timer = true;

        // Reduz o tempo de espera para alterar o estado do semáforo para 3 segundos
        semaphore_interval_ms = 3000;

        // Altera a cor do LED para vermelho
        current_status = RED;
        break;
    }

    if (reset_timer)
    {
        reset_timer = false;

        // Cancela o temporizador atual
        cancel_repeating_timer(&timer);

        // Inicia um novo contador com o tempo estipulado
        add_repeating_timer_ms(semaphore_interval_ms, semaphore_timer_callback, NULL, &timer);
    }

    return true;
}

/**
 * Função de gatilho do alarme. Quando o botão A é pressionado, ele acende o LED amarelo por 3 segundos (tempo definido pelo alarme),
 * então o LED vermelho e aciona a flag para acender o LED GREEN após o temporizador de 10s.
 */
int64_t reset_timer_callback(alarm_id_t id, void *user_data)
{
    // Cancela o temporizador atual
    cancel_repeating_timer(&timer);

    // Apaga o LED GREEN, permanecendo ligado apenas o vermelho
    gpio_put(GREEN_LED_PIN, 0);
    printf("Mudou a cor para vermelho\n");

    // Aumenta o tempo de espera para alterar o estado do semáforo para 3 segundos
    semaphore_interval_ms = 10000;

    current_status = GREEN;

    // Inicia um novo contador com o tempo estipulado
    add_repeating_timer_ms(semaphore_interval_ms, semaphore_timer_callback, NULL, &timer);
}

/**
 * Função de gatilho quando o botão A é pressionado.
 */
void button_pressed(uint gpio, uint32_t events)
{
    is_button_pressed = true;

    // Corrige o debouncing do botão
    if (is_button_pressed && !button_last_state)
    {
        button_last_state = true;
        if (current_status == YELLOW)
        {
            printf("Chegou na interrupção\n");
            button_last_state = true;

            // Acende o LED amarelo
            gpio_put(RED_LED_PIN, 1);
            gpio_put(GREEN_LED_PIN, 1);

            printf("Mudou a cor amarelo\n");

            reset_timer = true;

            add_alarm_in_ms(3000, reset_timer_callback, NULL, false);

            // Altera a cor do LED para vermelho
            current_status = RED;
        }
    }

    is_button_pressed = false;
    button_last_state = !button_last_state;
}

int main()
{
    // Inicializa os serviços do Raspberry Pi
    init();

    printf("Iniciando temporizador...\n");

    // Acende o LED vermelho
    gpio_put(RED_LED_PIN, 1);

    // Altera a flag da cor do semáforo para vede
    current_status = GREEN;

    // Inicia o temporizador de 10s com a função de callback definida acima
    semaphore_interval_ms = 10000;

    // Temporizador do semáforo
    add_repeating_timer_ms(semaphore_interval_ms, semaphore_timer_callback, NULL, &timer);

    // Função de interrupção do botão. Quando pressionado, acende o LED amarelo
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, button_pressed);

    while (true)
    {
        tight_loop_contents();
    }
}