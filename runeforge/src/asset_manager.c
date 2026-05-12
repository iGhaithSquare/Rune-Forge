#include "asset_manager.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

typedef struct asset{
    asset_type type;
    union{
        sprite Sprite;
        const char* Path;
    } data;
}asset;
struct asset_manager{
    asset* Assets;
    size_t Count;
    size_t Cap;
};
asset* get_asset(asset_manager *Asset_Manager,size_t id){
    GAVEN_ASSERT(id<Asset_Manager->Count,"Couldnt find asset of id %d",id);
    return &Asset_Manager->Assets[id];
}
sprite get_sprite(asset_manager *Asset_Manager,size_t id){
    return get_asset(Asset_Manager,id)->data.Sprite;
}
const char* get_scene_path(asset_manager *Asset_Manager,size_t id){
    return get_asset(Asset_Manager,id)->data.Path;
}
asset_manager* create_asset_manager(void){
    asset_manager *Asset_Manager = (asset_manager*)malloc(sizeof(asset_manager));
    Asset_Manager->Cap=512;
    Asset_Manager->Count=0;
    Asset_Manager->Assets=(asset*)(malloc(sizeof(asset)*Asset_Manager->Cap));
    return Asset_Manager;
}
size_t add_sprite(asset_manager *Asset_Manager,sprite Sprite){
    asset Asset = {
        .data = Sprite,
        .type= ASSET_TYPE_SPRITE
    };
    Asset_Manager->Assets[Asset_Manager->Count]=Asset;
    return Asset_Manager->Count++;
}
size_t add_path(asset_manager *Asset_Manager,const char* Path){
    asset Asset = {
        .data = Path,
        .type= ASSET_TYPE_SCENE
    };
    Asset_Manager->Assets[Asset_Manager->Count]=Asset;
    return Asset_Manager->Count++;
}
size_t add_asset(asset_manager *Asset_Manager,void* Asset,asset_type Type){
    if (Asset_Manager->Count>=Asset_Manager->Cap){
        Asset_Manager->Cap*=2;
        asset* temp = (asset*)realloc(Asset_Manager->Assets,sizeof(asset)*Asset_Manager->Cap);
        GAVEN_ASSERT(temp,"Couldnt allocate memory to Asset manager");
        Asset_Manager->Assets=temp;
    }
    switch(Type){
        case ASSET_TYPE_SPRITE: return add_sprite(Asset_Manager,*(sprite*)Asset);
        case ASSET_TYPE_SCENE: return add_path(Asset_Manager,*(char**)Asset);
        default: GAVEN_ASSERT(0,"UNSUPPORTED ASSET TYPE %d",Type); return 0;
    }
}
size_t add_sprite_from_file(asset_manager *Asset_Manager,const char* Path){
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
    return add_sprite(Asset_Manager,Sprite);
}
size_t add_asset_from_file(asset_manager *Asset_Manager,asset_type Type,const char* Path){
    if (Asset_Manager->Count>=Asset_Manager->Cap){
        Asset_Manager->Cap*=2;
        asset* temp = (asset*)realloc(Asset_Manager->Assets,sizeof(asset)*Asset_Manager->Cap);
        GAVEN_ASSERT(temp,"Couldnt allocate memory to Asset manager");
        Asset_Manager->Assets=temp;
    }
    switch(Type){
        case ASSET_TYPE_SPRITE: return add_sprite_from_file(Asset_Manager,Path);
        case ASSET_TYPE_SCENE: return add_path(Asset_Manager,Path);
        default: GAVEN_ASSERT(0,"UNSUPPORTED ASSET TYPE %d",Type); return 0;
    }
}
void destroy_asset_manager(asset_manager* Self){
    if(!Self) return;
    free(Self->Assets);
    free(Self);
}