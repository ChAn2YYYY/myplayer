#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/input.h>

#define TOUCHSCREEN_GET_COORDINATE 0
#define TOUCHSCREEN_TOUCH_STATUS 1
#define TOUCHSCREEN_RELEASE_STATUS 2
#define TOUCHSCREEN_ERROR_STATUS -1

int open_touchscreen_device(void)
{
    int ts_fd;

    // 打开触摸屏设备文件
    ts_fd = open("/dev/input/event0", O_RDONLY);
    if (ts_fd == -1)
    {
        fprintf(stderr, "打开触摸屏失败:%s\n", strerror(errno));
        return -1;
    }

    return ts_fd;
}

int close_touchscreen_device(int ts_fd)
{
    // 关闭触摸屏设备文件
    return close(ts_fd);
}

int get_touchscreen_coordinate(int ts_fd, int *get_x, int *get_y)
{
    int status;
    static int x, y; //为了一直保持上次的初值
    ssize_t rd_size;
    struct input_event event_info;

    while (1)
    {
        // 读取触摸屏设备文件
        rd_size = read(ts_fd, &event_info, sizeof(event_info)); // 阻塞特性（如果没有数据，该文件的读取将会阻塞（卡住））
        if (rd_size == -1)
        {
            fprintf(stderr, "读取触摸屏失败:%s\n", strerror(errno));
            return TOUCHSCREEN_ERROR_STATUS;
        }

        // 开始分析数据
        // 判断你的事件类型
        if (event_info.type == EV_ABS) // 判断是否为绝对坐标事件
        {
            // 判断你是x还是y坐标
            if (event_info.code == ABS_X)
            {
                x = event_info.value;
            }
            else if (event_info.code == ABS_Y)
            {
                y = event_info.value;
            }

            status = TOUCHSCREEN_GET_COORDINATE;
        }
        else if (event_info.type == EV_KEY) // 判断是否是按键事件
        {
            // 判断你是松开还是按下
            if (event_info.code == BTN_TOUCH)
            {
                if (event_info.value == 1) // 代表被按下了
                {
                    printf("触摸屏被按下了\n");
                    status = TOUCHSCREEN_TOUCH_STATUS;
                }
                else
                {
                    printf("触摸屏被松开了\n");
                    status = TOUCHSCREEN_RELEASE_STATUS;
                }
            }
        }
        else
        {
            // 这个结束包
            *get_x = x * 800.0 / 1024;
            *get_y = y * 480.0 / 600;
            printf("数据读取结束,状态%d\n", status);
            break;
        }
    }

    return status; //返回状态值
}

//触碰区域判断,传入的是触碰到的get_x,get_y坐标，判断是否在规定的传入参数st_x,end_x,st_y,end_y范围内
int judge_touch_area(int get_x, int get_y, int st_x, int end_x, int st_y, int end_y)
{
    if (get_x >= st_x && get_x <= end_x && get_y >= st_y && get_y <= end_y)
    {
        return 0;//在范围内返回0
    }
    else
    {
        return 1;//不在范围内返回1
    }
}