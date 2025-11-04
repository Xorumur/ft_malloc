// #include "../include/malloc.h"
// #include <string.h>
// #include <unistd.h>
// #include <sys/mman.h>

// #define ALIGN(x) (((x) + 15) & ~15)
// #define BLOCK_SIZE sizeof(t_block)

// t_malloc_env g_malloc = {
// 	.tiny = NULL,
// 	.small = NULL,
// 	.large = NULL,
// 	.lock = PTHREAD_MUTEX_INITIALIZER
// };

// void *malloc(size_t size)
// {
// 	void *ptr = NULL;

// 	if (size == 0)
// 		return NULL;

// 	pthread_mutex_lock(&g_malloc.lock);

// 	if (size > (size_t)SMALL_LIMIT)
// 	{
// 		// LARGE allocation
// 		size_t total_size = ALIGN(size) + BLOCK_SIZE + sizeof(t_zone);
// 		t_zone *zone = create_zone(total_size);
// 		if (!zone)
// 		{
// 			pthread_mutex_unlock(&g_malloc.lock);
// 			return NULL;
// 		}

// 		t_block *blk = (t_block *)(zone + 1);
// 		blk->size = ALIGN(size);
// 		blk->is_free = 0;
// 		blk->next = NULL;

// 		zone->blocks = blk;
// 		zone->next = g_malloc.large;
// 		g_malloc.large = zone;

// 		ptr = (void *)(blk + 1);
// 	}
// 	else
// 	{
// 		t_zone **zone_list = (size <= (size_t)TINY_LIMIT) ? &g_malloc.tiny : &g_malloc.small;
// 		size_t zone_size = (size <= (size_t)TINY_LIMIT) ? TINY_ZONE_SIZE : SMALL_ZONE_SIZE;

// 		t_block *blk = find_free_block(*zone_list, size);
// 		if (!blk)
// 		{
// 			t_zone *new_zone = create_zone(zone_size);
// 			if (!new_zone)
// 			{
// 				pthread_mutex_unlock(&g_malloc.lock);
// 				return NULL;
// 			}
// 			fill_zone_with_blocks(new_zone, size);
// 			new_zone->next = *zone_list;
// 			*zone_list = new_zone;
// 			blk = new_zone->blocks;
// 		}

// 		split_block(blk, ALIGN(size));
// 		blk->is_free = 0;
// 		ptr = (void *)(blk + 1);
// 	}

// 	pthread_mutex_unlock(&g_malloc.lock);
// 	return ptr;
// }
#include "../include/malloc.h"
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#define ALIGN(x) (((x) + 15) & ~15)
#define BLOCK_SIZE sizeof(t_block)

t_malloc_env g_malloc = {
	.tiny = NULL,
	.small = NULL,
	.large = NULL,
	.lock = PTHREAD_MUTEX_INITIALIZER
};

// void *malloc(size_t size) {
//     void *ptr = NULL;

//     if (size == 0) {
//         return ptr;
// 	}
// 	size_t total_size = ALIGN(size);
// 	ft_putnbr(total_size);
// 	pthread_mutex_lock(&g_malloc.lock);

//     if (size < (size_t)TINY_LIMIT) {
// 		write(2, "TINY\n", ft_strlen("TINY\n"));
//         t_zone **zone_list = &g_malloc.tiny;
// 		return zone_list;
//     } else if (size <= (size_t)SMALL_LIMIT) {
// 		write(2, "SMALL\n", ft_strlen("SMALL\n"));
//         t_zone **zone_list = &g_malloc.small;
// 		t_block *blk = NULL;
		
// 		if (*zone_list) {
//         	blk = find_free_block(*zone_list, total_size);
// 			ft_putstr("Zone found\n");
// 		} else {
// 			ft_putstr("Need new zone\n");
// 			t_zone *new_zone = create_zone(SMALL_ZONE_SIZE);

// 			if (!new_zone) {
// 				pthread_mutex_unlock(&g_malloc.lock);
// 				return NULL;
// 			}
// 			fill_zone_with_blocks(new_zone, total_size);
// 			new_zone->next = *zone_list;
// 			*zone_list = new_zone;
// 			blk = new_zone->blocks;
// 		}

// 		blk->is_free = 0;
// 		ptr = (void *)(blk + 1);
// 		pthread_mutex_unlock(&g_malloc.lock);
// 		return blk;
//     } else {
// 		write(2, "LARGE\n", ft_strlen("LARGE\n"));
//         t_zone **zone_list = &g_malloc.large;
		

// 		return zone_list;

//     }
// 	pthread_mutex_lock(&g_malloc.lock);
// }
void init_zone(t_zone *zone, size_t zone_size, size_t requested_size)
{
	zone->size = zone_size;
	zone->next = NULL;
	zone->blocks = NULL;

	size_t aligned_size = ALIGN(requested_size);
	size_t blk_size = aligned_size + BLOCK_SIZE;
	size_t usable = zone_size - sizeof(t_zone);
	size_t count = usable / blk_size;

	if (count == 0)
		return;

	t_block *head = (t_block *)(zone + 1);
	t_block *curr = head;
	zone->blocks = head;

	for (size_t i = 0; i < count - 1; ++i)
	{
		curr->size = aligned_size;
		curr->is_free = 1;
		curr->next = (t_block *)((char *)curr + blk_size);
		curr = curr->next;
	}

	curr->size = aligned_size;
	curr->is_free = 1;
	curr->next = NULL;
}

void *malloc(size_t size)
{
	void *ptr = NULL;

	if (size == 0)
		return NULL;

	pthread_mutex_lock(&g_malloc.lock);

	if (size > (size_t)SMALL_LIMIT)
	{
		// LARGE allocation
		size_t total_size = ALIGN(size) + BLOCK_SIZE + sizeof(t_zone);
		t_zone *zone = create_zone(total_size);
		if (!zone)
		{
			pthread_mutex_unlock(&g_malloc.lock);
			return NULL;
		}

		t_block *blk = (t_block *)(zone + 1);
		blk->size = ALIGN(size);
		blk->is_free = 0;
		blk->next = NULL;

		zone->blocks = blk;
		zone->next = g_malloc.large;
		g_malloc.large = zone;

		ptr = (void *)(blk + 1);
	}
	else
	{
		t_zone **zone_list = (size <= (size_t)TINY_LIMIT) ? &g_malloc.tiny : &g_malloc.small;
		size_t zone_size = (size <= (size_t)TINY_LIMIT) ? TINY_ZONE_SIZE : SMALL_ZONE_SIZE;

		t_block *blk = find_free_block(*zone_list, size);
		if (!blk)
		{
			t_zone *new_zone = create_zone(zone_size);
			if (!new_zone)
			{
				pthread_mutex_unlock(&g_malloc.lock);
				return NULL;
			}
			init_zone(new_zone, zone_size, size);
			new_zone->next = *zone_list;
			*zone_list = new_zone;
			blk = new_zone->blocks;
		}

		split_block(blk, ALIGN(size));
		blk->is_free = 0;
		ptr = (void *)(blk + 1);
	}

	pthread_mutex_unlock(&g_malloc.lock);
	return ptr;
}
