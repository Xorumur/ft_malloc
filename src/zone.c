#include "../include/malloc.h"
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define ALIGN(x) (((x) + 15) & ~15)
#define BLOCK_SIZE sizeof(t_block)

void	write_address(void *ptr)
{
	uintptr_t addr = (uintptr_t)ptr;
	char		hex[19] = "0x0000000000000000";
	const char	*base = "0123456789abcdef";

	for (int i = 0; i < 16; i++)
	{
		hex[17 - i] = base[addr & 0xF];
		addr >>= 4;
	}
	write(2, hex, 18);
	write(2, "\n", 1);
}

t_zone *create_zone(size_t size)
{
	void *mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (mem == MAP_FAILED)
		return NULL;

	t_zone *zone = (t_zone *)mem;
	zone->start = mem;
	zone->size = size;
	zone->next = NULL;
	zone->blocks = NULL;
    // dprintf(2, "✅ zone created: %p, blocks head: %p, zone end: %p\n",
    //     (void *)zone, (void *)zone->blocks, (void *)zone + zone->size);

	return zone;
}

void fill_zone_with_blocks(t_zone *zone, size_t requested_size)
{
    size_t aligned_size = requested_size;
    size_t blk_size = aligned_size ;
    size_t usable = zone->size - sizeof(t_zone);
    size_t count = usable / blk_size;

    if (count == 0)
        return;

    char *cursor = (char *)(zone + 1);
    t_block *prev = NULL;

    for (size_t i = 0; i < count; ++i)
    {
        if ((void *)cursor + blk_size > (void *)zone + zone->size)
        {
            write(2, "🛑 OUT OF ZONE\n", 15);
            if (prev)
                prev->next = NULL;
            break;
        }

        t_block *blk = (t_block *)cursor;
        blk->size = aligned_size;
        blk->is_free = 1;
        blk->next = NULL;

        if (prev)
            prev->next = blk;
        else
            zone->blocks = blk;

        prev = blk;
        cursor += blk_size;
    }
}



t_block *find_free_block(t_zone *zone, size_t size)
{
    if (!zone) {
        return NULL;
    }
	size_t aligned_size = size;
	while (zone)
	{
		t_block *blk = zone->blocks;
        if (!blk)
        {
            zone = zone->next;
            continue ;
        }
        write(2, "searching block1...\n", 21);
        write(2, "ZONE:\n", 6);
        write_address((void *)zone);  // Fonction de debug que tu peux écrire

        while (blk)
        {
            write(2, "  BLOCK:\n", 9);
            write_address((void *)blk);
            ft_putnbr(blk->size);  // Pareil, fonction de debug rapide

			if (blk->is_free && blk->size >= aligned_size)
				return blk;
			blk = blk->next;
		}
		zone = zone->next;
	}
	return NULL;
}

void split_block(t_block *block, size_t aligned_size)
{
	if (block->size >= aligned_size + BLOCK_SIZE + 16)
	{
		t_block *new_block = (t_block *)((char *)(block + 1) + aligned_size);
		new_block->size = block->size - aligned_size - BLOCK_SIZE;
		new_block->is_free = 1;
		new_block->next = block->next;

		block->size = aligned_size;
		block->next = new_block;
	}
}
