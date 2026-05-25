NAME = codexion
CC = cc 
CFLAGS = -Wall -Wextra -Werror -pthread -Iinclude
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include
SRCS_FILES = main.c parser.c actions.c scheduler.c threads.c threads_utils.c utils.c init.c
SRCS = $(addprefix $(SRC_DIR)/, $(SRCS_FILES))
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS_FILES:.c=.o))
HEADER = $(INC_DIR)/codexion.h 

GREEN = \033[0;32m
RED = \033[0;31m
RESET = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
		@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
		@echo "$(GREEN)Codexion compiled successfully$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER)
			@mkdir -p $(OBJ_DIR)
			@$(CC) $(CFLAGS) -c $< -o $@

debug:
	$(CC) $(CFLAGS) -O3 -fsanitize=thread -g -Iinc src/*.c -lpthread -o codexion
	setarch -R ./codexion 5 800 200 200 200 5 0 fifo

clean:
		@rm -rf $(OBJ_DIR)
		@echo "$(RED)Objects removed$(RESET)"
	
fclean:	clean
		@rm -rf $(NAME)
		@echo "$(RED)Build files cleaned$(RESET)"

re: fclean all

.PHONY: all clean fclean re