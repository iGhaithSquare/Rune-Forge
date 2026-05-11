#ifndef PANELBUTTON_H
#define PANELBUTTON_H
#include "panel_elements.h"
typedef struct panel_button panel_button;
struct panel_button{
    panel_element Base;
    void (*On_Click)(panel_button* Self);
    const char* Text;
    uint8_t Is_Held;
    short Width,Height;
    void *Button_Data;
    sprite Sprite;
};
panel_button* create_panel_button(short X, short Y,const char* Text,short Width,void* Button_Data,void (*On_Click)(panel_button* Self));
#endif