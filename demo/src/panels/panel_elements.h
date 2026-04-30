#ifndef PANELELEMENT_H
#define PANELELEMENT_H
#include "panel.h"
struct panel_element{
    size_t ID;
    panel* Parent;
    short X;
    short Y;
    short Width;
    short Height;
    sprite Sprite;
    void (*On_Update)(panel_element *Self);
};
void init_panel_element_base(panel_element* Element,short X,short Y,sprite Sprite,void (*On_Update)(panel_element *Self));
void destroy_panel_element(panel_element* Self);
void render_panel_element(panel_element* Self);
#endif