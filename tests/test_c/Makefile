include ../../make.inc

APP=test
OBJS=f1.o f2.o

all: $(APP)

$(APP): $(OBJS) main.c
	$(CC) -o $@ $^ $(PLUGIN)

%.o: %.c
	$(CC) -c -o $@ $^ $(PLUGIN)

clean:
	rm -rfv $(APP) $(OBJS)
