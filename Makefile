include make.inc

PLUGIN_SOURCE_FILES = $(NAME).c
PLUGIN_OBJECT_FILES = $(NAME).o
GCCPLUGINS_DIR = $(shell $(GCC) -print-file-name=plugin)
CFLAGS += -I$(GCCPLUGINS_DIR)/include \
          $(EXTRA_CFLAGS) -fPIC -g3 -O0 -Wall -pedantic -std=c99

$(NAME).so: $(PLUGIN_OBJECT_FILES)
	$(CC) -g -O0 -shared $^ -o $@ $(CFLAGS)

tests: $(NAME).so cleantests
	$(MAKE) -C tests

debug: $(NAME).so tests/test_c/f1.c
	gdb --args $(DBG_GCC) tests/test_c/f2.c -c $(PLUGIN) -O0 -g3

clean: cleantests
	rm -fv $(NAME).so *.o
	
cleantests:
	$(MAKE) clean -C tests
