#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H
#include <runewall.h>
typedef enum asset_type {
    ASSET_TYPE_SPRITE
}asset_type;
typedef struct asset_manager asset_manager;
asset_manager* create_asset_manager(void);
size_t add_asset(asset_manager* Manager,void* Asset,asset_type Type);
size_t add_asset_from_file(asset_manager* Manager,asset_type Type,const char* Path);

sprite get_sprite(asset_manager* Manager,size_t id);
void destroy_asset_manager(asset_manager* Self);
#endif