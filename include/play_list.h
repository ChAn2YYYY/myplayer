#ifndef __PLAY_LIST_H_
#define __PLAY_LIST_H_

extern video_t *request_video_node(void);

extern int make_video_list(struct resource *src);

extern int insert_video(struct resource *src,char *name);

extern char *order_video(struct resource *src,int idx);

extern int foreach_the_video_list(struct resource *src);


#endif