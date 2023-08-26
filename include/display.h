#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <font.h>
#include <video.h>
#define FRAMEBUFFER_WIDE 800
#define FRAMEBUFFER_HEIGHT 480

#define GREEN 0x0000FF00
#define BLACK 0X00000000
#define WHITE 0X00FFFFFF

/*
    函数功能：
        打开显存文件及映射显存地址
    参数：
        framebuffer_addr：用来存放显存的地址
    返回值：
        返回一个显存的文件描述符，失败返回-1
*/
extern int open_framebuffer_device(unsigned int **framebuffer_addr);

extern int close_framebuffer_device(int framebuffer_fd, unsigned int *framebuffer_addr);

extern int display_jpeg_format_to_framebuffer(const char *filename, int x_pos, int y_pos, unsigned int *framebuffer_addr, float mul);
extern int display_backgroud(unsigned int *frambuffer_addr);

extern int set_bottom_color(int pos_x, int pos_y, unsigned int *frambuffer_addr);

extern void display_touch_bar(struct resource *src, int flag);

extern int display_jpeg_format_to_buffer(const char *filename, int x_pos, int y_pos, int buffer_height,unsigned int *framebuffer_addr, float mul);

extern void init_video_menu(struct resource *src);

extern void display_menu(struct resource *src, int st_x, int st_y);



/*
    函数功能：
        给指定坐标显示上一个颜色
    参数：
        x,y：代表屏幕的坐标
        color：要显示的颜色
        framebuffer_addr：显存的地址
    返回值：
        无
*/
static inline void display_framebuffer_point(int x, int y, unsigned int color, unsigned int *framebuffer_addr)
{
    *(framebuffer_addr + FRAMEBUFFER_WIDE * y + x) = color; //给指定的x，y坐标的显存赋值上颜色
}

static inline void display_buffer_point(int x, int y, unsigned int color, unsigned int *framebuffer_addr)
{
    *(framebuffer_addr + 200 * y + x) = color; //给指定的x，y坐标的内存赋值上颜色
}

#endif /*end of _DISPLAY_H*/