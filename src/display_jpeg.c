#include <stdio.h>
#include <jpeglib.h>
#include <display.h>
#include <setjmp.h>
#include <stdlib.h>

extern JSAMPLE * image_buffer;	/* Points to large array of R,G,B-order data */
extern int image_height;	/* Number of rows in image */
extern int image_width;		/* Number of columns in image */

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void) my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

int display_jpeg_format_to_framebuffer(const char *filename, int x_pos, int y_pos, unsigned int *framebuffer_addr, float mul)
{
  
  struct jpeg_decompress_struct cinfo;
  struct my_error_mgr jerr;
  
  FILE * infile;		/* source file */
  char *buffer;		/* Output row buffer */
  int row_stride;		/* physical row width in output buffer */
  int x, y;

  //打开图片文件
  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return -1;
  }

  //注册出错处理
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  
  if (setjmp(jerr.setjmp_buffer)) {
    
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return -1;
  }
  
  //创建解码
  jpeg_create_decompress(&cinfo);

  //指定数据源
  jpeg_stdio_src(&cinfo, infile);

  //读取文件头部信息
  jpeg_read_header(&cinfo, TRUE);

  //开始解码
  jpeg_start_decompress(&cinfo);
  
  //计算出一行的字节数
  row_stride = cinfo.output_width * cinfo.output_components;
 
  //申请一块内存用来存放一行颜色数据
  buffer = malloc(row_stride);
  
  y=y_pos;

  char *pos;
  unsigned int color;

  char *data_pos;
	float point_distance = 1/mul;//得到每个位移的长度（是个小数），假如mul=0.7, 相当于10/7，意思是原本10个像素点每次都+1，分成10次读完
									//现在10/7的意思是分成7次读完10个像素点长度的内容，计算出他每次读取的长度是多少，也就是1.多
	float x_data_offset, y_data_offset=point_distance;//x_data_offset存放x获取点距离开头的偏移量，y_data_offset存放y获取点距离开头的偏移量
	int i=0;//记录已经获取到几行

	while(cinfo.output_scanline < cinfo.output_height) 
	{    
		data_pos = buffer;//还原每一行的数据位置
		
    // 读取jpeg图片的每一行
		for( ;i<y_data_offset && cinfo.output_scanline < cinfo.output_height; i+=1)
		{
			jpeg_read_scanlines(&cinfo, (JSAMPARRAY)&data_pos, 1);
		}
		y_data_offset+=point_distance;

		/*
			x_data_offset=point_distance,先记录一开始的偏移量大小
			x_data_offset+=point_distance，每次都让偏移量加上每个点的距离
		*/
		for(x=x_pos, x_data_offset=point_distance; x<x_pos+cinfo.output_width*mul; x++, x_data_offset+=point_distance)
		{
			if(x>=800 || y>= 480)
				break;

			color = data_pos[0]<<16 | data_pos[1]<<8 |data_pos[2];

			display_framebuffer_point(x, y, color, framebuffer_addr);

			//buffer就是一行数据开头的位置再去加上每一个点的偏移量，(int)x_data_offset是为了保留偏移量的整数位，小数位不要
			data_pos = buffer+3*(int)x_data_offset;
		}

		y++;
	}


  free(buffer);

  //结束解码
  jpeg_finish_decompress(&cinfo);
  
  //销毁解码
  jpeg_destroy_decompress(&cinfo);

  //关闭文件
  fclose(infile);

  return 0;
}

int display_jpeg_format_to_buffer(const char *filename, int x_pos, int y_pos, int buffer_height,unsigned int *framebuffer_addr, float mul)
{
  
  struct jpeg_decompress_struct cinfo;
  struct my_error_mgr jerr;
  
  FILE * infile;		/* source file */
  char *buffer;		/* Output row buffer */
  int row_stride;		/* physical row width in output buffer */
  int x, y;

  //打开图片文件
  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return -1;
  }

  //注册出错处理
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  
  if (setjmp(jerr.setjmp_buffer)) {
    
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return -1;
  }
  
  //创建解码
  jpeg_create_decompress(&cinfo);

  //指定数据源
  jpeg_stdio_src(&cinfo, infile);

  //读取文件头部信息
  jpeg_read_header(&cinfo, TRUE);

  //开始解码
  jpeg_start_decompress(&cinfo);
  
  //计算出一行的字节数
  row_stride = cinfo.output_width * cinfo.output_components;
 
  //申请一块内存用来存放一行颜色数据
  buffer = malloc(row_stride);
  
  y=y_pos;

  char *pos;
  unsigned int color;

  char *data_pos;
	float point_distance = 1/mul;//得到每个位移的长度（是个小数），假如mul=0.7, 相当于10/7，意思是原本10个像素点每次都+1，分成10次读完
									//现在10/7的意思是分成7次读完10个像素点长度的内容，计算出他每次读取的长度是多少，也就是1.多
	float x_data_offset, y_data_offset=point_distance;//x_data_offset存放x获取点距离开头的偏移量，y_data_offset存放y获取点距离开头的偏移量
	int i=0;//记录已经获取到几行

	while(cinfo.output_scanline < cinfo.output_height) 
	{    
		data_pos = buffer;//还原每一行的数据位置
		
    // 读取jpeg图片的每一行
		for( ;i<y_data_offset && cinfo.output_scanline < cinfo.output_height; i+=1)
		{
			jpeg_read_scanlines(&cinfo, (JSAMPARRAY)&data_pos, 1);
		}
		y_data_offset+=point_distance;

		/*
			x_data_offset=point_distance,先记录一开始的偏移量大小
			x_data_offset+=point_distance，每次都让偏移量加上每个点的距离
		*/
		for(x=x_pos, x_data_offset=point_distance; x<x_pos+cinfo.output_width*mul; x++, x_data_offset+=point_distance)
		{
			if(x>=800 || y>= buffer_height)
				break;

			color = data_pos[0]<<16 | data_pos[1]<<8 |data_pos[2];

			display_buffer_point(x, y, color, framebuffer_addr);

			//buffer就是一行数据开头的位置再去加上每一个点的偏移量，(int)x_data_offset是为了保留偏移量的整数位，小数位不要
			data_pos = buffer+3*(int)x_data_offset;
		}

		y++;
	}


  free(buffer);

  //结束解码
  jpeg_finish_decompress(&cinfo);
  
  //销毁解码
  jpeg_destroy_decompress(&cinfo);

  //关闭文件
  fclose(infile);

  return 0;
}
