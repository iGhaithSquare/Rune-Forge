#ifndef RUNEFORGE_H
#define RUNEFORGE_H
#include <stddef.h>
#include <stdint.h>
#include "../src/runeforge_layer.h"
#include "../src/input.h"
typedef enum asset_type {
    ASSET_TYPE_SPRITE
}asset_type;
create_layer_phase(Update,2);
size_t load_game_asset(const char* Path,asset_type Type);
void add_entity(entity* e);
#endif