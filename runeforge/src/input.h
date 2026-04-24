#ifndef INPUT_H
#define INPUT_H
#include <gaven.h>
create_event_category(Input,2);

uint8_t is_key_pressed(int Keycode);
short get_mouse_X(void);
short get_mouse_Y(void);

void init_input(void);
void input_polling(void);
#endif