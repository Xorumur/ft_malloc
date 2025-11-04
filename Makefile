ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME      = libft_malloc_$(HOSTTYPE).so
LIB_NAME  = libft_malloc.so
PATH_SRC  = src
PATH_INC  = include
PATH_OBJ  = obj

SOURCES   = malloc.c free.c show_alloc_mem.c realloc.c ./utils/libft.c ./utils/heap_managment.c zone.c
OBJECTS   = $(addprefix $(PATH_OBJ)/, $(SOURCES:.c=.o))

CC        = gcc
CFLAGS    = -Wall -Wextra -Werror -fPIC -I $(PATH_INC)
LDFLAGS   = -shared

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^
	ln -sf $@ $(LIB_NAME)

$(PATH_OBJ)/%.o: $(PATH_SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

main: main.c $(NAME)
	$(CC) -o $@ $< -I $(PATH_INC) -L. -lft_malloc -Wl,-rpath=.

run: main
	LD_LIBRARY_PATH=. ./main

# Dossier des tests
EVAL_DIR  = evaluation
TESTS     = test0 test1 test2 test3 test4 test5 test6 test7

# Règle générique pour compiler un test
$(TESTS): %: $(EVAL_DIR)/%.c $(NAME)
	$(CC) -o $@ $< -I $(PATH_INC) -L. -lft_malloc -Wl,-rpath=.

# Règle pour lancer un test
run_%: %
	LD_LIBRARY_PATH=. ./$<

# Exemple : make run_test0 → compile et exécute test0

clean:
	rm -rf $(PATH_OBJ)

fclean: clean
	rm -f $(NAME) $(LIB_NAME) main $(TESTS)

re: fclean all

.PHONY: all clean fclean re run main
