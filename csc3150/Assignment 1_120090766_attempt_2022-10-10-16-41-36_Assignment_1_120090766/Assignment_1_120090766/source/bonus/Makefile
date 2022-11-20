.SUFFIXES:.cpp .o

CC=g++

SRCS=pstree.cpp
OBJS=$(SRCS:.cpp=.o)
EXEC=pstree

start: $(OBJS)
	$(CC) -o $(EXEC) $(OBJS)
	@echo "-----------------------------OK-----------------------"

.cpp.o:
	$(CC) -Wall -o $@ -c $<

clean:
	rm -rf $(EXEC) $(OBJS)
