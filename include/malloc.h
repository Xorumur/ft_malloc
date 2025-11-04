#ifndef MALLOC_H
#define MALLOC_H

# include <stddef.h>
# include <unistd.h>
# include <sys/mman.h>
# include <pthread.h>

// #define TINY_ZONE_SIZE (getpagesize() * 32)   // ou plus
// #define SMALL_ZONE_SIZE (getpagesize() * 128)
#define TINY_ZONE_SIZE (getpagesize() * 16)   // ou même *16
#define SMALL_ZONE_SIZE (getpagesize() * 52)  // ou *32
#define TINY_LIMIT (TINY_ZONE_SIZE / 128)
#define SMALL_LIMIT (SMALL_ZONE_SIZE / 128)


typedef struct s_block
{
	size_t size;
	int is_free;
	struct s_block *next;
} t_block;

typedef struct s_zone {
	struct s_zone *next;
	t_block       *blocks;
	size_t         size;  // taille totale de la zone
	void		  *start;
} t_zone;

typedef enum e_zone_type {
	TINY,
	SMALL,
	LARGE
} t_zone_type;


typedef struct s_malloc_env
{
	t_zone *tiny;
	t_zone *small;
	t_zone *large;
	pthread_mutex_t lock;
} t_malloc_env;


void		*malloc(size_t size);
void		free(void *ptr);
void		*realloc(void *ptr, size_t size);
void 		show_alloc_mem(void) __attribute__((visibility("default")));


t_zone *create_zone(size_t size);
void fill_zone_with_blocks(t_zone *zone, size_t requested_size);
void split_block(t_block *block, size_t aligned_size);
t_block		*find_free_block(t_zone *zone, size_t size);
t_block		*add_block_to_zone(t_zone *zone, size_t size);
void		*ft_memcpy(void *dst, const void *src, size_t n);
int			is_managed_pointer(void *ptr);

int	ft_strlen(char *str);
void	ft_putnbr(int nb);
void	ft_putstr(char *s);

extern t_malloc_env g_malloc;

#endif