#include "../include/malloc.h"
#include <string.h> // pour memset

t_zone_type get_zone_type(size_t size)
{
	if (size <= TINY_LIMIT)
		return TINY;
	if (size <= SMALL_LIMIT)
		return SMALL;
	return LARGE;
}

size_t get_zone_size(t_zone_type type)
{
	if (type == TINY)
		return TINY_ZONE_SIZE;
	if (type == SMALL)
		return SMALL_ZONE_SIZE;
	return 0;
}

t_zone *create_zone(t_zone_type type)
{
	size_t zone_size = get_zone_size(type);
	void *ptr = mmap(NULL, zone_size, PROT_READ | PROT_WRITE,
					 MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (ptr == MAP_FAILED)
		return NULL;

	t_zone *zone = (t_zone *)ptr;
	zone->size = zone_size;
	zone->blocks = NULL;
	zone->next = NULL;
	return zone;
}

t_block *find_free_block(t_zone *zone, size_t size)
{
	t_block *current = zone->blocks;
	while (current)
	{
		if (current->is_free && current->size >= size)
			return current;
		current = current->next;
	}
	return NULL;
}

t_block *add_block_to_zone(t_zone *zone, size_t size)
{
	t_block *block;

	if (!zone->blocks)
	{
		block = (t_block *)((char *)zone + sizeof(t_zone));
		zone->blocks = block;
	}
	else
	{
		block = zone->blocks;
		while (block->next)
			block = block->next;
		block = block->next = (t_block *)((char *)block + sizeof(t_block) + block->size);
	}
	block->size = size;
	block->is_free = 0;
	block->next = NULL;
	return block;
}
