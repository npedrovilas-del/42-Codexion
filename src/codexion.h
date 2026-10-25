/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pneto-vi <pneto-vi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 23:30:00 by pneto-vi          #+#    #+#             */
/*   Updated: 2026/10/25 23:30:00 by pneto-vi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <limits.h>

typedef struct s_hub	t_hub;
typedef struct s_dongle	t_dongle;
typedef struct s_coder	t_coder;

typedef struct s_request
{
	int				id_code;
	long int		deadline;
	unsigned int	seq;
}	t_request;

typedef struct s_heap
{
	t_request	*requests;
	int			size;
	int			max_capacity;
	int			mode;
}	t_heap;

typedef struct s_dongle
{
	int				dongle_id;
	int				holder;
	long int		cooldown;
	long int		next_request;
	pthread_mutex_t	mutex;
	pthread_cond_t	condition;
	t_heap			wait;
}	t_dongle;

typedef struct s_coder
{
	int			coder_id;
	int			compiles;
	long int	last_compile;
	pthread_t	thread;
	t_dongle	*first;
	t_dongle	*second;
	t_hub		*hub;
}	t_coder;

typedef struct s_hub
{
	int				number_coder;
	long int		time_burnout;
	long int		time_compile;
	long int		time_debug;
	long int		time_refactor;
	int				compiles_required;
	int				dongle_cooldown;
	int				scheduler;
	long int		stopped;
	long int		start;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	mutex;
	pthread_mutex_t	mutex_state;
	pthread_t		monitor;
	t_dongle		*dongles;
	t_coder			*coders;
}	t_hub;

int			parse_args(t_hub *hub, int argc, char **argv);
long int	time_now(void);
long int	time_since_start(t_hub *hub);
void		precise_sleep(long int ms, t_hub *hub);
int			mutex_verification(t_hub *hub);
void		mutex_remove(t_hub *hub);
int			hub_alloc(t_hub *hub);
void		free_hub(t_hub *hub);
int			init_dongles(t_hub *hub);
void		dongles_remove(t_hub *hub);
void		init_coders(t_hub *hub);
int			search_better(t_heap *heap, t_request *a, t_request *b);
void		heap_swap(t_request *a, t_request *b);
void		heap_sift_up(t_heap *h, int i);
void		heap_sift_down(t_heap *h, int i);
t_request	*heap_verification(t_heap *h);
int			heap_push(t_heap *h, t_request req);
int			heap_pop(t_heap *h, t_request *out);
int			heap_remover(t_heap *h, int coder_id);
int			hub_stop(t_hub *hub);
int			dongle_take(t_dongle *d, t_coder *coder);
void		dongle_release(t_dongle *d, t_hub *hub);
void		log_state(t_hub *hub, int coder_id, const char *msg);
void		log_burnout(t_hub *hub, int coder_id);
void		*coder_cycle(void *arg);
void		*monitor(void *arg);

#endif
