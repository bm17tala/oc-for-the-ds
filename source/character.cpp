#include <nds.h>
#include <nds/arm9/input.h>
#include <nds/arm9/sound.h>
#include <nds/arm9/sprite.h>
#include <nf_lib.h>
#include <stdio.h>
#include "character.h"
#include "mem_slots.h"
#include "key_input.h"
#include "main.h"

// pixel size of each tile
const int tile_size = 32;

// position that map is scrolled to
int tile_x_pos = 21;
int tile_y_pos = 20;

// so we are centered inside of a tile rather than on its top left corner
int tile_walk_offset = 16;

// equations convert tile map positions to x,y coordinates to paint TOP LEFT CORNER of image to
int x_pos = ((tile_x_pos - 4) * tile_size) + tile_walk_offset;
int y_pos = ((tile_y_pos - 3) * tile_size) + tile_walk_offset;

// direction toggles. toggle on/off when character is or isn't moving, used as booleans
int up = 0, down = 0, left = 0, right = 0;

// current frame of animation, and pointer of last frame used for clearing screen before next paint
//if these are at max value, the game is not running
int anim_step = 0x7FFFFFFF;
int last_frame = 0x7FFFFFFF;

// last L and R sprites from bottom screen used so they can be deleted properly from the screen
int last_l_sprite = BOTTOM_OAM_8;
int last_r_sprite = BOTTOM_OAM_2;

void load_player(const char *player)
{

	soundPause(MUSIC_1);

	NF_UnloadRawSound(MUSIC_1);
	NF_UnloadSpriteGfx(DOWN_0);
	NF_UnloadSpriteGfx(DOWN_1);
	NF_UnloadSpriteGfx(DOWN_2);
	NF_UnloadSpriteGfx(UP_0);
	NF_UnloadSpriteGfx(UP_1);
	NF_UnloadSpriteGfx(UP_2);
	NF_UnloadSpriteGfx(LEFT_0);
	NF_UnloadSpriteGfx(LEFT_1);
	NF_UnloadSpriteGfx(LEFT_2);
	NF_UnloadSpriteGfx(RIGHT_0);
	NF_UnloadSpriteGfx(RIGHT_1);
	NF_UnloadSpriteGfx(RIGHT_2);

	NF_FreeSpriteGfx(0, DOWN_0);
	NF_FreeSpriteGfx(0, DOWN_1);
	NF_FreeSpriteGfx(0, DOWN_2);
	NF_FreeSpriteGfx(0, UP_0);
	NF_FreeSpriteGfx(0, UP_1);
	NF_FreeSpriteGfx(0, UP_2);
	NF_FreeSpriteGfx(0, LEFT_0);
	NF_FreeSpriteGfx(0, LEFT_1);
	NF_FreeSpriteGfx(0, LEFT_2);
	NF_FreeSpriteGfx(0, RIGHT_0);
	NF_FreeSpriteGfx(0, RIGHT_1);
	NF_FreeSpriteGfx(0, RIGHT_2);

	init_player(player);
}

void init_player(const char *player)
{

	char frame[50] = {0};

	
	// Re-create sprite to stop any problems with character changing direction when last_frame is not
	//its max value (max value indicates not in game)
	if (last_frame != 0x7FFFFFFF)
	{
		NF_DeleteSprite(0, last_frame);
	}


	sprintf(frame, "bonk_%s", player);
	NF_LoadRawSound(frame, MUSIC_1, 11025, 0);

	sprintf(frame, "%s_down_0", player);
	NF_LoadSpriteGfx(frame, DOWN_0, 32, 32);
	sprintf(frame, "%s_down_1", player);
	NF_LoadSpriteGfx(frame, DOWN_1, 32, 32);
	sprintf(frame, "%s_down_2", player);
	NF_LoadSpriteGfx(frame, DOWN_2, 32, 32);
	sprintf(frame, "%s_up_0", player);
	NF_LoadSpriteGfx(frame, UP_0, 32, 32);
	sprintf(frame, "%s_up_1", player);
	NF_LoadSpriteGfx(frame, UP_1, 32, 32);
	sprintf(frame, "%s_up_2", player);
	NF_LoadSpriteGfx(frame, UP_2, 32, 32);
	sprintf(frame, "%s_left_0", player);
	NF_LoadSpriteGfx(frame, LEFT_0, 32, 32);
	sprintf(frame, "%s_left_1", player);
	NF_LoadSpriteGfx(frame, LEFT_1, 32, 32);
	sprintf(frame, "%s_left_2", player);
	NF_LoadSpriteGfx(frame, LEFT_2, 32, 32);
	sprintf(frame, "%s_right_0", player);
	NF_LoadSpriteGfx(frame, RIGHT_0, 32, 32);
	sprintf(frame, "%s_right_1", player);
	NF_LoadSpriteGfx(frame, RIGHT_1, 32, 32);
	sprintf(frame, "%s_right_2", player);
	NF_LoadSpriteGfx(frame, RIGHT_2, 32, 32);

	NF_VramSpriteGfx(0, DOWN_0, DOWN_0, true);
	NF_VramSpriteGfx(0, DOWN_1, DOWN_1, true);
	NF_VramSpriteGfx(0, DOWN_2, DOWN_2, true);
	NF_VramSpriteGfx(0, UP_0, UP_0, true);
	NF_VramSpriteGfx(0, UP_1, UP_1, true);
	NF_VramSpriteGfx(0, UP_2, UP_2, true);
	NF_VramSpriteGfx(0, LEFT_0, LEFT_0, true);
	NF_VramSpriteGfx(0, LEFT_1, LEFT_1, true);
	NF_VramSpriteGfx(0, LEFT_2, LEFT_2, true);
	NF_VramSpriteGfx(0, RIGHT_0, RIGHT_0, true);
	NF_VramSpriteGfx(0, RIGHT_1, RIGHT_1, true);
	NF_VramSpriteGfx(0, RIGHT_2, RIGHT_2, true);

	// Re-create sprite to stop any problems with character changing direction when last_frame is not its
	// starting value
	if (last_frame != 0x7FFFFFFF)
	{
		NF_CreateSprite(0, last_frame, last_frame, CHARACTER_SPRITE_PAL, 112, 80);
	}
}

