#include "ui_text.h"
#include <nf_lib.h>

//prints to a ui text bubble on the top screen. 
void ui_print(const char *string)
{
    NF_WriteText(0, 0, 4, 17, string);
    NF_UpdateTextLayers();
}