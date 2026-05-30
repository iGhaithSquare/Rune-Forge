#ifndef PANEL_H
#define PANEL_H
#include <stdint.h>
#include <stddef.h>
#include <runeforge.h>
typedef struct panel panel;
typedef struct panel_element panel_element;
typedef struct panel_data {
    const char* Name;
    short Min_Width;
    short Min_Height;
    short Width;
    short Height;
    char Background_Char;
    uint8_t Is_Resizable; 
    uint8_t Anchor; // bit 0 for right bit 1 for down bit 2 for left bit 3 for top bit
    short Z_Index;
}panel_data;
typedef struct panel_neighbors panel_neighbors;
struct panel_neighbors{
    panel *Left[4];
    uint8_t Left_Count;
    panel *Right[4];
    uint8_t Right_Count;
    panel *Top[4];
    uint8_t Top_Count;
    panel *Bottom[4];
    uint8_t Bottom_Count;
};
typedef struct panel_registry{
    panel** Panels;
    size_t Count;
    size_t Cap;
    panel* Focused;
    panel* Hovered;
}panel_registry;
struct panel{
    panel_data Data;
    size_t ID;
    uint8_t Is_Resizing;
    uint8_t Is_Dirty;
    uint8_t Remove;
    char *Background_String;
    sprite Background_Sprite;
    panel_neighbors Panel_Neighbors;
    panel_element **Elements;
    size_t Count;
    size_t Cap;
    short X;
    short Y;
    panel_registry* Registry;
};
panel_registry* create_panel_registry();
void destroy_panel_registry(panel_registry* Self);
panel* create_panel(panel_data Data);
/* Direction: 0 for right,1 for bottom 2 for left, 3 for top*/
uint8_t add_panel_neighbor(panel* Panel,panel* Neighbor,uint8_t direction);
void add_panel_to_registry(panel* Panel,panel_registry* Registry);
void update_panels(panel_registry* Registry);
void render_panels(panel_registry* Registry);
void add_element_to_panel(panel* Panel,panel_element* Element);
void remove_element_from_panel(panel* Panel,panel_element* Element);
#endif