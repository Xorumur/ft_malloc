#include <stdlib.h>
#include <stdio.h>

extern void show_alloc_mem(void);

int main(void)
{
	void *a = malloc(32);     // TINY
	void *b = malloc(2048);   // SMALL
	void *c = malloc(200000); // LARGE

	show_alloc_mem(); // tu dois voir seulement les blocs non libérés

	free(a);
	free(b);
	free(c);

	show_alloc_mem(); // tu dois voir seulement les blocs non libérés


	write(2, "Test realloc:\n", 14);
	char *s = malloc(10);
	if (!s) return;
	strcpy(s, "42");
	char *r = realloc(s, 50);
	strcpy(r + 2, "School");
	write(1, r, 10); // affichera "42School"
	write(1, "\n", 1);
	free(r);

	return 0;
}
