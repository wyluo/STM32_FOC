#ifndef APP_DINOGAME_H
#define APP_DINOGAME_H

#include "app_oled.h"

void oled1306_draw_cloud(void);
void oled1306_draw_ground(void);
int oled1306_draw_dino_jump(char reset);
void oled1306_draw_dino(void);
void oled1306_draw_cactus_obstacle(void);
int oled1306_draw_cactus_random(unsigned char ver, unsigned char reset);
void oled1306_draw_restart(void);
void Game_control(void);

#endif
