#ifndef PANEL_TEXT_H
#define PANEL_TEXT_H
#include "panel_elements.h"
typedef struct panel_text panel_text;
struct panel_text{
    panel_element Base;
    const char* Text;
    short Padding;
    sprite Sprite;
    uint8_t Is_Dirty;
};
panel_text* create_panel_text(const char* Text,short X, short Y,short Padding);
#endif