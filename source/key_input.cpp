
#include <nds.h>
#include <nds/arm9/input.h>
#include <nds/arm9/sound.h>
#include <nds/arm9/sprite.h>
#include <nf_lib.h>
#include <stdio.h>
#include "key_input.h"
#include "main.h"
#include "character.h"
#include "mem_slots.h"
#include "ui_text.h"

// 1024x1024 pixel map, therefore 32x32 tiles
// each value represents the default collision status for each tile
u8 collision[32][32] = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // 0
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // 1
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // 2
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // 3
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // 4
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // 5
                        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},  // 6
                        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},  // 7
                        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},  // 8
                        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},  // 9
                        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},  // 10
                        {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},  // 11
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},  // 12
                        {1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},  // 13
                        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},  // 14
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},  // 15
                        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},  // 16
                        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // 17
                        {1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},  // 18
                        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},  // 19
                        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},  // 20
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},  // 21
                        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},  // 22
                        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},  // 23
                        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},  // 24
                        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},  // 25
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},  // 26
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},  // 27
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // 28
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // 29
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // 30
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}; // 31

// number index of character selected
int character_selected = ALEX;
const char *players[] = {"alex", "mike", "mittens", "vex", "shibe", "athena", "faelyn", "glen_4"};

// if text bubble layer is showing
int text_bubble_showing = 0;

// current sentence that is being shown in dialogue
// STARTS AT 1
int current_sentence = 1;

// last direction the character tried to go in, whether there was colission or not
int last_direction = KEY_DOWN;

// the button highlighted on the pause menu. 0=btg, 1=quit
int pause_selection = 0;

// the button highlighted on the main menu. 0=single, 1=multi, 2=options
int menu_select = 0;

// if the user spams a, this is the minimum amount of vblanks between teleports there must be
int teleport_limit = 25;

