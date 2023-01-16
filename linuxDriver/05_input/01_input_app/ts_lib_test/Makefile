
CC?=$(CROSS_COMPILE)gcc

C_OBJS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(C_OBJS))

dep_files := $(patsubst %,.%.d, $(OBJS))
dep_files := $(wildcard $(dep_files))

TAR = main
#TAR = 1_input_info
#TAR = 2_input_read

all: $(TAR)

ifneq ($(dep_files),)
include $(dep_files)
endif

#$(TAR): $(OBJS)
$(TAR): $(TAR).o
	$(CC) -o $(TAR).app $^ -DDEBUG

%.o: %.c
	$(CC) -c -o $@ $< -MMD -MF .$@.d -lts

clean:
	rm -rf *.o *.app .*.d

install:
	cp $(TAR).app /nfsshare

test:
	gcc -M -MF b.d b.c 
	gcc -MD -MF b.d b.c 

help: a.c b.c
	@echo $(C_OBJS)
	@echo $(OBJS)
	@echo $@
	@echo $^
	@echo $<
	
