#include <nds.h>
#include <nds/arm9/input.h>
#include <nds/arm9/sound.h>
#include <nds/arm9/sprite.h>
#include <nf_lib.h>
#include <stdio.h>
#include "main.h"
#include "character.h"
#include "mem_slots.h"
#include "key_input.h"
#include"animations.h"

// position of red X marker
int x_red_pos = x_pos;
int y_red_pos = y_pos;

// number of vblanks (frames) that have happened so far, plus some variables for counting elapsed time
int vblank = 0;
int current_vblank = 0x7FFFFFFF;
int last_vblank = 0;

// how much vblanks should elapse between "bonks"
int diff_vblank = 40;

// map is 1024x1024
int map_size = 1024;

touchPosition touch_pos;

// if game is paused. game is NOT paused on main menu. only pause menu
int paused = 0;

// if debugging is enabled
int debugging = 0;

// if game is running
// title = 0
// game = 1
int running = 0;

/*
	Take the current tile written to the background tilesheet
	and replicate it across the entire background

	DEPRECATED
// */
// void draw_level(){

// 	for( int offset = 0; offset < 4; offset++){
// 		for(int y = 0; y < 8; y++){
// 			for(int x = 0; x < 8; x++){
// 				NF_DmaMemCopy(  (void*) ( ((0x06000000 + offset*64) + y*256) + x*8), (void*) (0x06000000 + offset*64), 8);
// 			}
// 		}
// 	}

// }

void count_vBlank()
{
	vblank++;

	if (vblank == 0x7FFFFFFF)
		vblank = 0;
}

void stop()
{
	paused = 0;
	running = 0;
	anim_step = 0x7FFFFFFF;
	last_frame = 0x7FFFFFFF;

	// NF_CreateTextLayer(1, 0, 0, "font_white");
	// NF_CreateTextLayer(0, 0, 0, "font_black");

	// if a dialogue was open, close it
	if (text_bubble_showing)
	{
		NF_HideBg(0, 1);
		NF_ClearTextLayer(0, 0);
		NF_UpdateTextLayers();
		current_sentence = 1;
		text_bubble_showing = 0;
	}

	NF_DeleteTextLayer(0, 0);
	NF_CreateTiledBg(0, 3, "title");
	NF_CreateTiledBg(1, 3, "title_bottom_0");
	NF_ResetSpriteBuffers();
	NF_InitSpriteBuffers();
	NF_InitSpriteSys(0);
	NF_InitSpriteSys(1);
	NF_ResetRawSoundBuffers();
}

