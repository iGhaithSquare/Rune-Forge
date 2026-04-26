#include "asset_manager.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

typedef struct asset{
    asset_type type;
    union{
        sprite Sprite;
    } data;
}asset;
struct asset_manager{
    asset* Assets;
    size_t Count;
    size_t Cap;
};

asset* get_asset(asset_manager* Manager,size_t id){
    GAVEN_ASSERT(id<Manager->Count,"Couldnt find asset of id %d",id);
    return &Manager->Assets[id];
}
sprite get_sprite(asset_manager* Manager,size_t id){
    return get_asset(Manager,id)->data.Sprite;
}
asset_manager* create_asset_manager(void){
    asset_manager* Manager = (asset_manager*)malloc(sizeof(asset_manager));
    Manager->Cap=512;
    Manager->Count=0;
    Manager->Assets=(asset*)(malloc(sizeof(asset)*Manager->Cap));
    return Manager;
}
size_t add_sprite(asset_manager* Manager,sprite Sprite){
    if (Manager->Count>=Manager->Cap){
        Manager->Cap*=2;
        asset* temp = (asset*)realloc(Manager->Assets,sizeof(asset)*Manager->Cap);
        GAVEN_ASSERT(temp,"Couldnt allocate memory to Asset manager");
        Manager->Assets=temp;
    }
    asset Asset = {
        .data = Sprite,
        .type= ASSET_TYPE_SPRITE
    };
    Manager->Assets[Manager->Count]=Asset;
    return Manager->Count++;
}

size_t add_asset(asset_manager* Manager,void* Asset,asset_type Type){
    switch(Type){
        case ASSET_TYPE_SPRITE: return add_sprite(Manager,*(sprite*)Asset);
        default: GAVEN_ASSERT(0,"UNSUPPORTED ASSET TYPE %d",Type); return 0;
    }
}
size_t add_sprite_from_file(asset_manager* Manager,const char* Path){
    short width,y;
    char* data;
    char buffer[1024];
    FILE *f = fopen(Path,"r");
    GAVEN_ASSERT(f,"Unable to open file %s",Path);
    width=y=0;
    while (fgets(buffer,sizeof(buffer),f)){
        size_t len = strcspn(buffer,"\n");
        if(len>width) width=len;
        y++;
    }
    GAVEN_ASSERT(width>0&&y>0,"File %s is empty",Path);
    rewind(f);
    data=(char*)malloc(width*y);
    GAVEN_ASSERT(data,"Couldnt allocate memory to sprite");
    y=0;
    while(fgets(buffer,sizeof(buffer),f)){
        size_t len =strcspn(buffer,"\n");
        char *row=data+y*width;
        memcpy(row,buffer,len);
        memset(row+len,' ',width-len);
        y++;
    }
    sprite Sprite = {
        .Data=data,
        .Height=y,
        .Width=width
    };
    fclose(f);
    return add_sprite(Manager,Sprite);
}
size_t add_asset_from_file(asset_manager* Manager,asset_type Type,const char* Path){
    if (Manager->Count>=Manager->Cap){
        Manager->Cap*=2;
        asset* temp = (asset*)realloc(Manager->Assets,sizeof(asset)*Manager->Cap);
        GAVEN_ASSERT(temp,"Couldnt allocate memory to Asset manager");
        Manager->Assets=temp;
    }
    switch(Type){
        case ASSET_TYPE_SPRITE: return add_sprite_from_file(Manager,Path);
        default: GAVEN_ASSERT(0,"UNSUPPORTED ASSET TYPE %d",Type); return 0;
    }
}