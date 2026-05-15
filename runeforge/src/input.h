#ifndef INPUT_H
#define INPUT_H
#include <gaven.h>
#include "key_codes.h"

#ifndef RUNEFORGE_API
#ifdef _WIN32
    #ifdef RUNEFORGE_BUILD_DLL
        #define RUNEFORGE_API __declspec(dllexport)
    #else
        #define RUNEFORGE_API __declspec(dllimport)
    #endif
#else
    #define RUNEFORGE_API __attribute__((visibility("default")))
#endif
#endif
RUNEFORGE_API uint8_t is_key_pressed(int Keycode);
RUNEFORGE_API uint8_t is_key_just_pressed(int Keycode);
RUNEFORGE_API uint8_t is_key_just_released(int Keycode);
RUNEFORGE_API short get_mouse_X(void);
RUNEFORGE_API short get_mouse_Y(void);
RUNEFORGE_API const char* get_text_input(size_t* len);
RUNEFORGE_API void init_input(void);
RUNEFORGE_API void input_polling(void);
#endif