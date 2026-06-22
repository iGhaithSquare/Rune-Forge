#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H
#include <runewall.h>
#include "asset_types.h"
typedef struct asset_manager asset_manager;
asset_manager* create_asset_manager(void);

size_t add_asset_from_file(asset_manager *Asset_Manager,asset_type Type,const char* Path);
sprite get_sprite(asset_manager *Asset_Manager,size_t id);
const char* get_scene_path(asset_manager *Asset_Manager,size_t id);
void destroy_asset_manager(asset_manager* Self);
void remove_asset_from_asset_manager(asset_manager* Manager,asset_type Type,size_t ID);
size_t find_asset_from_asset_manager_with_path(asset_manager* Manager,asset_type Type,const char* Path);
#endif