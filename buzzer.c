#include "buzzer.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

void play_buzzer(uint32_t freq)
{
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint channel = pwm_gpio_to_channel(BUZZER_PIN);

    // Divisor de clock fixo para manter wrap dentro de 16 bits
    const float clock_divider = 8.0f;
    pwm_set_clkdiv(slice_num, clock_divider);

    // Cálculo do valor de wrap
    uint32_t wrap = (uint32_t)(125000000.0f / (clock_divider * freq)) - 1;
    pwm_set_wrap(slice_num, wrap);
    
    // Duty cycle de 50% para melhor qualidade de som
    // Para ajustar o volume: mude o duty cycle (ex: wrap / 4 para 25%)
    pwm_set_chan_level(slice_num, channel, wrap / 4);
    
    pwm_set_enabled(slice_num, true);
}

void buzzer_off()
{
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_enabled(slice_num, false);
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_SIO);  // Volta para modo GPIO
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);
}
