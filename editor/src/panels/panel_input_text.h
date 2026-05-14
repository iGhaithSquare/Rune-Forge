#ifndef PANEL_INPUT_TEXT_H
#define PANEL_INPUT_TEXT_H
#include "panel_elements.h"
typedef struct panel_input_text panel_input_text;
struct panel_input_text{
    panel_element Base;
    char Real_Text[256];
    char Final_Text[256];
    short Width;
    sprite Sprite;
    uint8_t Is_Dirty;
    uint8_t Editing;
};
panel_input_text* create_panel_input_text(short X, short Y,short Width);
#endif