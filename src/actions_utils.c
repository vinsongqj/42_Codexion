/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgoh <vgoh@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 18:05:15 by vgoh              #+#    #+#             */
/*   Updated: 2026/06/18 18:05:20 by vgoh             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long long	get_priority(t_coder *coder)
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

int	can_grab(t_coder *coder, int d_id, t_heap *heap)
{
	long long	last;
	long long	now;

	pthread_mutex_lock(&coder->table->stop_lock);
	if (coder->table->stop_sim)
	{
		pthread_mutex_unlock(&coder->table->stop_lock);
		return (-1);
	}
	last = coder->table->dongle_last[d_id];
	pthread_mutex_unlock(&coder->table->stop_lock);
	now = get_time_in_ms();
	if (now - last < coder->table->dongle_cooldown)
		return (usleep(500), 0);
	if (is_coder_next(heap, coder->id))
	{
		pthread_mutex_lock(&coder->table->dongle_locks[d_id]);
		heap_pop(heap);
		return (1);
	}
	return (0);
}

static void	repush_coder(t_heap *heap, int coder_id, long long priority)
{
	heap_remove_coder(heap, coder_id);
	heap_push(heap, coder_id, priority);
}

int	grab_dongle(t_coder *coder, int d_id)
{
	t_heap		*heap;
	long long	priority;
	int			i;
	int			res;

	heap = &coder->table->dongle_queues[d_id];
	priority = get_priority(coder);
	heap_push(heap, coder->id, priority);
	i = 0;
	while (1)
	{
		res = can_grab(coder, d_id, heap);
		if (res == -1)
			return (heap_remove_coder(heap, coder->id), 0);
		if (res == 1)
			return (1);
		if (++i >= 10)
			(repush_coder(heap, coder->id, priority), i = 0);
		usleep(50);
	}
}
