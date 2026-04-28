#ifndef RUNEFORGELAYER_H
#define RUNEFORGELAYER_H
#include "runewall.h"
typedef enum{
    SCENE_EDIT,
    SCENE_LOAD
} SCENE_MODE;
void set_scene_mode(SCENE_MODE mod);
void game_main(int argc, char** argv);
void set_main_scene(const char *path);
void draw_game_sprite(sprite Sprite,short X,short Y,short Z);
sprite get_game_sprite(size_t id);
#endif