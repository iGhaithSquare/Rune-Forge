#ifndef NAVBAR_H
#define NAVBAR_H
#include "../panel.h"
#include "../../editor.h"
panel* create_navbar(editor* Editor);
void navbar_add_inspector(panel* Self,panel* Inspector);
#endif