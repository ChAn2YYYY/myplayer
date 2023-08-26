#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <display.h>

/*
    函数功能：
        打开显存文件及映射显存地址
    参数：
        framebuffer_addr：用来存放显存的地址
    返回值：
        返回一个显存的文件描述符，失败返回-1
*/
int open_framebuffer_device(unsigned int **framebuffer_addr)
{
    int framebuffer_fd;

    //打开显存文件
    framebuffer_fd = open("/dev/fb0", O_RDWR);
    if(framebuffer_fd == -1)
    {
        fprintf(stderr, "打开显存设备文件失败:%s", strerror(errno));
        return -1;
    }

    //映射显存地址
    *framebuffer_addr = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED, framebuffer_fd, 0);
    if(*framebuffer_addr == MAP_FAILED)
    {
        fprintf(stderr, "映射显存设备文件失败:%s", strerror(errno));
        goto map_framebuffer_err;
    }

    return framebuffer_fd;

map_framebuffer_err:
    close(framebuffer_fd);

    return -1;

}


int close_framebuffer_device(int framebuffer_fd, unsigned int *framebuffer_addr)
{
    //取消映射显存地址
    munmap(framebuffer_addr, 800*480*4);

    //关闭显存文件
    return close(framebuffer_fd);
}