void move_character()
{

	if (up)
	{

		if ((((y_pos + tile_walk_offset) % 32) == 0))
			tile_y_pos--;

		y_pos--;

		if ((y_pos % 8) == 0)
		{
			if (anim_step > 3)
				anim_step = 0;

			switch (anim_step)
			{

			// standstill
			case 0:
			case 2:
				NF_DeleteSprite(0, last_frame);
				NF_CreateSprite(0, UP_0, UP_0, CHARACTER_SPRITE_PAL, 112, 80);
				last_frame = UP_0;
				break;

			// moving
			case 1:
				NF_DeleteSprite(0, last_frame);
				NF_CreateSprite(0, UP_1, UP_1, CHARACTER_SPRITE_PAL, 112, 80);
				last_frame = UP_1;
				break;

			case 3:
				NF_DeleteSprite(0, last_frame);
				NF_CreateSprite(0, UP_2, UP_2, CHARACTER_SPRITE_PAL, 112, 80);
				last_frame = UP_2;
				break;
			}

			anim_step++;
		}

		if (((((y_pos + tile_walk_offset) % 32) == 0)) && (!(keysHeld() & KEY_UP) || collision[tile_y_pos - 1][tile_x_pos]))
		{
			up = 0;
			NF_DeleteSprite(0, last_frame);
			NF_CreateSprite(0, UP_0, UP_0, CHARACTER_SPRITE_PAL, 112, 80);
			last_frame = UP_0;
		}

		return;
	}
	if (down)
	{

		if (((y_pos + tile_walk_offset) % 32) == 0)
			tile_y_pos++;

		y_pos++;

		if ((y_pos % 8) == 0)
		{
			if (anim_step > 3)
				anim_step = 0;

			switch (anim_step)
			{

			// standstill
			case 0:
			case 2:
				NF_DeleteSprite(0, last_frame);
				NF_CreateSprite(0, DOWN_0, DOWN_0, CHARACTER_SPRITE_PAL, 112, 80);
				last_frame = DOWN_0;
				break;

			// moving
			case 1:
				NF_DeleteSprite(0, last_frame);
				NF_CreateSprite(0, DOWN_1, DOWN_1, CHARACTER_SPRITE_PAL, 112, 80);
				last_frame = DOWN_1;
				break;

			case 3:
				NF_DeleteSprite(0, last_frame);
				NF_CreateSprite(0, DOWN_2, DOWN_2, CHARACTER_SPRITE_PAL, 112, 80);
				last_frame = DOWN_2;
				break;
			}

			anim_step++;
		}

		if ((((y_pos + tile_walk_offset) % 32) == 0) && (!(keysHeld() & KEY_DOWN) || collision[tile_y_pos + 1][tile_x_pos]))
		{
			down = 0;
			NF_DeleteSprite(0, last_frame);
			NF_CreateSprite(0, DOWN_0, DOWN_0, CHARACTER_SPRITE_PAL, 112, 80);
			last_frame = DOWN_0;
		}
		return;
	}

	if (left)
	{

		if (((x_pos + tile_walk_offset) % 32) == 0)
			tile_x_pos--;

		// if at end of map, roll over to the other end of the map
		//  if(x_pos == 0){
		//  	x_pos = map_size - 1;
		//  	left = 0;
		//  	return;
		//  }
		x_pos--;

		if ((x_pos % 8) == 0)
		{
			if (anim_step > 3)
				anim_step = 0;

			switch (anim_step)
			{

			// standstill
			case 0:
			case 2:
				NF_DeleteSprite(0, last_frame);
				NF_CreateSprite(0, LEFT_0, LEFT_0, CHARACTER_SPRITE_PAL, 112, 80);
				last_frame = LEFT_0;
				break;

			// moving
			case 1:
				NF_DeleteSprite(0, last_frame);
				NF_CreateSprite(0, LEFT_1, LEFT_1, CHARACTER_SPRITE_PAL, 112, 80);
				last_frame = LEFT_1;
				break;

			case 3:
				NF_DeleteSprite(0, last_frame);
				NF_CreateSprite(0, LEFT_2, LEFT_2, CHARACTER_SPRITE_PAL, 112, 80);
				last_frame = LEFT_2;
				break;
			}

			anim_step++;
		}

		if ((((x_pos + tile_walk_offset) % 32) == 0) && (!(keysHeld() & KEY_LEFT) || collision[tile_y_pos][tile_x_pos - 1]))
		{
			left = 0;
			NF_DeleteSprite(0, last_frame);
			NF_CreateSprite(0, LEFT_0, LEFT_0, CHARACTER_SPRITE_PAL, 112, 80);
			last_frame = LEFT_0;
		}
		return;
	}

	if (right)
	{

		if (((x_pos + tile_walk_offset) % 32) == 0)
			tile_x_pos++;

		// go to next pixel
		x_pos++;

		// animate the character every 1/4th of a tile
		if ((x_pos % 8) == 0)
		{
			if (anim_step > 3)
				anim_step = 0;

			switch (anim_step)
			{

			// standstill
			case 0:
			case 2:
				NF_DeleteSprite(0, last_frame);
				NF_CreateSprite(0, RIGHT_0, RIGHT_0, CHARACTER_SPRITE_PAL, 112, 80);
				last_frame = RIGHT_0;
				break;

			// moving
			case 1:
				NF_DeleteSprite(0, last_frame);
				NF_CreateSprite(0, RIGHT_1, RIGHT_1, CHARACTER_SPRITE_PAL, 112, 80);
				last_frame = RIGHT_1;
				break;

			case 3:
				NF_DeleteSprite(0, last_frame);
				NF_CreateSprite(0, RIGHT_2, RIGHT_2, CHARACTER_SPRITE_PAL, 112, 80);
				last_frame = RIGHT_2;
				break;
			}
			anim_step++;
		}

		// procedure when landing on a tile AND the button is no longer pressed OR if the next tile has collision detection
		if ((((x_pos + tile_walk_offset) % 32) == 0) && (!(keysHeld() & KEY_RIGHT) || collision[tile_y_pos][tile_x_pos + 1]))
		{
			right = 0;
			NF_DeleteSprite(0, last_frame);
			NF_CreateSprite(0, RIGHT_0, RIGHT_0, CHARACTER_SPRITE_PAL, 112, 80);
			last_frame = RIGHT_0;
		}
		return;
	}
}

