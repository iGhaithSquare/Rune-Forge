#ifndef RUNEFORGELAYER_H
#define RUNEFORGELAYER_H
#include "runewall.h"
#include "entity_registry.h"
void game_main(application* app,int argc, char** argv);
void set_main_scene(const char *path);
void draw_game_sprite(sprite Sprite,short X,short Y,short Z);
void draw_game_overlay_sprite(sprite Sprite,short X,short Y,short Z);
void set_window_size(short Width,short Height);
void set_panel_offset(short X,short Y);
sprite get_game_sprite(size_t id);
short get_window_width(void);
short get_window_height(void);
entity_registry* load_scene(const char* path);
void save_scene(const char* Path,const char* Name);
void unload_scene(void);
#endif