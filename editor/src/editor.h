#ifndef EDITOR_H
#define EDITOR_H
#ifdef _WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif
#include "runeforge.h"
void open_project(layer_registry* Registry,const char* Path);
void write_file(const char* Path,const char* Content);
void append_file(const char* Path,const char* Content);
uint8_t get_current_path(char* Buffer,size_t Size);
const char* get_projecta_file(void);
#endif