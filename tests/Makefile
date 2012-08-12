include ../make.inc

SUBDIRS=test_c

all: tests

tests: test $(SUBDIRS)

test: test.c
	$(GCC) test.c -o $@ $(PLUGIN)

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@

clean:
	rm -f test
	@for i in $(SUBDIRS); do $(MAKE) clean -C $$i; done
