
NAME = codexion

CC = cc

CFLAGS = -Wall -Wextra -Werror

CODEXION_SRC = parser.c proccess.c operations.c

CODEXION_OBJS = $(CODEXION_SRC:.c=.o)

all: $(NAME)

$(NAME): $(CODEXION_OBJS)
	$(CC) $(CFLAGS) codexion.c $(CODEXION_OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./codexion 5 1000 100 50 50 5 150 fifo

run2:
	clear
	./codexion 10 2000 100 100 100 15 50 fifo
clean:
	rm -rf $(CODEXION_OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean fclean re all
