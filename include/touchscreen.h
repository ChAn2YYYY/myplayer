#ifndef _TOUCHSCREEN_H
#define _TOUCHSCREEN_H

#include <stdbool.h>

#define TOUCHSCREEN_GET_COORDINATE      0
#define TOUCHSCREEN_TOUCH_STATUS        1
#define TOUCHSCREEN_RELEASE_STATUS      2
#define TOUCHSCREEN_ERROR_STATUS        -1


extern int open_touchscreen_device(void);

extern int close_touchscreen_device(int ts_fd);

extern int get_touchscreen_coordinate(int ts_fd, int *get_x, int *get_y);

extern int get_command(struct resource *src,bool flag,char *cmd);

extern int judge_touch_area(int get_x, int get_y, int st_x, int end_x, int st_y, int end_y);

extern int show_percent(struct resource *src,char * buf); //显示百分比

extern int progress_bar(struct resource *src,int rate,int start_x, int start_y, int width, int height);

extern int select_menu(struct resource *src, int init_x, int init_y);

extern int gestrue_handler(struct resource *src, char *buffer, int flag, int init_x, int init_y);

#endif/*end of _TOUCHSCREEN_H*/