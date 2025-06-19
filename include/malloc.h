#ifndef MALLOC_H
#define MALLOC_H

# include <stddef.h>
# include <unistd.h>
# include <sys/mman.h>
# include <pthread.h>

#define TINY_ZONE_SIZE (getpagesize() * 32)   // ou plus
#define SMALL_ZONE_SIZE (getpagesize() * 128)
#define TINY_LIMIT 512
#define SMALL_LIMIT 2048


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


void	*malloc(size_t size);
void	free(void *ptr);
void	*realloc(void *ptr, size_t size);
void show_alloc_mem(void) __attribute__((visibility("default")));

// zone helpers
t_zone_type	get_zone_type(size_t size);
size_t		get_zone_size(t_zone_type type);
t_zone		*create_zone(t_zone_type type);
t_block		*find_free_block(t_zone *zone, size_t size);
t_block		*add_block_to_zone(t_zone *zone, size_t size);

extern t_malloc_env g_malloc;

#endif