#include "file_explorer.h"
#include <stdlib.h>
#include <string.h>
#include "panel_button.h"
#ifdef _WIN32
#include <windows.h>
#ifndef PATH_MAX
#define PATH_MAX 260
#endif
#else
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
typedef struct file_node{
    char* File_Path;
    sprite Text_Sprite;
    struct file_node* Parent;
    struct file_node** Children;
    size_t Count;
    size_t Cap;
    uint8_t Data; //Bit 0 for is directory, Bit 1 for is expanded
}file_node;
typedef struct file_explorer_button_data{
    file_node* Selected;
    asset_type Type;
}file_explorer_button_data;
typedef struct file_explorer_element{
    panel_element Base;
    file_node* Root;
    file_node* Selected;
    sprite Back_Sprite;
    file_explorer_button_data* Button1_Data;
}file_explorer_element;
const char* get_file_extension(const char* Path){
    const char* dot =strrchr(Path,'.');
    if(!dot||dot==Path) return NULL;
    return dot+1;
}
asset_type get_asset_type(const char* Path){
    const char* Extension = get_file_extension(Path);
    if(!Extension) return -1;
    if(!strcmp(Extension,"jsons")) return ASSET_TYPE_SPRITE;
    if(!strcmp(Extension,"json")) return ASSET_TYPE_SCENE;
    return -1;
}
void file_button_load_scene_impl(file_node* Node){
    unload_scene();
    load_scene(Node->File_Path);
}
void file_button_press_impl(panel_button* Self){
    if(get_state()&1) return;
    file_explorer_button_data* Data= (file_explorer_button_data*)Self->Button_Data;
    switch (Data->Type){
    case ASSET_TYPE_SCENE:
        file_button_load_scene_impl(Data->Selected);
        break;
    default:
        break;
    }
}
void update_file_explorer_element(panel_element* Self){
    file_explorer_element* fe = (file_explorer_element*)Self;
    panel* Panel = Self->Parent;
    panel_button* Button1 = (panel_button*)Panel->Elements[1];
    if(Panel->Is_Focused&&Panel->Is_Hovered){
        if(is_key_just_pressed(RUNEFORGE_MOUSE_BUTTON_LEFT)){
            short MX = get_mouse_X()-Panel->X;
            short MY = get_mouse_Y()-Panel->Y;
            int offset=1;
            file_node* Node = NULL;
            if(fe->Root->Parent)
                offset=2;
            if(MY-offset>=0&&MY-offset<(short)(fe->Root->Count)){
                Node=fe->Root->Children[MY-offset];
            }
            if(offset==2&&MX>=3&&MY==1&&MX<fe->Back_Sprite.Width+3){
                fe->Root=fe->Root->Parent;
                fe->Selected=NULL;
            }
            else if (Node&&MX>=3&&MX<Node->Text_Sprite.Width+3){
                if(fe->Selected==Node&&Node->Data&1){
                    fe->Selected=NULL;
                    fe->Root=Node;
                }
                fe->Selected = Node;
                file_explorer_button_data* Data = (file_explorer_button_data*)Button1->Button_Data;
                Data->Selected=Node;
                Data->Type=get_asset_type(Node->File_Path);
                switch(Data->Type){
                    case ASSET_TYPE_SCENE:
                        Button1->Text="Load Scene";
                        break;
                    default:
                        Button1->Text="FILE EXPLORER";
                }
                Button1->Is_Dirty=1;
            }

        }
        
    }
    else if (is_key_just_pressed(RUNEFORGE_MOUSE_BUTTON_LEFT)){
        fe->Selected=NULL;
    }
}
void render_file_explorer_element(panel_element* Self){
    file_explorer_element* fe = (file_explorer_element*)Self;
    int offset=1;
    if(fe->Root->Parent){
        draw_game_overlay_sprite(fe->Back_Sprite,3,1,1);
        offset=2;
    }
    for (size_t i=0;i<fe->Root->Count;i++){
        file_node *Node=fe->Root->Children[i];
        draw_game_overlay_sprite(Node->Text_Sprite,3,i+offset,1);
    }
}
void destroy_file_node(file_node* Node){
    for(size_t i=0;i<Node->Count;i++){
        destroy_file_node(Node->Children[i]);
    }
    free(Node->File_Path);
    free(Node->Children);
    destroy_sprite(&Node->Text_Sprite);
    free(Node);
}
void destroy_file_explorer_element(panel_element* Self){
    file_explorer_element* fe = (file_explorer_element*)Self;
    destroy_sprite(&fe->Back_Sprite);
    destroy_file_node(fe->Root);
    free(fe->Button1_Data);
    return;
}
void add_child_file_node(file_node* Parent,file_node* Child){
    if(Parent->Count>=Parent->Cap){
        Parent->Cap=Parent->Cap?Parent->Cap*2:8;
        file_node** temp= (file_node**)realloc(Parent->Children,sizeof(file_node*)*Parent->Cap);
        GAVEN_ASSERT(temp,"Couldnt allocate memory to file node");
        Parent->Children=temp;
    }
    Parent->Children[Parent->Count++]=Child;
    Child->Parent=Parent;
}
file_node* create_filenode(const char* Name,const char* File_Path,short length,uint8_t Is_directory){
    file_node* node = calloc(1,sizeof(file_node));
    GAVEN_ASSERT(node,"Couldnt allocate memory to file node");
    node->Data=Is_directory;
    size_t Name_Len =strlen(Name);
    if(Name_Len+2>(size_t)length)
        Name_Len=length-2;
    char* buffer = malloc(length+2);
    memset(buffer,' ',(size_t)length);
    GAVEN_ASSERT(buffer,"Couldnt allocate memory to string");    
    memcpy(buffer+1,Name,Name_Len);
    buffer[0]='|';
    if(Is_directory)buffer[Name_Len+1]='/';
    node->Text_Sprite=create_text(buffer,length);
    free(buffer);
    node->Children=NULL;
    node->File_Path=strdup(File_Path);
    node->Cap=0;
    node->Count=0;
    node->Parent=NULL;
    return node;
}
file_node* build_filenode_tree(const char* Path,file_node* Parent,short Length){
    const char* Name;
    const char* a =strrchr(Path,'\\');
    const char* b =strrchr(Path,'/');
    Name=a>b?a+1:b+1;
    file_node *Root=Parent;
    if(!Root) Root=create_filenode(Name,Path,Length,1);
    size_t Len;
    size_t Search_Len= Len=strlen(Path);
    #ifdef _WIN32
    char Search_Path[PATH_MAX];
    memcpy(Search_Path,Path,Len);
    if(Search_Len&&Path[Search_Len-1]!='\\'&&Path[Search_Len]!='/')
        Search_Path[Search_Len++]='\\';
    Search_Path[Search_Len]='*';
    Search_Path[Search_Len+1]='\0';
    WIN32_FIND_DATA fd;
    HANDLE h = FindFirstFileA(Search_Path,&fd);
    if(h==INVALID_HANDLE_VALUE) return Root;
    do{
        const char* entry= fd.cFileName;
        if(strcmp(entry,".")==0||strcmp(entry,"..")==0)continue;
        size_t Entry_Len=strlen(entry);
        char* Full_Path=(char*)malloc(Entry_Len+Len+2);
        GAVEN_ASSERT(Full_Path,"Couldnt allocate memory to building file node tree");
        memcpy(Full_Path,Path,Len);
        if(Len&&Path[Len-1]!='\\'&&Path[Len-1]!='/'){
            Full_Path[Len]='\\';
            memcpy(Full_Path+Len+1,entry,Entry_Len+1);
        }
        else
            memcpy(Full_Path+Len,entry,Entry_Len+1);
        uint8_t is_dir=(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)&&!(fd.dwFileAttributes&FILE_ATTRIBUTE_REPARSE_POINT);
        file_node* Child = create_filenode(entry,Full_Path,Length,is_dir);
        add_child_file_node(Root,Child);
        if(is_dir){
            build_filenode_tree(Full_Path,Child,Length);
        }
        free(Full_Path);
    }while(FindNextFileA(h,&fd));
    FindClose(h);
    #else
    DIR* dir =opendir(Path);
    if(!dir) return Root;
    struct dirent* ent;
    while((ent=readdir(dir))!=NULL){
        const char* entry= ent->d_name;
        if(strcmp(entry,".")==0||strcmp(entry,"..")==0)continue;
        size_t Entry_Len=strlen(entry);
        char* Full_Path=(char*)malloc(Entry_Len+Len+2);
        GAVEN_ASSERT(Full_Path,"Couldnt allocate memory to building file node tree");
        memcpy(Full_Path,Path,Len);
        if(Len&&Path[Len-1]!='\\'&&Path[Len-1]!='/'){
            Full_Path[Len]='/';
            memcpy(Full_Path+Len+1,entry,Entry_Len+1);
        }
        else
            memcpy(Full_Path+Len,entry,Entry_Len+1);
        struct stat st;
        uint8_t is_dir=lstat(Full_Path,&st)?0:S_ISDIR(st.st_mode);
        file_node* Child = create_filenode(entry,Length,is_dir);
        add_child_file_node(Root,Child);
        if(is_dir){
            build_filenode_tree(Full_Path,Child,Length);
        }
        free(Full_Path);
    };
    closedir(dir);
    #endif
    return Root;
}