void start()
{

	running = 1;

	// reset map positions
	tile_x_pos = 21;
	tile_y_pos = 20;

	// equations convert tile map positions to x,y coordinates to paint TOP LEFT CORNER of image to
	x_pos = ((tile_x_pos - 4) * tile_size) + tile_walk_offset;
	y_pos = ((tile_y_pos - 3) * tile_size) + tile_walk_offset;

	// reset currently selected character
	character_selected = ALEX;
	last_l_sprite = BOTTOM_OAM_8;
	last_r_sprite = BOTTOM_OAM_2;

	// reload data
	NF_LoadSpritePal("characters", CHARACTER_SPRITE_PAL);
	NF_LoadSpritePal("bottom_screen", BOTTOM_SCREEN_SPRITE_PAL);
	NF_LoadSpritePal("map_items", MAP_ITEMS);
	NF_LoadSpriteGfx("red_x", BOTTOM_OAM_0, 8, 8);
	NF_LoadSpriteGfx("alex_dialogue", BOTTOM_OAM_1, 32, 32);
	NF_LoadSpriteGfx("mike_dialogue", BOTTOM_OAM_2, 32, 32);
	NF_LoadSpriteGfx("mittens_dialogue", BOTTOM_OAM_3, 32, 32);
	NF_LoadSpriteGfx("vex_dialogue", BOTTOM_OAM_4, 32, 32);
	NF_LoadSpriteGfx("shibe_dialogue", BOTTOM_OAM_5, 32, 32);
	NF_LoadSpriteGfx("athena_dialogue", BOTTOM_OAM_6, 32, 32);
	NF_LoadSpriteGfx("faelyn_dialogue", BOTTOM_OAM_7, 32, 32);
	NF_LoadSpriteGfx("glen_4_dialogue", BOTTOM_OAM_8, 32, 32);
	NF_LoadSpriteGfx("l_button", BOTTOM_OAM_9, 64, 64);
	NF_LoadSpriteGfx("r_button", BOTTOM_OAM_10, 64, 64);
	NF_LoadRawSound("portal", MUSIC_0, 11025, 0);

	// show all graphics on screen and play sound
	NF_CreateTiledBg(0, 0, "buffer");
	NF_CreateTiledBg(0, 3, "test1");
	NF_CreateTiledBg(1, 3, "test1mini");
	NF_CreateTiledBg(0, 1, "text_bubble");
	//NF_CreateTiledBg(0, 2, "water_0");
	NF_HideBg(0, 1);
	NF_HideBg(0, 3);
	NF_CreateTextLayer(0, 0, 0, "font_black");
	//NF_CreateTextLayer(1, 0, 0, "font_white");
	NF_ClearTextLayer(1, 0);
	NF_VramSpritePal(0, CHARACTER_SPRITE_PAL, CHARACTER_SPRITE_PAL);
	NF_VramSpritePal(1, BOTTOM_SCREEN_SPRITE_PAL, BOTTOM_SCREEN_SPRITE_PAL);
	NF_VramSpritePal(0, MAP_ITEMS, MAP_ITEMS);
	NF_VramSpriteGfx(1, BOTTOM_OAM_0, BOTTOM_OAM_0, true);
	NF_VramSpriteGfx(1, BOTTOM_OAM_1, BOTTOM_OAM_1, true);
	NF_VramSpriteGfx(1, BOTTOM_OAM_2, BOTTOM_OAM_2, true);
	NF_VramSpriteGfx(1, BOTTOM_OAM_3, BOTTOM_OAM_3, true);
	NF_VramSpriteGfx(1, BOTTOM_OAM_4, BOTTOM_OAM_4, true);
	NF_VramSpriteGfx(1, BOTTOM_OAM_5, BOTTOM_OAM_5, true);
	NF_VramSpriteGfx(1, BOTTOM_OAM_6, BOTTOM_OAM_6, true);
	NF_VramSpriteGfx(1, BOTTOM_OAM_7, BOTTOM_OAM_7, true);
	NF_VramSpriteGfx(1, BOTTOM_OAM_8, BOTTOM_OAM_8, true);
	NF_VramSpriteGfx(1, BOTTOM_OAM_9, BOTTOM_OAM_9, true);
	NF_VramSpriteGfx(1, BOTTOM_OAM_10, BOTTOM_OAM_10, true);
	NF_PlayRawSound(MUSIC_0, 95, 64, true, 0);

	// draw character sprite
	init_player("alex");
	NF_CreateSprite(0, DOWN_0, DOWN_0, CHARACTER_SPRITE_PAL, 112, 80);
	last_frame = DOWN_0;

	//draw animation sprites

	animate_water();

	//draw red x
	NF_CreateSprite(1, BOTTOM_OAM_0, BOTTOM_OAM_0, BOTTOM_SCREEN_SPRITE_PAL, 45, 8);

	// draw L and R sprites

	NF_CreateSprite(1, BOTTOM_OAM_9, BOTTOM_OAM_9, BOTTOM_SCREEN_SPRITE_PAL, 0, 0);
	NF_CreateSprite(1, BOTTOM_OAM_10, BOTTOM_OAM_10, BOTTOM_SCREEN_SPRITE_PAL, 220, 0);
	NF_CreateSprite(1, last_l_sprite, last_l_sprite, BOTTOM_SCREEN_SPRITE_PAL, 2, 10);
	NF_CreateSprite(1, last_r_sprite, last_r_sprite, BOTTOM_SCREEN_SPRITE_PAL, 222, 10);

	// Prep for printing text
	// consoleDemoInit();

	// Show the actual background
	NF_ShowBg(0, 3);
	NF_ShowBg(1, 3);
	NF_HideBg(0, 1);
	NF_ScrollBg(0, 3, x_pos, y_pos);
}

// Draw debug text
void debug(int mode)
{

	// mode 0 = in-game text
	// mode 1 = just show touch x/y

	char buff[300] = {0};

	switch (mode)
	{
	case 0:

		if (debugging)
		{

			sprintf(buff, "OC for the DS                   v0.0.8                          "
						  "X = %2d Y = %2d                   vBlanks: %105d"
						  "              Touch X = %3d                         Y = %3d",
					tile_x_pos, tile_y_pos, vblank, touch_pos.px, touch_pos.py);

			NF_ClearTextLayer(1, 0);
			NF_WriteText(1, 0, 14, 0, buff);
			NF_UpdateTextLayers();
		}
		else
		{
			NF_HideBg(1, 0);
		}

		break;
	case 1:

		if (debugging)
		{
			sprintf(buff, "Touch X = %3d Y = %3d",
					touch_pos.px, touch_pos.py);

			NF_ClearTextLayer(1, 0);
			NF_WriteText(1, 0, 0, 0, buff);
			NF_UpdateTextLayers();
		}
		else
		{
			NF_HideBg(1, 0);
		}

		break;
	}
}

