#ifndef PANEL_H
#define PANEL_H
#include <stdint.h>
#include <stddef.h>
#include <runeforge.h>
typedef struct panel panel;
typedef struct panel_element panel_element;
typedef struct panel_data {
    const char* Name;
    short X;
    short Y;
    short Min_Width;
    short Min_Height;
    short Width;
    short Height;
    char Background_Char;
    uint8_t Is_Resizable;
    uint8_t Is_Draggable;
}panel_data;
struct panel{
    panel_data Data;
    size_t ID;
    uint8_t Is_Hovered;
    uint8_t Is_Focused;
    uint8_t Is_Resizing;
    uint8_t Is_Dragging;
    uint8_t Is_Dirty;
    char *Background_String;
    sprite Background_Sprite;
    panel_element** Elements;
    size_t Count;
    size_t Cap;
};
typedef struct panel_registry{
    panel** Panels;
    size_t Count;
    size_t Cap;
}panel_registry;

panel_registry* create_panel_registry();
void destroy_panel_registry(panel_registry* Self);
panel* create_panel(panel_data Data);
void add_panel_to_registry(panel* Panel,panel_registry* Registry);
void remove_panel_from_registry(panel* Panel,panel_registry* Registry);
void update_panels(panel_registry* Registry);
void render_panels(panel_registry* Registry);
void add_element_to_panel(panel* Panel,panel_element* Element);
void remove_element_from_panel(panel* Panel,panel_element* Element);
#endif