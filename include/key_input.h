#ifndef KEY_INPUT_H
#define KEY_INPUT_H

extern u8 collision[32][32];


extern int character_selected;
extern const char* players[];
extern int text_bubble_showing;
extern int current_sentence;
extern int last_direction;
extern int pause_selection;
extern int menu_select;
extern int teleport_limit;


extern void set_keys_in_game(int, int);
extern void set_keys_on_menu(int, int);
extern void set_keys_on_pause(int, int);

#endif