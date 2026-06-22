#include "asset_manager.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

typedef struct asset{
    asset_type type;
    union{
        sprite Sprite;
    } data;
    const char* Path;
}asset;
typedef struct asset_bucket{
    asset* Assets;
    size_t Count;
    size_t Cap;
} asset_bucket;
struct asset_manager{
    asset_bucket Buckets[ASSET_TYPE_COUNT];
};
asset* get_asset(asset_manager *Asset_Manager,asset_type Type,size_t id){
    if (id==0) return NULL;
    if(Type>=ASSET_TYPE_COUNT){
        GAVEN_WARN("Type Not Supported");
        return NULL;
    }
    if(id>Asset_Manager->Buckets[Type].Count){
        GAVEN_WARN("Couldnt find asset of id %d",id);
        return NULL;
    }
    return &Asset_Manager->Buckets[Type].Assets[id-1];
}
sprite get_sprite(asset_manager *Asset_Manager,size_t id){
    asset* A= get_asset(Asset_Manager,ASSET_TYPE_SPRITE,id);
    if(!A)
        return (sprite){0};
    return A->data.Sprite;
}
const char* get_scene_path(asset_manager *Asset_Manager,size_t id){
    asset *A =get_asset(Asset_Manager,ASSET_TYPE_SCENE,id);
    return A?A->Path:NULL;
}
asset_manager* create_asset_manager(void){
    asset_manager *Asset_Manager = (asset_manager*)malloc(sizeof(asset_manager));
    GAVEN_ASSERT(Asset_Manager,"Couldnt allocate memory to asset manager");
    for(int i=0;i<ASSET_TYPE_COUNT;i++){
        Asset_Manager->Buckets[i].Cap=512;
        Asset_Manager->Buckets[i].Count=0;
        Asset_Manager->Buckets[i].Assets=(asset*)(malloc(sizeof(asset)*Asset_Manager->Buckets[i].Cap));
        GAVEN_ASSERT(Asset_Manager->Buckets[i].Assets,"Couldnt allocate memory to asset manager bucket %d",i);
    }
    return Asset_Manager;
}
size_t add_sprite(asset_manager *Asset_Manager,sprite Sprite,const char *Path){
    asset Asset = {
        .data = Sprite,
        .type= ASSET_TYPE_SPRITE,
        .Path= Path
    };
    asset_bucket* B=&Asset_Manager->Buckets[ASSET_TYPE_SPRITE];
    B->Assets[B->Count++]=Asset;
    return B->Count;
}
size_t add_scene(asset_manager *Asset_Manager,const char* Path){
    asset Asset = {
        .data = {0},
        .Path=Path,
        .type= ASSET_TYPE_SCENE
    };
    asset_bucket* B=&Asset_Manager->Buckets[ASSET_TYPE_SCENE];
    B->Assets[B->Count++]=Asset;
    return B->Count;
}
size_t add_sprite_from_file(asset_manager *Asset_Manager,const char* Path){
    short width,y;
    char* data;
    char buffer[1024];
    FILE *f = fopen(Path,"r");
    GAVEN_ASSERT(f,"Unable to open file %s",Path);
    width=y=0;
    while (fgets(buffer,sizeof(buffer),f)){
        short len = strcspn(buffer,"\n");
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
    return add_sprite(Asset_Manager,Sprite,Path);
}
size_t add_asset_from_file(asset_manager *Asset_Manager,asset_type Type,const char* Path){    
    GAVEN_ASSERT(Type<ASSET_TYPE_COUNT,"Type Not Supported");
    asset_bucket* B=&Asset_Manager->Buckets[Type];
    if (B->Count>=B->Cap){
        B->Cap*=2;
        asset* temp = (asset*)realloc(B->Assets,sizeof(asset)*B->Cap);
        GAVEN_ASSERT(temp,"Couldnt allocate memory to Asset manager");
        B->Assets=temp;
    }
    switch(Type){
        case ASSET_TYPE_SPRITE: return add_sprite_from_file(Asset_Manager,Path);
        case ASSET_TYPE_SCENE: return add_scene(Asset_Manager,Path);
        default: GAVEN_ASSERT(0,"UNSUPPORTED ASSET TYPE %d",Type); return 0;
    }
}
void destroy_asset(asset* Asset){
    asset_type Type=Asset->type;
    switch(Type){
        case ASSET_TYPE_SPRITE: destroy_sprite(&Asset->data.Sprite); return;
        case ASSET_TYPE_SCENE: return;
        default: GAVEN_ASSERT(0,"UNSUPPORTED ASSET TYPE %d",Type); return;
    }

}
void destroy_asset_manager(asset_manager* Self){
    if(!Self) return;
    for(int i=0;i<ASSET_TYPE_COUNT;i++){
        asset_bucket *B= &Self->Buckets[i];
        for(size_t j=0;j<B->Count;j++){
            destroy_asset(&B->Assets[j]);
        }
        free(B->Assets);
    }
    free(Self);
}
void remove_asset_from_asset_manager(asset_manager* Manager,asset_type Type,size_t ID){
    if(!Manager) return;
    GAVEN_ASSERT(Type<ASSET_TYPE_COUNT,"Type Not Supported");
    asset_bucket* B= &Manager->Buckets[Type];
    if (ID==0||ID>B->Count) return;
    asset* Asset= &B->Assets[ID-1];
    destroy_asset(Asset);
    B->Assets[ID-1] = B->Assets[B->Count-1];
    B->Count--;
}
size_t find_asset_from_asset_manager_with_path(asset_manager* Manager,asset_type Type,const char* Path){
    if(!Manager) return -1;
    GAVEN_ASSERT(Type<ASSET_TYPE_COUNT,"Type Not Supported");
    asset_bucket* B=&Manager->Buckets[Type];
    for(size_t i =0;i<B->Count;i++){
        if(strcmp(B->Assets[i].Path,Path)==0)
            return i+1;
    }
    return -1;
}