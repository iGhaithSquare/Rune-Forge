#ifndef EDITOR_H
#define EDITOR_H
#ifdef _WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif
#include "runeforge.h"
void open_project(layer_registry* Registry,const char* Path);
#endif