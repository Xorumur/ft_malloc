#include <string.h>

void	print(char *s)
{
	write(1, s, strlen(s));
}

int		main(void)
{
	char *addr;

	addr = malloc(16);
    show_alloc_mem();
	free(NULL);
	free((void *)addr + 5);
    show_alloc_mem();
	if (realloc((void *)addr + 5, 10) == NULL)
		print("Bonjours\n");
	return (0);
}