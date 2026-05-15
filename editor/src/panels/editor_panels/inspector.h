#ifndef INSPECTOR_H
#define INSPECTOR_H
#include "../panel_elements.h"
panel* create_inspector(void);
void inspect_entity(entity* Entity,panel* Inspector_Panel);
void inspect_asset(size_t Asset_ID,panel* Inspector_Panel,asset_type Type);
#endif