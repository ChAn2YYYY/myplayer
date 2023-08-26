#include <list.h>
#include <video.h>

//申请视频链表节点
video_t *request_video_node(void)
{
    video_t *new_node;
    new_node = (video_t *)malloc(sizeof(video_t));
    return new_node;
}

//遍历视频文件夹，将文件夹的视频加入链表中
int make_video_list(struct resource *src)
{
    DIR *dir;
    struct dirent *entry;
    dir = opendir(VIDEO_FOLDER);
    if (dir == NULL)
    {
        perror("打开文件夹失败");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        { // 检查是否是普通文件
            insert_video(src, entry->d_name);
        }
    }
    //显示链表内容
    video_t *pos;
    list_for_each_entry(pos, &src->video_list_head->list, list)
    {
        printf("%s已加入播放列表\n", pos->name);
    }
    closedir(dir);
    return 0;
}

//将视频信息插入链表
int insert_video(struct resource *src, char *name)
{
    video_t *new_node;
    new_node = request_video_node();
    if (new_node == NULL)
    {
        perror("申请新的电影节点失败");
        return -1;
    }
    strcpy(new_node->name, name);
    list_add_tail((&new_node->list), &(src->video_list_head->list));

    return 0;
}

//按次序返回视频名字
char *order_video(struct resource *src, int idx)
{
    int max = foreach_the_video_list(src);
    if (idx > max)
    {
        idx = 1;
        src->video_order = 1;
    }
    
    int count = 0;
    video_t *pos;
    if (idx <= 0)
    {
        return NULL;
    }
    else
    {
        list_for_each_entry(pos, &src->video_list_head->list, list)
        {
            count++;
            if (count == idx && pos->name != NULL)
            {
                printf("1.src->video_order == %d\n", src->video_order);
                return pos->name;
            }
        }
    }
}

//统计文件夹内视频数量
int foreach_the_video_list(struct resource *src)
{
    int count = 0;
    video_t *pos;
    list_for_each_entry(pos, &src->video_list_head->list, list)
    {
        count++;
    }
    src->video_sum = count;
    printf("总共有影片%d部\n",count);
    return count;
}
