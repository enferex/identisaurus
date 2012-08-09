include ../../make.inc

GOCC=gccgo
APP=test
LIB=foo_package.o

all: $(APP)

$(LIB): f1.go f2.go
	$(GOCC) -o $@ $(PLUGIN) $^

$(APP): $(LIB) main.go
	$(GOCC) -o $@ $^

clean:
	rm -rfv $(APP) $(LIB)
