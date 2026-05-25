/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgoh <vgoh@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 14:41:37 by vgoh              #+#    #+#             */
/*   Updated: 2026/05/26 03:27:54 by vgoh             ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>

typedef struct s_table	t_table;

typedef struct s_node
{
	int			coder_id;
	long long	priority;
}	t_node;

typedef struct s_heap
{
	t_node			*nodes;
	int				size;
	pthread_mutex_t	lock;
}	t_heap;

typedef struct s_coder
{
	int			id;
	long long	last_compile;
	int			compiles_done;
	int			left_dongle;
	int			right_dongle;
	t_table		*table;
	pthread_t	thread_id;
}	t_coder;

typedef struct s_table
{
	int				number_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	int				dongle_cooldown;
	int				scheduler;
	int				stop_sim;
	int				all_finished;
	long long		start_time;
	long long		*dongle_last;
	t_heap			*dongle_queues;
	t_coder			*coders;
	pthread_mutex_t	stop_lock;
	pthread_mutex_t	write_lock;
	pthread_mutex_t	*dongle_locks;
}	t_table;

// parser.c //
int			parse_args(t_table *table, int argc, char **argv);

// threads.c //
void		*coder_routine(void *arg);
void		*monitor_routine(void *arg);

// actions.c //
int			compile_action(t_coder *coder);

// scheduler.c //
void		heap_push(t_heap *heap, int coder_id, long long priority);
void		heap_pop(t_heap *heap);
int			is_coder_next(t_heap *heap, int coder_id);
void		heap_remove_coder(t_heap *heap, int coder_id);

// init.c //
int			init_table(t_table *table, int argc, char **argv);

// utils.c //
long long	ft_atoll(const char *str);
long long	get_time_in_ms(void);
void		ft_usleep(long long time_in_ms, t_table *table);
void		cleanup_table(t_table *table);
int			is_higher(t_node a, t_node b);

// threads_utils.c //
void		log_action(t_coder *coder, char *message);
long long	get_priority(t_coder *coder);
int			handle_single_coder(t_coder *coder, int f);
int			should_stop_sim(t_table *t);
int			handle_all_done_check(t_table *t, int f_count);

#endif
