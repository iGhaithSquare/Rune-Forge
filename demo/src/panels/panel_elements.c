#include "panel_elements.h"
void init_panel_element_base(panel_element* Element,short X,short Y,void (*On_Update)(panel_element *Self),void (*On_Render)(panel_element *Self),void (*On_Destroy)(panel_element *Self)){
    Element->X=X;
    Element->Y=Y;
    Element->On_Update=On_Update;
    Element->On_Render=On_Render;
    Element->On_Destroy=On_Destroy;
    //todo add an onclick;
}