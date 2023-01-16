
CC=$(CROSS_COMPILE)gcc

C_OBJS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(C_OBJS))

dep_files := $(patsubst %,.%.d, $(OBJS))
dep_files := $(wildcard $(dep_files))

all: i2c_test

ifneq ($(dep_files),)
include $(dep_files)
endif

i2c_test: $(OBJS)
	$(CC) -o i2c_test.app $^

%.o: %.c
	$(CC) -c -o $@ $< -MMD -MF .$@.d

clean:
	rm -rf *.o *.app .*.d

help: 
	@echo $(C_OBJS)
	@echo $(OBJS)
	@echo $@
	@echo $^
	@echo $<
	
