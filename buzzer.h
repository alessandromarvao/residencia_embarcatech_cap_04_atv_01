#ifndef BUZZER_H
#define BUZZER_H

#include "pico/stdlib.h"

#define BUZZER_PIN 21

void init_buzzer();
void play_buzzer(uint32_t freq);
void buzzer_off();


#endif