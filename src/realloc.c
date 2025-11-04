#include "../include/malloc.h"
#include <stdio.h>

void *realloc(void *ptr, size_t size)
{
	void *new_ptr;
	t_block *block;

	if (!ptr)
		return malloc(size);
	if (!is_managed_pointer(ptr))
		return NULL;
	if (size == 0)
	{
		free(ptr);
		return NULL;
	}

	block = ((t_block *)ptr) - 1;

	if (block == NULL) {
		return NULL;
	}


	if (block->size >= size)
	{
		return ptr;
	}

	new_ptr = malloc(size);
	if (!new_ptr)
		return NULL;

	ft_memcpy(new_ptr, ptr, block->size);
	free(ptr);

	return new_ptr;
}
