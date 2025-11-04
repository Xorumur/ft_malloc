#include "../include/malloc.h"
#include <unistd.h>
#include <stdint.h>
#include <string.h>



// Convertit un nombre en base 10 vers une chaîne de caractères (terminée par '\0')
static int utoa_dec(char *buf, size_t n)
{
	int i = 0;
	char tmp[20];

	if (n == 0)
	{
		buf[0] = '0';
		buf[1] = '\0';
		return 1;
	}
	while (n > 0)
	{
		tmp[i++] = '0' + (n % 10);
		n /= 10;
	}
	for (int j = 0; j < i; j++)
		buf[j] = tmp[i - j - 1];
	buf[i] = '\0';
	return i;
}

// Convertit un pointeur en chaîne hexadécimale (ex : 0x7ffe1234)
static int ptr_to_hex(char *buf, void *ptr)
{
	uintptr_t val = (uintptr_t)ptr;
	const char *hex = "0123456789abcdef";
	int i = 0;

	buf[i++] = '0';
	buf[i++] = 'x';

	for (int shift = (sizeof(uintptr_t) * 8) - 4; shift >= 0; shift -= 4)
		buf[i++] = hex[(val >> shift) & 0xF];

	buf[i] = '\0';
	return i;
}

static void write_line(char *s)
{
	write(2, s, ft_strlen(s));
	write(2, "\n", 1);
}

static void write_alloc(void *start, void *end, size_t size)
{
	char buf[256];
	int len = 0;

	len += ptr_to_hex(buf + len, start);
	buf[len++] = ' ';
	buf[len++] = '-';
	buf[len++] = ' ';
	len += ptr_to_hex(buf + len, end);
	buf[len++] = ' ';
	buf[len++] = ':';
	buf[len++] = ' ';

	char size_buf[32];
	int size_len = utoa_dec(size_buf, size);
	ft_memcpy(buf + len, size_buf, size_len);
	len += size_len;

	ft_memcpy(buf + len, " bytes\n", 7);
	len += 7;

	write(2, buf, len);
}

static void print_zone(char *label, t_zone *zone, size_t *total)
{
	while (zone)
	{
		write_line(label);
		t_block *block = zone->blocks;
		while (block)
		{
			if (!block->is_free)
			{
				void *start = (void *)(block + 1);
				void *end = (void *)((char *)start + block->size);
				write_alloc(start, end, block->size);
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

	char buf[64] = "Total : ";
	int len = ft_strlen(buf);
	len += utoa_dec(buf + len, total);
	ft_memcpy(buf + len, " bytes\n", 7);
	len += 7;
	write(2, buf, len);

	pthread_mutex_unlock(&g_malloc.lock);
}
