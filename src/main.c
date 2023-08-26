#include <video.h>
#include <play_list.h>
#include <display.h>
#include <touchscreen.h>

struct resource src;

pthread_t m_tid;

int main(int argc, char *argv[])
{
    int framebuffer_fd;
    //初始化
    src.video_list_head = request_video_node();
    strcpy(src.video_list_head->name, "NULL");
    if (src.video_list_head == NULL)
    {
        perror("request video head node failed");
        return 0;
    }

    INIT_LIST_HEAD(&src.video_list_head->list);

    make_video_list(&src);

    src.video_order = 1;
    printf("main.src->video_order == %d\n", src.video_order);
    //打开触摸屏
    src.touch_fd = open_touchscreen_device();
    //打开显存
    framebuffer_fd = open_framebuffer_device(&src.framebuffer_addr);
    //创建菜单缓存
    int count;
    count = foreach_the_video_list(&src);

    //缓存申请
    src.menu_buffer = malloc(800 * (480 + ((count - 3) * 150)) * 4);

    /*
        测试
    */
    init_video_menu(&src);

    display_menu(&src, 0, 0);

    //创建管道文件，
    if (access("myfifo", F_OK))
    {
        mkfifo("myfifo", O_CREAT | 0666);
    }
 
    //显示播放栏
    display_touch_bar(&src, 1);
    
    while (1)
    {
        play_the_video(&src);
        src.video_order++;
        usleep(50000);
    }

    close_framebuffer_device(framebuffer_fd, src.framebuffer_addr);
    free(src.menu_buffer);
    return 0;
}