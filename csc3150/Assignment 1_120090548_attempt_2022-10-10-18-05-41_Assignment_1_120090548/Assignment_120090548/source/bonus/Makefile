NAME = pstree 
CC = gcc 
CCFLAGS = -Wall -Wextra -Werror 
CCFLAGS += -g 
DEL = rm -rf 
FILES = pstree.c 
OBJ = $(FILES:.c=.o) 
%.o : %.c 
		@$(CC) $(CFLAGS) -c $< -o $@ 
all: $(NAME) 
$(NAME): $(OBJ) 
		@$(CC) $(CCFLAGS) -o $(NAME) $(OBJ) 
		@echo "Run ./pstree" 
clean: 
	@$(DEL) $(OBJ) 
fclean: clean 
		@$(DEL) $(NAME) 
.PHONY: all clean fclean 