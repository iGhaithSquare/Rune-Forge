#include "popup_panel.h"
#include "panel_button.h"
panel* create_popup_panel(const char* Name,short X,short Y,short Width,short Height){
    panel_data Data = {
        .Name=Name,
        .Anchor=0,
        .Background_Char='`',
        .Height=Height,
        .Width=Width,
        .Is_Resizable=0,
        .Z_Index=2,
        .Min_Height=Height-1,
        .Min_Width=Width-1
    };
    panel* Popup = create_panel(Data);
    Popup->X=X;
    Popup->Y=Y;
    return Popup;
}
