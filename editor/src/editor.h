#ifndef EDITOR_H
#define EDITOR_H
#ifdef _WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif
#include "runeforge.h"
typedef struct editor editor;
typedef struct editor_cfg editor_cfg;
struct editor_cfg{
    char Recent_Projects[16][512];
    char Recent_Project_Names[16][32];
    char Project_Count;
};
struct editor{
    editor_cfg CFG;
    char project_file_path[512];
    char project_root_path[512];
    char project_name[32];
    layer_registry* Layer_Registry;
    entity_registry* Entity_Registry;
};

void load_editor_cfg(editor* E);
void save_editor_cfg(editor* E);
editor* create_editor(layer_registry* Registry);
void open_project(editor* Editor,const char* Path);
void write_file(const char* Path,const char* Content);
void append_file(const char* Path,const char* Content);
uint8_t get_current_path(char* Buffer,size_t Size);
void poll_dll(editor* Editor);
uint8_t file_exists(const char* Path);
void get_editor_path(char* Buffer,size_t Size);
uint8_t check_if_folder_exists(const char* path);
void create_new_folder(const char* path);
void copy_file_to_dir(const char* Source,const char* Dir);
void copy_file(const char* Source,const char* Destination);
void copy_dir_to_dir(const char* Source,const char* Destination);
#endif