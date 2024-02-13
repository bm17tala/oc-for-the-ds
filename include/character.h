#ifndef CHARACTERLOADS_H
#define CHARACTERLOADS_H

#define ALEX 0
#define MIKE 1
#define MITTENS 2
#define VEX 3
#define SHIBE 4
#define ATHENA 5
#define FAELYN 6
#define GLEN_4 7

#define NUMBER_OF_PLAYERS 8

extern const int tile_size;
extern int tile_x_pos;
extern int tile_y_pos;
extern int tile_walk_offset;
extern int x_pos;
extern int y_pos;
extern int up, down, left, right;
extern int anim_step;
extern int last_frame;
extern int last_l_sprite;
extern int last_r_sprite;

extern void load_player(const char* player);
extern void init_player(const char* player);
extern void move_character();
extern void change_character(int);


#endif