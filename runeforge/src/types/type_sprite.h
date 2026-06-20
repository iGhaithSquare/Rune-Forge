#ifndef SPRITE_TYPE_H
#define SPRITE_TYPE_H
#include "type_entity2d.h"
typedef struct type_sprite{
    type_entity2d Base;
    size_t SpriteID;
    uint8_t Dirty;
}type_sprite;
extern type_info Type_Sprite;
#endif