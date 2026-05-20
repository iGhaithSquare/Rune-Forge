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
void poll_dll(void);
uint8_t file_exists(const char* Path);
void get_editor_path(char* Buffer,size_t Size);
uint8_t check_if_folder_exists(const char* path);
void create_new_folder(const char* path);
const char* get_project_name(void);
const char* get_project_path(void);
void copy_file_to_dir(const char* Source,const char* Dir);
void copy_file(const char* Source,const char* Destination);
void copy_dir_to_dir(const char* Source,const char* Destination);
#endif