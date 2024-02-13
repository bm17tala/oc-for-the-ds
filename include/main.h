#ifndef MAIN_H
#define MAIN_H

extern int x_red_pos;
extern int y_red_pos;
extern int vblank;
extern int current_vblank;
extern int last_vblank;
extern int diff_vblank;
extern int map_size;
extern touchPosition touch_pos;
extern int paused;
extern int debugging;
extern int running;

extern void start();
extern void stop();
extern void debug(int);

#endif