/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgoh <vgoh@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 14:41:00 by vgoh              #+#    #+#             */
/*   Updated: 2026/05/25 14:41:01 by vgoh             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_action(t_coder *coder, char *message)
{
	long long	ts;

	ts = get_time_in_ms() - coder->table->start_time;
	pthread_mutex_lock(&coder->table->stop_lock);
	pthread_mutex_lock(&coder->table->write_lock);
	if (!coder->table->stop_sim)
		printf("%lld %d %s\n", ts, coder->id, message);
	pthread_mutex_unlock(&coder->table->write_lock);
	pthread_mutex_unlock(&coder->table->stop_lock);
}

long long	get_priority(t_coder *coder)
{
	long long	priority;

	if (coder->table->scheduler == 0)
		return (get_time_in_ms());
	pthread_mutex_lock(&coder->table->stop_lock);
	priority = coder->last_compile + coder->table->time_to_burnout;
	pthread_mutex_unlock(&coder->table->stop_lock);
	return (priority);
}

int	handle_single_coder(t_coder *coder, int f)
{
	while (1)
	{
		pthread_mutex_lock(&coder->table->stop_lock);
		if (coder->table->stop_sim)
		{
			pthread_mutex_unlock(&coder->table->stop_lock);
			break ;
		}
		pthread_mutex_unlock(&coder->table->stop_lock);
		usleep(500);
	}
	pthread_mutex_unlock(&coder->table->dongle_locks[f]);
	heap_pop(&coder->table->dongle_queues[f]);
	return (0);
}

int	should_stop_sim(t_table *t)
{
	pthread_mutex_lock(&t->stop_lock);
	if (t->stop_sim)
	{
		pthread_mutex_unlock(&t->stop_lock);
		return (1);
	}
	pthread_mutex_unlock(&t->stop_lock);
	return (0);
}

int	handle_all_done_check(t_table *t, int f_count)
{
	int	l_act;
	int	all_done;

	l_act = (t->number_of_compiles_required != -1);
	all_done = (f_count == t->number_of_coders);
	if (l_act && all_done)
	{
		pthread_mutex_lock(&t->stop_lock);
		t->all_finished = 1;
		t->stop_sim = 1;
		pthread_mutex_unlock(&t->stop_lock);
		return (1);
	}
	return (0);
}
