#ifndef FILE_EXPLORER_H
#define FILE_EXPLORER_H
#include "../panel.h"
#include "../../editor.h"
panel* create_file_explorer(editor* Editor);
void file_explorer_point_to_inspector(panel* File_Explorer,panel* Inspector);
#endif