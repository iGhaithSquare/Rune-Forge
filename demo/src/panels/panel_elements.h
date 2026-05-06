#ifndef PANELELEMENT_H
#define PANELELEMENT_H
#include "panel.h"
struct panel_element{
    size_t ID;
    panel* Parent;
    short X;
    short Y;
    void (*On_Update)(panel_element *Self);
    void (*On_Render)(panel_element *Self);
    void (*On_Destroy)(panel_element *Self);
};
void init_panel_element_base(panel_element* Element,short X,short Y,void (*On_Update)(panel_element *Self),void (*On_Render)(panel_element *Self),void (*On_Destroy)(panel_element *Self));
#endif