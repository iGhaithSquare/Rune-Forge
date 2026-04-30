#ifndef PANELBUTTON_H
#define PANELBUTTON_H
#include "panel_elements.h"
typedef struct panel_button panel_button;
struct panel_button{
    panel_element Base;
    void (*On_Click)(panel_button* Self);
    const char* Text;
    uint8_t Is_Held;
};
panel_button* create_panel_button(short X, short Y,const char* Text,short Width,void (*On_Click)(panel_button* Self));
#endif