// handle when buttons are pressed while playing game
void set_keys_in_game(int touch_pos_x, int touch_pos_y)
{

    if (keysHeld() & KEY_START)
    {
        current_vblank = vblank;
        if (current_vblank - last_vblank > 20)
        {
            if (!paused)
            {
                paused = 1;
                pause_selection = 0;
                NF_DeleteSprite(1, BOTTOM_OAM_0);
                NF_DeleteSprite(1, last_l_sprite);
                NF_DeleteSprite(1, last_r_sprite);
                NF_DeleteSprite(1, BOTTOM_OAM_9);
                NF_DeleteSprite(1, BOTTOM_OAM_10);
                soundPause(MUSIC_0);
                NF_CreateTiledBg(1, 3, "pause_0");
            }
            last_vblank = vblank;
        }

        return;
    }

    if ((keysHeld() & KEY_UP) && !text_bubble_showing)
    {

        last_direction = KEY_UP;

        if (!collision[tile_y_pos - 1][tile_x_pos])
        {
            up = 1;
        }
        else
        {
            current_vblank = vblank;
            if (current_vblank - last_vblank > diff_vblank)
            {
                NF_PlayRawSound(MUSIC_1, 127, 64, false, 0);

                last_vblank = vblank;
            }
            NF_DeleteSprite(0, last_frame);
            NF_CreateSprite(0, UP_0, UP_0, CHARACTER_SPRITE_PAL, 112, 80);
            last_frame = UP_0;
        }
        return;
    }

    if ((keysHeld() & KEY_DOWN) && !text_bubble_showing)
    {

        last_direction = KEY_DOWN;

        if (!collision[tile_y_pos + 1][tile_x_pos])
        {
            down = 1;
        }
        else
        {
            current_vblank = vblank;
            if (current_vblank - last_vblank > diff_vblank)
            {
                NF_PlayRawSound(MUSIC_1, 127, 64, false, 0);

                last_vblank = vblank;
            }
            NF_DeleteSprite(0, last_frame);
            NF_CreateSprite(0, DOWN_0, DOWN_0, CHARACTER_SPRITE_PAL, 112, 80);
            last_frame = DOWN_0;
        }
        return;
    }

    if ((keysHeld() & KEY_LEFT) && !text_bubble_showing)
    {

        last_direction = KEY_LEFT;

        if (!collision[tile_y_pos][tile_x_pos - 1])
        {
            left = 1;
        }
        else
        {
            current_vblank = vblank;
            if (current_vblank - last_vblank > diff_vblank)
            {
                NF_PlayRawSound(MUSIC_1, 127, 64, false, 0);

                last_vblank = vblank;
            }
            NF_DeleteSprite(0, last_frame);
            NF_CreateSprite(0, LEFT_0, LEFT_0, CHARACTER_SPRITE_PAL, 112, 80);
            last_frame = LEFT_0;
        }
        return;
    }

    if ((keysHeld() & KEY_RIGHT) && !text_bubble_showing)
    {

        last_direction = KEY_RIGHT;

        if (!collision[tile_y_pos][tile_x_pos + 1])
        {
            right = 1;
        }
        else
        {
            current_vblank = vblank;
            if (current_vblank - last_vblank > diff_vblank)
            {
                NF_PlayRawSound(MUSIC_1, 127, 64, false, 0);

                last_vblank = vblank;
            }
            NF_DeleteSprite(0, last_frame);
            NF_CreateSprite(0, RIGHT_0, RIGHT_0, CHARACTER_SPRITE_PAL, 112, 80);
            last_frame = RIGHT_0;
        }
        return;
    }

    if (keysHeld() & KEY_SELECT)
    {
        current_vblank = vblank;

        if (current_vblank - last_vblank > teleport_limit)
        {

            if (!debugging)
            {
                NF_ShowBg(1, 0);
                debugging = 1;
            }
            else
            {
                NF_HideBg(1, 0);
                debugging = 0;
            }

            last_vblank = vblank;
        }

        return;
    }

    if ((keysHeld() & KEY_A) && ((tile_x_pos == 10 && tile_y_pos == 13) || (tile_x_pos == 9 && tile_y_pos == 14)))
    {
        current_vblank = vblank;

        if (current_vblank - last_vblank > teleport_limit)
        {
            tile_x_pos = 9;
            tile_y_pos = 10;
            x_pos = ((tile_x_pos - 4) * tile_size) + tile_walk_offset;
            y_pos = ((tile_y_pos - 3) * tile_size) + tile_walk_offset;
            last_vblank = vblank;
        }
        return;
    }

    if ((keysHeld() & KEY_A) && ((tile_x_pos == 8 && tile_y_pos == 11) || (tile_x_pos == 9 && tile_y_pos == 10) || (tile_x_pos == 10 && tile_y_pos == 11)))
    {
        current_vblank = vblank;

        if (current_vblank - last_vblank > teleport_limit)
        {
            tile_x_pos = 9;
            tile_y_pos = 14;
            x_pos = ((tile_x_pos - 4) * tile_size) + tile_walk_offset;
            y_pos = ((tile_y_pos - 3) * tile_size) + tile_walk_offset;
            last_vblank = vblank;
        }
        return;
    }

    if ((keysHeld() & KEY_L) || (touch_pos_x >= 1 && touch_pos_x <= 36 && touch_pos_y >= 1 && touch_pos_y <= 43))
    {
        change_character(0);

        return;
    }

    if ((keysHeld() & KEY_R) || (touch_pos_x >= 220 && touch_pos_x <= 255 && touch_pos_y >= 0 && touch_pos_y <= 43))
    {
        change_character(1);

        return;
    }

    // 6, 19
    // 8, 14

    if ((keysHeld() & KEY_A) && !text_bubble_showing && last_direction == KEY_UP && ((tile_x_pos == 6 && tile_y_pos == 19)))
    {

        current_vblank = vblank;
        if (current_vblank - last_vblank > 20)
        {
            NF_ShowBg(0, 1);
            ui_print("Alex can use Aurora to          freeze the water");
            current_sentence++;
            text_bubble_showing = 1;
            last_vblank = vblank;
        }

        return;
    }

    if ((keysHeld() & KEY_A) && text_bubble_showing && (tile_x_pos == 6 && tile_y_pos == 19))
    {

        current_vblank = vblank;
        if (current_vblank - last_vblank > 20)
        {

            switch (current_sentence)
            {
            case 2:
                ui_print("Be aware, Aurora needs          space in front of Alex          to appear.");
                current_sentence++;
                break;
            case 3:
                ui_print("Luckily, you can use the        D-Pad to face in the            direction you would like.");
                current_sentence++;
                break;
            case 4:
                NF_ClearTextLayer(0, 0);
                NF_UpdateTextLayers();
                NF_HideBg(0, 1);
                current_sentence = 1;
                text_bubble_showing = 0;
                break;
            }

            last_vblank = vblank;
        }
    }

    if ((keysHeld() & KEY_A) && !text_bubble_showing && last_direction == KEY_UP && ((tile_x_pos == 8 && tile_y_pos == 14)))
    {

        current_vblank = vblank;
        if (current_vblank - last_vblank > 20)
        {
            NF_ShowBg(0, 1);
            ui_print("Alex can also use Aurora        to climb into vents.");
            current_sentence++;
            text_bubble_showing = 1;
            last_vblank = vblank;
        }

        return;
    }

    if ((keysHeld() & KEY_A) && text_bubble_showing && (tile_x_pos == 8 && tile_y_pos == 14))
    {

        current_vblank = vblank;
        if (current_vblank - last_vblank > 20)
        {

            if (current_sentence == 2)
            {
                NF_ClearTextLayer(0, 0);
                NF_UpdateTextLayers();
                NF_HideBg(0, 1);
                current_sentence = 1;
                text_bubble_showing = 0;

                last_vblank = vblank;
            }
        }
        return;
    }
}

