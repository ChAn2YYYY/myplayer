
CC=arm-linux-gcc#判断该变量是否被赋值过，如果没有我们赋值为gcc

COM_INCLUDE=-I ../include

COM_LIB=-L ../lib -l jpeg -l font -lm

IMAGE?=player
SRC=$(wildcard *.c)#找到本文件夹下面所有的.c文件
OBJ=$(SRC:%.c=%.o)#将SRC里面的.c尾缀替换成.o尾缀

$(IMAGE):$(OBJ)
	$(CC) $^ -o $@  -L ../lib -l jpeg -l font -lm -pthread

$(OBJ):%.o:%.c
	$(CC) $^ -o $@ -c -I ../include

.PHONY clean:
	@rm $(IMAGE) $(OBJ) -rf


