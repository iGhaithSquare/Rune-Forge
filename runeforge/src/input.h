#ifndef INPUT_H
#define INPUT_H
#include <gaven.h>
#include "key_codes.h"

uint8_t is_key_pressed(int Keycode);
uint8_t is_key_just_pressed(int Keycode);
uint8_t is_key_just_released(int Keycode);
short get_mouse_X(void);
short get_mouse_Y(void);

void init_input(void);
void input_polling(void);
#endif