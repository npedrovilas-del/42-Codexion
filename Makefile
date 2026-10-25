NAME	= codexion
CC		= cc
CFLAGS	= -Wall -Wextra -Werror -pthread
SRCS	= src/main.c \
	src/parser.c \
	src/time.c \
	src/init.c \
	src/init_dongles.c \
	src/heap.c \
	src/heap_ops.c \
	src/dongles.c \
	src/logs.c \
	src/coders.c \
	src/monitor.c

all: $(NAME)

$(NAME):
	$(CC) $(CFLAGS) -Isrc $(SRCS) -o $(NAME)

clean:
	rm -rf *.o

fclean: clean
	rm -f $(NAME)

re: fclean all
