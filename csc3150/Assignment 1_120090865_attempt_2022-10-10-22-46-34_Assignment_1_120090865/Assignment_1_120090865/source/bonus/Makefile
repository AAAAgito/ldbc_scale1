OBJS=pstree.o basic.o get.o sort.o build.o
CC=gcc
CFLAGS+=-c -g

pstree:$(OBJS)
	$(CC) $^ -o $@

%.o:%.c
	$(CC) $^ $(CFLAGS) -o $@

clean:
	$(RM) *.o pstree -r
