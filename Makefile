
NAME = codexion

CC = cc

CFLAGS = -Wall -Wextra -Werror #-fsanitize=thread

CODEXION_SRC = surce_code/parser.c surce_code/queue_functions.c \
				surce_code/proccess.c surce_code/operations.c \
				surce_code/dongle_utils.c surce_code/init_utils.c \
				surce_code/monitor.c surce_code/dongle_utils2.c

CODEXION_OBJS = $(CODEXION_SRC:.c=.o)

all: $(NAME)

$(NAME): $(CODEXION_OBJS)
	$(CC) $(CFLAGS) surce_code/codexion.c $(CODEXION_OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./codexion 5 1000 100 50 50 5 150 fifo

run2:
	clear
	./codexion 20 110 50 20 10 20 20 edf
clean:
	rm -rf $(CODEXION_OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean fclean re all
