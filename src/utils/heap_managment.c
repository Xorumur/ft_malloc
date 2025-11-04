#include "../../include/malloc.h"

int	is_managed_pointer(void *ptr)
{
	t_zone *zone;
	t_block *block;

	for (zone = g_malloc.tiny; zone; zone = zone->next)
		for (block = zone->blocks; block; block = block->next)
			if ((void *)(block + 1) == ptr)
				return 1;

	for (zone = g_malloc.small; zone; zone = zone->next)
		for (block = zone->blocks; block; block = block->next)
			if ((void *)(block + 1) == ptr)
				return 1;

	for (zone = g_malloc.large; zone; zone = zone->next)
		if ((void *)(zone->blocks + 1) == ptr)
			return 1;

	return 0;
}