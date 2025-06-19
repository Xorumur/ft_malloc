// #include "../include/malloc.h"
// #include <stdio.h>
// #include <stdint.h>

// static void print_zone(const char *label, t_zone *zone, size_t *total)
// {
// 	while (zone)
// 	{
// 		dprintf(2, "%s\n", label);
// 		t_block *block = zone->blocks;
// 		while (block)
// 		{
// 			if (!block->is_free)
// 			{
// 				void *start = (void *)(block + 1);
// 				void *end = (void *)((char *)start + block->size);
// 				dprintf(2, "%p - %p : %zu bytes\n", start, end, block->size);
// 				*total += block->size;
// 			}
// 			block = block->next;
// 		}
// 		zone = zone->next;
// 	}
// }

// void show_alloc_mem(void)
// {
// 	size_t total = 0;

// 	pthread_mutex_lock(&g_malloc.lock);

// 	print_zone("TINY", g_malloc.tiny, &total);
// 	print_zone("SMALL", g_malloc.small, &total);
// 	print_zone("LARGE", g_malloc.large, &total);

// 	dprintf(2, "Total : %zu bytes\n", total);

// 	pthread_mutex_unlock(&g_malloc.lock);
// }

#include "../include/malloc.h"
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static void	log_line(const char *label)
{
	write(2, label, strlen(label));
	write(2, "\n", 1);
}

static void	log_alloc(void *start, void *end, size_t size)
{
	char buffer[256];
	int len = snprintf(buffer, sizeof(buffer),
		"%p - %p : %zu bytes\n", start, end, size);
	write(2, buffer, len);
}

static void	log_total(size_t total)
{
	char buffer[128];
	int len = snprintf(buffer, sizeof(buffer), "Total : %zu bytes\n", total);
	write(2, buffer, len);
}

static void print_zone(const char *label, t_zone *zone, size_t *total)
{
	while (zone)
	{
		log_line(label);
		t_block *block = zone->blocks;
		while (block)
		{
			if (!block->is_free)
			{
				void *start = (void *)(block + 1);
				void *end = (void *)((char *)start + block->size);
				log_alloc(start, end, block->size);
				*total += block->size;
			}
			block = block->next;
		}
		zone = zone->next;
	}
}

void show_alloc_mem(void)
{
	size_t total = 0;

	pthread_mutex_lock(&g_malloc.lock);

	print_zone("TINY", g_malloc.tiny, &total);
	print_zone("SMALL", g_malloc.small, &total);
	print_zone("LARGE", g_malloc.large, &total);

	log_total(total);

	pthread_mutex_unlock(&g_malloc.lock);
}
