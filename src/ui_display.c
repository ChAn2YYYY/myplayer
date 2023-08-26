#include <video.h>
#include <play_list.h>
#include <display.h>

#define TOUCH_BAR_PLAY "../picture/play.jpg"
#define TOUCH_BAR_PAUSE "../picture/pause.jpg"

void display_touch_bar(struct resource *src, int flag)
{
    printf("显存地址%p\n", src->framebuffer_addr);
    if (flag > 0)
        display_jpeg_format_to_framebuffer(TOUCH_BAR_PLAY, 0, 400, src->framebuffer_addr, 1); //显示播放进度条
    else if (flag < 0)
        display_jpeg_format_to_framebuffer(TOUCH_BAR_PAUSE, 0, 400, src->framebuffer_addr, 1); //显示暂停进度条
}

//生成一个选择长图
void init_video_menu(struct resource *src)
{
    int x, y;
    video_t *pos;
    char temp[100];
    char temp_1[100];
    char *ptr;
    int i = 0;

    int count = foreach_the_video_list(src);
    src->buffer_height = 480 + ((count - 3) * 150);
    //背景底色
    for (y = 0; y < (480 + ((count - 3) * 150)); y++)
    {
        for (x = 0; x < 200; x++)
        {
            display_buffer_point(x, y, 0x00CDD6F5, src->menu_buffer);
        }
    }

    //贴上图片
    list_for_each_entry(pos, &src->video_list_head->list, list)
    {
        strcpy(temp, pos->name);
        ptr = strchr(temp, '.');
        strcpy(ptr + 1, "jpg");
        sprintf(temp_1,"../picture/%s",temp);
        display_jpeg_format_to_buffer(temp_1, 15, 30 + (i * 150), (480 + ((count - 3) * 150)), src->menu_buffer, 1);
        printf("%s已加入菜单\n", temp_1);
        i++;
        bzero(temp, 100);
        bzero(temp_1, 100);
    }
}

//将缓存复制到显存中显示
void display_menu(struct resource *src, int st_x, int st_y)
{
    int x, y;
    for (y = 0; y < 480; y++)
    {
        memcpy((src->framebuffer_addr) + 600 + 800 * y, (src->menu_buffer) + 200 * y + st_y * 200, 200 * 4);
    }
}


