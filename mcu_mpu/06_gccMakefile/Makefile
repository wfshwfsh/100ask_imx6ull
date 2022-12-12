
CC=gcc

C_OBJS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(C_OBJS))

dep_files := $(patsubst %,.%.d, $(OBJS))
dep_files := $(wildcard $(dep_files))

all: main

ifneq ($(dep_files),)
include $(dep_files)
endif

main: $(OBJS)
	$(CC) -o main.app $^

%.o: %.c
	$(CC) -c -o $@ $< -MMD -MF .$@.d

clean:
	rm -rf *.o *.app .*.d

test:
	gcc -M -MF b.d b.c 
	gcc -MD -MF b.d b.c 

help: a.c b.c
	@echo $(C_OBJS)
	@echo $(OBJS)
	@echo $@
	@echo $^
	@echo $<
	