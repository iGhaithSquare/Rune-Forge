#include "file_explorer.h"
#include <stdlib.h>
#include <string.h>
#include "../panel_button.h"
#include "inspector.h"
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#ifndef PATH_MAX
#define PATH_MAX 260
#endif
#else
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/inotify.h>
#endif
static char asset_dir[512];
typedef struct file_node{
    char* File_Path;
    sprite Text_Sprite;
    struct file_node* Parent;
    struct file_node** Children;
    size_t Count;
    size_t Cap;
    uint8_t Data; //Bit 0 for is directory, Bit 1 for is expanded
}file_node;
typedef struct file_explorer_element file_explorer_element;
typedef struct file_explorer_button_data{
    file_node* Selected;
    asset_type Type;
    size_t Data;
    panel* Inspector;
    editor* Editor;
}file_explorer_button_data;
typedef struct file_explorer_element{
    panel_element Base;
    file_node* Root;
    file_node* Current;
    file_node* Selected;
    sprite Back_Sprite;
    file_explorer_button_data* Button1_Data;
    #ifdef _WIN32
    HANDLE Watch_Handle;
    OVERLAPPED Overlapped;
    char Watch_Buffer[1];
    #else
    int Watch_FD;
    #endif
}file_explorer_element;
const char* get_file_extension(const char* Path){
    const char* dot =strrchr(Path,'.');
    if(!dot||dot==Path) return NULL;
    return dot+1;
}
asset_type get_asset_type(const char* Path){
    const char* Extension = get_file_extension(Path);
    if(!Extension) return -1;
    if(!strcmp(Extension,"txts")) return ASSET_TYPE_SPRITE;
    if(!strcmp(Extension,"jsonscn")) return ASSET_TYPE_SCENE;
    return -1;
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
char* get_normalize_path(const char* File_Path){
    char* Normalized =strdup(File_Path);
    for(int i=0;Normalized[i];i++)
        if(Normalized[i]=='\\')
            Normalized[i]='/';
    return Normalized;
}
file_node* create_filenode(const char* Name,const char* File_Path,short length,uint8_t Is_directory){
    file_node* node = calloc(1,sizeof(file_node));
    GAVEN_ASSERT(node,"Couldnt allocate memory to file node");
    node->Data=Is_directory;
    node->File_Path=get_normalize_path(File_Path);
    size_t Name_Len =strlen(Name);
    if(Name_Len+2>(size_t)length)
        Name_Len=length-2;
    char* buffer = malloc(length+2);
    memset(buffer,' ',(size_t)length);
    GAVEN_ASSERT(buffer,"Couldnt allocate memory to string");    
    memcpy(buffer+1,Name,Name_Len);
    buffer[0]='|';
    if(Is_directory){
        buffer[Name_Len+1]='/';
    }
    node->Text_Sprite=create_text(buffer,length);
    free(buffer);
    node->Children=NULL;
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

void update_button_selected(panel_button *Button){
    file_explorer_button_data* Data = (file_explorer_button_data*)Button->Button_Data;
    switch(Data->Type){
        case ASSET_TYPE_SCENE:
            Button->Text="Load Scene";
            break;
        case ASSET_TYPE_SPRITE:
            Data->Data=get_asset_id_from_path(Data->Selected->File_Path,ASSET_TYPE_SPRITE);
            if(Data->Data==-1)
                Button->Text="Load Sprite";
            else
                Button->Text="Inspect Sprite";
            break;
    default:
        Button->Text="FILE EXPLORER";
    }
    Button->Is_Dirty=1;
}
void file_button_load_scene_impl(panel_button* Self){
    file_explorer_button_data* Data= (file_explorer_button_data*)Self->Button_Data;
    uninspect_inspector_panel(Data->Inspector);
    unload_scene();
    load_scene(Data->Selected->File_Path);
}
void file_button_load_sprite_impl(panel_button* Self){
    file_explorer_button_data* Data= (file_explorer_button_data*)Self->Button_Data;
    const char* Path=Data->Selected->File_Path;
    Data->Data=load_game_asset(Path,ASSET_TYPE_SPRITE);
    char content[128];
    update_button_selected(Self);
    snprintf(content,sizeof(content),"\nasset=%d,%s",ASSET_TYPE_SPRITE,Path);
    append_file(Data->Editor->project_file_path,content);
}
void file_button_inspect_sprite_impl(panel_button* Self){
    file_explorer_button_data* Data= (file_explorer_button_data*)Self->Button_Data;
    inspect_asset(Data->Data,Data->Inspector,Data->Type);
}
void file_button_press_impl(panel_button* Self){
    if(get_state()&1) return;
    file_explorer_button_data* Data= (file_explorer_button_data*)Self->Button_Data;
    switch (Data->Type){
    case ASSET_TYPE_SCENE:
        file_button_load_scene_impl(Self);
        break;
    case ASSET_TYPE_SPRITE:
        if(Data->Data==-1){
            file_button_load_sprite_impl(Self);
        }
        else{
            file_button_inspect_sprite_impl(Self);
        }
    default:
        break;
    }
}

void poll_root_node(file_explorer_element* FE){
    if(!FE||!FE->Root) return;
    size_t i;
    #ifdef _WIN32
    DWORD bytes=0;
    if(!GetOverlappedResult(FE->Watch_Handle,&FE->Overlapped,&bytes,FALSE)){
        return;
    }
    memset(&FE->Overlapped,0,sizeof(OVERLAPPED));
    ReadDirectoryChangesW(FE->Watch_Handle,FE->Watch_Buffer,1,TRUE,FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|FILE_NOTIFY_CHANGE_LAST_WRITE,NULL,&FE->Overlapped,NULL);
    #else
    char *buffer =malloc(sizeof(struct inoti));
    ssize_t len =read(FE->Watch_FD,buffer,sizeof(buffer));
    free(buffer);
    if(len<=0)
        return;
    #endif
    file_node* Root=FE->Root;
    FE->Root=build_filenode_tree(Root->File_Path,NULL,74);
    FE->Current=FE->Root;
    FE->Selected=NULL;
    FE->Button1_Data->Selected=NULL;
    destroy_file_node(Root);
}
void update_file_explorer_element(panel_element* Self){
    file_explorer_element* fe = (file_explorer_element*)Self;
    panel* Panel = Self->Parent;
    panel_button* Button1 = (panel_button*)Panel->Elements[1];
    poll_root_node(fe);
    if(Panel==Panel->Registry->Focused&&Panel==Panel->Registry->Hovered){
        if(is_key_just_pressed(RUNEFORGE_MOUSE_BUTTON_LEFT)){
            short MX = get_mouse_X()-Panel->X;
            short MY = get_mouse_Y()-Panel->Y;
            int offset=1;
            file_node* Node = NULL;
            if(fe->Current->Parent)
                offset=2;
            if(MY-offset>=0&&MY-offset<(short)(fe->Current->Count)){
                Node=fe->Current->Children[MY-offset];
            }
            if(offset==2&&MX>=3&&MY==1&&MX<fe->Back_Sprite.Width+3){
                fe->Current=fe->Current->Parent;
                fe->Selected=NULL;
            }
            else if (Node&&MX>=3&&MX<Node->Text_Sprite.Width+3){
                if(fe->Selected==Node&&Node->Data&1){
                    fe->Selected=NULL;
                    fe->Current=Node;
                }
                fe->Selected = Node;
                file_explorer_button_data* Data = (file_explorer_button_data*)Button1->Button_Data;
                Data->Selected=Node;
                Data->Type=get_asset_type(Node->File_Path);
                update_button_selected(Button1);
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
    if(fe->Current->Parent){
        draw_game_overlay_sprite(fe->Back_Sprite,3,1,1);
        offset=2;
    }
    for (size_t i=0;i<fe->Current->Count;i++){
        file_node *Node=fe->Current->Children[i];
        draw_game_overlay_sprite(Node->Text_Sprite,3,i+offset,Self->Parent->Data.Z_Index+1);
    }
}
file_explorer_element* create_file_explorer_element(editor* Editor,const char* Path, short Length){
    
    file_explorer_element* Element =(file_explorer_element*)malloc(sizeof(file_explorer_element));
    GAVEN_ASSERT(Element,"Couldnt allocat enough memory to file explorer");
    Element->Back_Sprite=create_text("|->Return",9);
    Element->Root=Element->Current=build_filenode_tree(Path,NULL,Length);
    Element->Selected=NULL;
    Element->Button1_Data=(file_explorer_button_data*)malloc(sizeof(file_explorer_button_data));
    Element->Button1_Data->Inspector=NULL;
    Element->Button1_Data->Editor=Editor;
    init_panel_element_base(&Element->Base,0,0,update_file_explorer_element,render_file_explorer_element,destroy_file_explorer_element);
    #ifdef _WIN32
    Element->Watch_Handle=CreateFileA(Element->Root->File_Path,FILE_LIST_DIRECTORY,FILE_SHARE_DELETE|FILE_SHARE_WRITE|FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED,NULL);
    memset(&Element->Overlapped,0,sizeof(OVERLAPPED));
    ReadDirectoryChangesW(Element->Watch_Handle,Element->Watch_Buffer,1,TRUE,FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|FILE_NOTIFY_CHANGE_LAST_WRITE,NULL,&Element->Overlapped,NULL);
    #else
    Element->Watch_FD=inotify_init1(IN_NONBLOCKING);
    inotify_add_watch(Element->Watch_FD,Element->Root->File_Path,IN_CREATE|IN_DELETE|IN_MODIFY|IN_MOVED_FROM|IN_MOVED_TO);
    #endif
    
    return Element;
}
panel* create_file_explorer(editor* Editor){
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
    snprintf(asset_dir,sizeof(asset_dir),"%s%s%s%s",Editor->project_root_path,PATH_SEP,"assets",PATH_SEP);
    file_explorer_element *E=create_file_explorer_element(Editor,asset_dir,74);

    panel_button *Button= create_panel_button(60,0,"FILE EXPLORER",20,E->Button1_Data,file_button_press_impl);
    add_element_to_panel(File_Explorer,&E->Base);
    add_element_to_panel(File_Explorer,&Button->Base);
    return File_Explorer;
}
void file_explorer_point_to_inspector(panel* File_Explorer,panel* Inspector){
    if(!File_Explorer||File_Explorer->Cap<=0) return;
    file_explorer_element* fe = (file_explorer_element*)File_Explorer->Elements[0];
    fe->Button1_Data->Inspector=Inspector;
}
const char* get_asset_dir(void){
    return asset_dir;
}