int main(void)
{

	// set up file system and screens
	NF_SetRootFolder("NITROFS");

	NF_Set2D(0, 0);
	NF_Set2D(1, 0);

	// set up graphics and sound systems
	NF_InitTiledBgBuffers();
	NF_InitTiledBgSys(0);
	NF_InitTiledBgSys(1);
	NF_InitTextSys(0);
	NF_InitTextSys(1);
	NF_InitSpriteBuffers();
	NF_InitSpriteSys(0);
	NF_InitSpriteSys(1);

	soundEnable();
	NF_InitRawSoundBuffers();

	// load all graphics and sounds into memory
	NF_LoadTiledBg("buffer", "buffer", 256, 256);
	NF_LoadTiledBg("test1", "test1", 1024, 1024);
	NF_LoadTiledBg("test1mini", "test1mini", 1024, 1024);
	NF_LoadTiledBg("text_bubble", "text_bubble", 256, 256);
	NF_LoadTiledBg("title", "title", 256, 256);
	NF_LoadTiledBg("title_bottom_0", "title_bottom_0", 256, 256);
	NF_LoadTiledBg("title_bottom_1", "title_bottom_1", 256, 256);
	NF_LoadTiledBg("title_bottom_2", "title_bottom_2", 256, 256);
	//NF_LoadTiledBg("water_0", "water_0", 1024, 1024);
	//NF_LoadTiledBg("water_1", "water_1", 1024, 1024);
	//NF_LoadTiledBg("water_2", "water_2", 1024, 1024);
	NF_LoadTiledBg("pause_0", "pause_0", 256, 256);
	NF_LoadTiledBg("pause_1", "pause_1", 256, 256);

	NF_LoadTextFont("font_white", "font_white", 256, 256, 0);
	NF_LoadTextFont("font_black", "font_black", 256, 256, 0);

	NF_CreateTiledBg(0, 3, "title");
	NF_CreateTiledBg(1, 3, "title_bottom_0");
	NF_CreateTextLayer(1, 0, 0, "font_white");

	while (1)
	{
		// Refresh keys and check touch screen
		scanKeys();
		touchRead(&touch_pos);

		if (running)
		{

			if (!paused)
			{
				// if not currently moving, check if any buttons are pressed
				if (!(up || down || left || right))
				{
					set_keys_in_game(touch_pos.px, touch_pos.py);
				}

				if (paused)
				{
					continue;
				}

				// then move the character if need be
				if (up || down || left || right)
					move_character();

				// and show those changes on the screen by scrolling
				NF_ScrollBg(0, 3, x_pos, y_pos);
				NF_ScrollBg(0, 2, x_pos, y_pos);

				//be sure to move the animated sprites as well
				//NF_DeleteSprite(0, WATER_0);
				animate_water();

				// update X on bottom screen as well
				x_red_pos = (x_pos / 4) + 25;
				y_red_pos = (y_pos / 4) - 20;

				NF_DeleteSprite(1, BOTTOM_OAM_0);
				NF_CreateSprite(1, BOTTOM_OAM_0, BOTTOM_OAM_0, BOTTOM_SCREEN_SPRITE_PAL, x_red_pos, y_red_pos);


				// update characters on L and R toggles

				// Print text on bottom screen
				//  iprintf("\033[2J\033[H");
				//  iprintf("\x1b[32mOC for the DS - v0.0.7\n\n\n");
				//  iprintf("\x1b[37mD-Pad UP - Move Up\n");
				//  iprintf("D-Pad DOWN - Move Down\n");
				//  iprintf("D-Pad LEFT - Move Left\n");
				//  iprintf("D-Pad RIGHT - Move Right\n\n\n");
				//  iprintf("A - Interact\n");
				//  iprintf("L/R - Switch Characters\n\n\n");
				//  iprintf("Start - Exit Game\n\n\n");
				//  iprintf("\x1b[35mX:%d, Y:%d\nvBlanks: %d\n\n", tile_x_pos, tile_y_pos, vblank);
				//  iprintf("\x1b[36m---------------------\n");
				debug(0);
			}
			else
			{
				set_keys_on_pause(touch_pos.px, touch_pos.py);
				debug(1);
			}
		}
		else
		{
			set_keys_on_menu(touch_pos.px, touch_pos.py);
			debug(1);
		}

		// update the sprite in the OAM and wait for vblank
		NF_SpriteOamSet(1);
		NF_SpriteOamSet(0);
		swiWaitForVBlank();
		count_vBlank();
		oamUpdate(&oamSub);
		oamUpdate(&oamMain);
	}

	return 0;
}