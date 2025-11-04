#ifndef MALLOC_H
#define MALLOC_H

# include <stddef.h>
# include <unistd.h>
# include <sys/mman.h>
# include <pthread.h>

# define ALIGNMENT 16

# define TINY_LIMIT 128
# define SMALL_LIMIT 2048
# define TINY_ZONE_SIZE (getpagesize() * 8)  // page = 4096 bytes; (4096 - 24) * 8 = 32576; 32576/128 = 254
# define SMALL_ZONE_SIZE (getpagesize() * 64) // page = 4096 bytes; (4096 - 24) * 64 = 260608; 260608/2048 = 127

enum e_zone_type {
	TINY_ZONE,
	SMALL_ZONE,
	LARGE_ZONE
};

typedef struct s_block
{
	size_t size;
	int is_free;
	char padding[8];
	struct s_block *next;
} t_block;

typedef struct s_zone {
	struct s_zone *next;
	t_block       *blocks;
	size_t         size;  // taille totale de la zone
	void		  *start;
} t_zone;

typedef struct s_malloc
{
	t_zone *tiny;
	t_zone *small;
	t_zone *large;
	pthread_mutex_t lock;
} t_malloc;

extern t_malloc g_malloc;

// Fonctions utilitaires
size_t align_size(size_t size);
int	is_managed_pointer(void *ptr);

// Fonctions de gestion des zones
t_zone **get_zone_list(size_t size);
size_t get_zone_size(size_t size);
t_zone *create_zone(size_t size);
void init_zone(t_zone *zone, size_t size);

// Fonctions de gestion des blocs
t_block *find_free_block(t_zone *zone, size_t size);
void split_block(t_block *blk, size_t size);
t_block *find_block(t_zone **zone, void *ptr);

// Fonctions principales d'allocation
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);

// Fonction de débogage
void show_alloc_mem(void);
void print_zones_state(void); 

void	*ft_memcpy(void *dst, const void *src, size_t n);
int		ft_strlen(char *str);
void	ft_putchar(char c);
void	ft_putnbr(int nb);
void	ft_putstr(char *s);


#endif