void set_keys_on_menu(int touch_pos_x, int touch_pos_y)
{

    if (keysHeld() & KEY_UP)
    {

        current_vblank = vblank;
        if (current_vblank - last_vblank > 20)
        {
            if (menu_select == 0)
            {
                menu_select = 2;
            }
            else
            {
                menu_select--;
            }

            char buff[25] = {0};
            sprintf(buff, "title_bottom_%d", menu_select);
            NF_CreateTiledBg(1, 3, buff);

            last_vblank = vblank;
        }
        return;
    }

    if (keysHeld() & KEY_DOWN)
    {

        current_vblank = vblank;
        if (current_vblank - last_vblank > 20)
        {
            if (menu_select == 2)
            {
                menu_select = 0;
            }
            else
            {
                menu_select++;
            }

            char buff[25] = {0};
            sprintf(buff, "title_bottom_%d", menu_select);
            NF_CreateTiledBg(1, 3, buff);

            last_vblank = vblank;
        }
        return;
    }

    if ((keysHeld() & KEY_A) && menu_select == 0)
    {
        current_vblank = vblank;

        if (current_vblank - last_vblank > 20)
        {
            start();
            last_vblank = vblank;
        }
        return;
    }

    if (keysHeld() & KEY_SELECT)
    {
        current_vblank = vblank;

        if (current_vblank - last_vblank > teleport_limit)
        {

            if (!debugging)
            {
                NF_ShowBg(1, 0);
                debugging = 1;
            }
            else
            {
                NF_HideBg(1, 0);
                debugging = 0;
            }

            last_vblank = vblank;
        }
        return;
    }

    // menu buttons:
    // singleplayer: 49,12 -> 254,48
    // multiplayer:  49,78 -> 254,114
    // options:     49,143 -> 254,179

    // if touching singleplayer
    if (touch_pos_x >= 49 && touch_pos_y >= 12 && touch_pos_x <= 254 && touch_pos_y <= 48)
    {
        current_vblank = vblank;
        if (current_vblank - last_vblank > 20)
        {
            start();
            last_vblank = vblank;
        }
        return;
    }
    if (touch_pos_x >= 49 && touch_pos_y >= 78 && touch_pos_x <= 254 && touch_pos_y <= 114)
    {

        current_vblank = vblank;
        if (current_vblank - last_vblank > 20)
        {
            menu_select = 1;
            NF_CreateTiledBg(1, 3, "title_bottom_1");
            last_vblank = vblank;
        }
        return;
    }
    if (touch_pos_x >= 49 && touch_pos_y >= 143 && touch_pos_x <= 254 && touch_pos_y <= 179)
    {
        current_vblank = vblank;
        if (current_vblank - last_vblank > 20)
        {
            menu_select = 2;
            NF_CreateTiledBg(1, 3, "title_bottom_2");
            last_vblank = vblank;
        }
        return;
    }
}

