include make.inc

PLUGIN_SOURCE_FILES = $(NAME).c
PLUGIN_OBJECT_FILES = $(NAME).o
GCCPLUGINS_DIR = $(shell $(GCC) -print-file-name=plugin)
CFLAGS += -I$(GCCPLUGINS_DIR)/include -fPIC -g3 -O0 -Wall #-pedantic -std=c99
DBG_CC = /home/enferex/docs/edu/go/dev/gcc-obj-4.7.1/gcc/cc1

$(NAME).so: $(PLUGIN_OBJECT_FILES)
	$(CC) -g -O0 -shared $^ -o $@ $(CFLAGS)

tests: $(NAME).so cleantests
	$(MAKE) -C tests

debug: $(NAME).so tests/test.c
	gdb --args $(DBG_CC) tests/test.c -o test $(PLUGIN) -O0 -g3

clean: cleantests
	rm -fv $(NAME).so *.o
	
cleantests:
	$(MAKE) clean -C tests
