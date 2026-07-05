NAME     = codexion
CC       = cc
CFLAGS   = -Wall -Wextra -Werror -pthread
INCLUDES = -I include/

SRCS     = src/main.c src/init.c src/coder.c src/dongle.c \
           src/scheduler.c src/monitor.c src/log.c src/utils.c

HEADER   = include/codexion.h

OBJS     = $(patsubst src/%.c, objects/%.o, $(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

objects/%.o: src/%.c $(HEADER)
	mkdir -p objects
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf objects

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re