void set_keys_on_pause(int touch_pos_x, int touch_pos_y)
{

    if (keysHeld() & KEY_START)
    {
        current_vblank = vblank;
        if (current_vblank - last_vblank > 20)
        {
            paused = 0;
            NF_CreateSprite(1, BOTTOM_OAM_0, BOTTOM_OAM_0, BOTTOM_SCREEN_SPRITE_PAL, x_red_pos, y_red_pos);
            NF_CreateSprite(1, BOTTOM_OAM_9, BOTTOM_OAM_9, BOTTOM_SCREEN_SPRITE_PAL, 0, 0);
            NF_CreateSprite(1, BOTTOM_OAM_10, BOTTOM_OAM_10, BOTTOM_SCREEN_SPRITE_PAL, 220, 0);
            NF_CreateSprite(1, last_l_sprite, last_l_sprite, BOTTOM_SCREEN_SPRITE_PAL, 2, 10);
            NF_CreateSprite(1, last_r_sprite, last_r_sprite, BOTTOM_SCREEN_SPRITE_PAL, 222, 10);
            soundResume(MUSIC_0);
            NF_CreateTiledBg(1, 3, "test1mini");
            last_vblank = vblank;
        }
        return;
    }

    if (keysHeld() & KEY_SELECT)
    {
        current_vblank = vblank;

        if (current_vblank - last_vblank > teleport_limit)
        {

            if (!debugging)
            {
                NF_ShowBg(1, 0);
                debugging = 1;
            }
            else
            {
                NF_HideBg(1, 0);
                debugging = 0;
            }

            last_vblank = vblank;
        }
        return;
    }

    if ((keysHeld() & KEY_DOWN) || (keysHeld() & KEY_UP))
    {
        current_vblank = vblank;
        if (current_vblank - last_vblank > 20)
        {
            if (pause_selection == 0)
            {
                NF_CreateTiledBg(1, 3, "pause_1");
                pause_selection = 1;
            }
            else
            {
                NF_CreateTiledBg(1, 3, "pause_0");
                pause_selection = 0;
            }
            last_vblank = vblank;
        }
        return;
    }

    if (keysHeld() & KEY_A)
    {

        if (pause_selection == 0)
        {
            current_vblank = vblank;
            if (current_vblank - last_vblank > 20)
            {
                paused = 0;
                NF_CreateSprite(1, BOTTOM_OAM_0, BOTTOM_OAM_0, BOTTOM_SCREEN_SPRITE_PAL, x_red_pos, y_red_pos);
                NF_CreateSprite(1, BOTTOM_OAM_9, BOTTOM_OAM_9, BOTTOM_SCREEN_SPRITE_PAL, 0, 0);
                NF_CreateSprite(1, BOTTOM_OAM_10, BOTTOM_OAM_10, BOTTOM_SCREEN_SPRITE_PAL, 220, 0);
                NF_CreateSprite(1, last_l_sprite, last_l_sprite, BOTTOM_SCREEN_SPRITE_PAL, 2, 10);
                NF_CreateSprite(1, last_r_sprite, last_r_sprite, BOTTOM_SCREEN_SPRITE_PAL, 222, 10);
                soundResume(MUSIC_0);
                NF_CreateTiledBg(1, 3, "test1mini");
                last_vblank = vblank;
            }
        }
        else
        {
            current_vblank = vblank;
            if (current_vblank - last_vblank > 20)
            {
                stop();
                last_vblank = vblank;
            }
        }
        return;
    }

    // back to game touched
    if (touch_pos_x >= 68 && touch_pos_y >= 17 && touch_pos_x <= 254 && touch_pos_y <= 58)
    {
        current_vblank = vblank;
        if (current_vblank - last_vblank > 20)
        {
            paused = 0;
            NF_CreateSprite(1, BOTTOM_OAM_0, BOTTOM_OAM_0, BOTTOM_SCREEN_SPRITE_PAL, x_red_pos, y_red_pos);
            NF_CreateSprite(1, BOTTOM_OAM_9, BOTTOM_OAM_9, BOTTOM_SCREEN_SPRITE_PAL, 0, 0);
            NF_CreateSprite(1, BOTTOM_OAM_10, BOTTOM_OAM_10, BOTTOM_SCREEN_SPRITE_PAL, 220, 0);
            NF_CreateSprite(1, last_l_sprite, last_l_sprite, BOTTOM_SCREEN_SPRITE_PAL, 2, 10);
            NF_CreateSprite(1, last_r_sprite, last_r_sprite, BOTTOM_SCREEN_SPRITE_PAL, 222, 10);
            soundResume(MUSIC_0);
            NF_CreateTiledBg(1, 3, "test1mini");
            last_vblank = vblank;
        }
        return;
    }

    // quit touched
    if (touch_pos_x >= 68 && touch_pos_y >= 71 && touch_pos_x <= 254 && touch_pos_y <= 113)
    {
        current_vblank = vblank;
        if (current_vblank - last_vblank > 20)
        {
            stop();
            last_vblank = vblank;
        }
        return;
    }
}