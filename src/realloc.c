#include "../include/malloc.h"
#include <string.h> // memcpy

void *realloc(void *ptr, size_t size)
{
	void *new_ptr;
	t_block *block;

	if (!ptr)
		return malloc(size);
	if (size == 0)
	{
		free(ptr);
		return NULL;
	}

	block = ((t_block *)ptr) - 1;

	pthread_mutex_lock(&g_malloc.lock);

	if (block->size >= size)
	{
		pthread_mutex_unlock(&g_malloc.lock);
		return ptr;
	}

	pthread_mutex_unlock(&g_malloc.lock);

	new_ptr = malloc(size);
	if (!new_ptr)
		return NULL;

	memcpy(new_ptr, ptr, block->size);
	free(ptr);

	return new_ptr;
}