// 0 = left, 1 = right
void change_character(int direction)
{
	current_vblank = vblank;

	// wait 5 blanks if user holds button to switch characters
	if (current_vblank - last_vblank > 10)
	{
		// delete bottom screen dialouge imgs
		NF_DeleteSprite(1, last_l_sprite);
		NF_DeleteSprite(1, last_r_sprite);

		switch (direction)
		{
		case 0:

			// modify character to be displayed on top screen
			if (character_selected == ALEX)
				character_selected = GLEN_4;
			else
				character_selected--;

			// modify values of dialogue imgs to be displayed on bottom screen
			if (last_l_sprite == BOTTOM_OAM_1)
			{
				last_l_sprite = BOTTOM_OAM_8;
			}
			else
			{
				last_l_sprite--;
			}

			if (last_r_sprite == BOTTOM_OAM_1)
			{
				last_r_sprite = BOTTOM_OAM_8;
			}
			else
			{
				last_r_sprite--;
			}

			break;

		case 1:

			if (character_selected == GLEN_4)
				character_selected = ALEX;
			else
				character_selected++;

			// modify values of dialogue imgs to be displayed on bottom screen
			if (last_l_sprite == BOTTOM_OAM_8)
			{
				last_l_sprite = BOTTOM_OAM_1;
			}
			else
			{
				last_l_sprite++;
			}

			if (last_r_sprite == BOTTOM_OAM_8)
			{
				last_r_sprite = BOTTOM_OAM_1;
			}
			else
			{
				last_r_sprite++;
			}

			break;
		}

		

		// load player on top screen
		load_player(players[character_selected]);

		// render left sprite on bottom screen
		NF_CreateSprite(1, last_l_sprite, last_l_sprite, BOTTOM_SCREEN_SPRITE_PAL, 2, 10);

		// render right sprite on bottom screen
		NF_CreateSprite(1, last_r_sprite, last_r_sprite, BOTTOM_SCREEN_SPRITE_PAL, 222, 10);

		last_vblank = vblank;
	}
}