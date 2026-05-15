#ifndef RUNEFORGELAYER_H
#define RUNEFORGELAYER_H
#include "runewall.h"
#include "entity_registry.h"
#include "asset_types.h"
RUNEFORGE_API void set_main_scene(const char *path);
RUNEFORGE_API void draw_game_sprite(sprite Sprite,short X,short Y,short Z);
RUNEFORGE_API void draw_game_overlay_sprite(sprite Sprite,short X,short Y,short Z);
RUNEFORGE_API void set_window_size(short Width,short Height);
RUNEFORGE_API void set_panel_offset(short X,short Y);
RUNEFORGE_API sprite get_game_sprite(size_t id);
RUNEFORGE_API short get_window_width(void);
RUNEFORGE_API short get_window_height(void);
RUNEFORGE_API entity_registry* load_scene(const char* path);
RUNEFORGE_API void save_scene(const char* Path,const char* Name);
RUNEFORGE_API void unload_scene(void);
RUNEFORGE_API void change_update_state(uint8_t new_state);
RUNEFORGE_API uint8_t get_state(void);
RUNEFORGE_API void remove_asset(size_t ID);
RUNEFORGE_API application* runeforge_main(void);
RUNEFORGE_API size_t load_game_asset(const char* Path,asset_type Type);
RUNEFORGE_API void add_entity(entity* e);
RUNEFORGE_API size_t get_asset_id_from_path(const char* Path);

#endif