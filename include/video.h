#ifndef __VIDEO_H__
#define __VIDEO_H__

#include <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/mman.h>



#define VIDEO_FOLDER "../video"


typedef struct video
{
    char name[30];
    struct list_head list;
} video_t;

struct resource
{
    video_t *video_list_head;
    int fifo_fd;
    int pipe_fd[2];
    int touch_fd;
    unsigned int *framebuffer_addr;
    unsigned int *menu_buffer;
    int video_order;
    int buffer_height;
    int video_sum;
};

extern int play_the_video(struct resource *src);

#endif