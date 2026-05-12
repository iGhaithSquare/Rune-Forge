#ifndef SCENE_EXPLORER_H
#define SCENE_EXPLORER_H
#include "panel_elements.h"
panel* create_scene_explorer(void);
void add_entity_registry_to_scene_explorer(entity_registry* Registry,panel* Scene_Explorer);
void scene_explorer_point_to_inspector(panel* Scene_Explorer,panel* Inspector);
#endif