CFILES:= $(shell ls|grep .c)
PROGS:=$(patsubst %.c,%,$(CFILES))
PSTREE:=$(./pstree)

all: $(PROGS)

%:%.c
	$(CC) -o $@ $<

clean:$(PROGS)
	rm $(PROGS)

pstree:$(PSTREE)