file_explorer_element* create_file_explorer_element(const char* Path, short Length){
    file_explorer_element* Element =(file_explorer_element*)malloc(sizeof(file_explorer_element));
    GAVEN_ASSERT(Element,"Couldnt allocat enough memory to file explorer");
    Element->Back_Sprite=create_text("|->Return",9);
    Element->Root=build_filenode_tree(Path,NULL,Length);
    Element->Selected=NULL;
    Element->Button1_Data=(file_explorer_button_data*)malloc(sizeof(file_explorer_button_data));
    init_panel_element_base(&Element->Base,0,0,update_file_explorer_element,render_file_explorer_element,destroy_file_explorer_element);
    return Element;
}
panel* create_file_explorer(void){
    panel_data File_expo={
        .Name="File Explorer",
        .Background_Char='\'',
        .Height=9,
        .Width=80,
        .Min_Height=9,
        .Min_Width=80,
        .Anchor=15, //anchored all
        .Is_Resizable=0,
        .Z_Index=0
    };
    panel *File_Explorer = create_panel(File_expo);
    file_explorer_element *E=create_file_explorer_element("assets/",74);
    panel_button *Button= create_panel_button(60,0,"FILE EXPLORER",20,E->Button1_Data,file_button_press_impl);
    add_element_to_panel(File_Explorer,&E->Base);
    add_element_to_panel(File_Explorer,&Button->Base);
    return File_Explorer;
}