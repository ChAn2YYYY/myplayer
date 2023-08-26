#include <video.h>
#include <play_list.h>
#include <touchscreen.h>
#include <display.h>
#include <unistd.h>
#include <stdbool.h>

pthread_mutex_t mutex; //用于线程池同步互斥的互斥锁
pthread_cond_t cond;   //用于让线程池里面的线程睡眠的条件变量
pthread_t tid[3];

static bool pause_flag = false;

//钩子，防死锁
void handler(void *arg)
{
    pthread_mutex_unlock((pthread_mutex_t *)arg); //防止死锁，所以在这里添加解锁操作
}

//接收触摸屏层发过来的命令，向有名管道发送命令
void *command_receive(void *arg)
{
    int flag = 1;
    struct resource *src = (struct resource *)arg;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);  //使能取消线程
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL); //立即响应
    char cmd[100];
    //读取控制命令
    while (1)
    {
        bzero(cmd, 100);
        //获取控制层信息
        get_command(src, pause_flag, cmd);
        printf("%s\n", cmd);
        pthread_mutex_lock(&mutex); //加锁
        if (strncmp(cmd, "pause", 5) == 0)
        {
            flag = -flag;
            if (flag < 0)
            {
                pause_flag = true;
            }
            else if (flag > 0)
            {
                pause_flag = false;
                pthread_cond_signal(&cond);
            }
        }
        // de seek
        if (strncmp(cmd, "seek", 4) == 0)
        {
            if (flag < 0)
            {
                flag = -flag;
                pause_flag = false;
                pthread_cond_signal(&cond);
            }
        }
        pthread_mutex_unlock(&mutex);          //解锁
        write(src->fifo_fd, cmd, strlen(cmd)); //发送命令到有名管道中
        if (strncmp(cmd, "quit", 4) == 0)
        {
            write(src->fifo_fd, cmd, strlen(cmd)); //发送命令到有名管道中
            pause_flag = false;
            pthread_cancel(tid[1]);
            pthread_cancel(tid[2]);
            pthread_cancel(tid[0]);
            break;
        }
    }
    pthread_exit(NULL);
}

void *send_percent(void *arg)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);  //使能取消线程
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL); //立即响应

    struct resource *src = (struct resource *)arg;
    char buf[128] = "";

    while (1)
    {
        pthread_cleanup_push(handler, (void *)&mutex);
        pthread_mutex_lock(&mutex); //加锁

        while (pause_flag == true) //判断是否没有需要运行的任务
        {
            pthread_cond_wait(&cond, &mutex); //让线程睡眠
        }

        // pthread_mutex_unlock(&mutex); //解锁
        pthread_cleanup_pop(0); //解除登记取消线程之后所做的函数

        bzero(buf, sizeof(buf));
        strcpy(buf, "get_time_pos\n"); //获取实时进度

        if (write(src->fifo_fd, buf, strlen(buf)) != strlen(buf))
            perror("write"); //将命令写入命名管道
        write(src->fifo_fd, "get_time_length\n", strlen("get_time_length\n"));
        pthread_mutex_unlock(&mutex); //解锁
        sleep(1);                     //降低性能消耗
    }
}

void *recv_percent(void *arg)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);      //使能取消线程
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL); //立即响应

    struct resource *src = (struct resource *)arg;
    char buf[128] = "";

    while (1)
    {
        pthread_cleanup_push(handler, (void *)&mutex);
        pthread_mutex_lock(&mutex); //加锁

        while (pause_flag == true) //判断是否没有需要运行的任务
        {
            pthread_cond_wait(&cond, &mutex); //让线程睡眠
        }

        pthread_mutex_unlock(&mutex); //解锁
        pthread_cleanup_pop(0);       //解除登记取消线程之后所做的函数

        read(src->pipe_fd[0], buf, sizeof(buf)); //从无名管道的写端读取信息打印在屏幕上

        show_percent(src, buf);
    }
}

int play_the_video(struct resource *src)
{
    // system("killall -9 mplayer"); //杀死上一个进程
    display_touch_bar(src, 1);
    src->fifo_fd = open("myfifo", O_RDWR);
    char video_name[30];
    //测试断点
    printf("0.src->video_order == %d\n", src->video_order);
    //
    strcpy(video_name, order_video(src, src->video_order));
    printf("开始播放：%s\n", order_video(src, src->video_order));

    if (pipe(src->pipe_fd) < 0) //创建无名管道
    {
        perror("pipe");
        return -1;
    }

    pid_t pid;
    pid = fork();
    if (pid == 0) //子进程打开mpalyer播放视频
    {
        close(src->pipe_fd[0]);
        dup2(src->pipe_fd[1], 1); //将子进程的标准输出重定向到管道的写端
        char temp[100];
        sprintf(temp, "../video/%s", video_name);
        src->fifo_fd = open("myfifo", O_RDWR);
        // execlp("mplayer", "mplayer", "-slave", "-quiet", "-input", "file=/home/fei/temp/video/myfifo",temp, NULL);
        execlp("mplayer", "mplayer", "-slave", "-quiet", "-input", "file=./myfifo", "-zoom", "-x", "600",
               "-y", "380", temp, NULL);
        exit(0);
    }

    else if (pid > 0)
    {
        //创造接收触摸屏层发过来的命令，向有名管道发送命令
        pthread_create(&tid[0], NULL, command_receive, (void *)src);
        pthread_create(&tid[1], NULL, send_percent, (void *)src);
        pthread_create(&tid[2], NULL, recv_percent, (void *)src);
        pthread_mutex_init(&mutex, NULL); //初始化互斥锁
        pthread_cond_init(&cond, NULL);   //初始化条件变量
        wait(NULL);
        pthread_cancel(tid[0]);
        pthread_cancel(tid[1]);
        pthread_cancel(tid[2]);
    }

    int i;
    for (i = 0; i < 3; i++)
    {
        pthread_join(tid[i], NULL);
    }
    close(src->fifo_fd);
    close(src->pipe_fd[0]);
    close(src->pipe_fd[1]);
    return 0;
}