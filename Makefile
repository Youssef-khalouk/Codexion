
NAME = codexion

CC = cc

CFLAGS = -Wall -Wextra -Werror

CODEXION_SRC =

CODEXION_OBJS = $(CODEXION_SRC:.c=.o)

all: $(NAME)

$(NAME): $(CODEXION_OBJS)
	$(CC) $(CFLAGS) codexion.c $(CODEXION_OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(CODEXION_OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean fclean re all
