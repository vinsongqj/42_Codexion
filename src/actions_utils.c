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

static int	handle_single_coder(t_coder *coder, int f)
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

static int	can_grab(t_coder *coder, int d_id, t_heap *heap)
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

static int	grab_dongle(t_coder *coder, int d_id)
{
	t_heap	*heap;
	int		i;
	int		res;

	heap = &coder->table->dongle_queues[d_id];
	heap_push(heap, coder->id, get_priority(coder));
	i = 0;
	while (1)
	{
		res = can_grab(coder, d_id, heap);
		if (res == -1)
		{
			heap_remove_coder(heap, coder->id);
			return (0);
		}
		if (res == 1)
			return (1);
		if (++i >= 10)
		{
			heap_remove_coder(heap, coder->id);
			heap_push(heap, coder->id, get_priority(coder));
			i = 0;
		}
		usleep(50);
	}
}

int	try_grab_dongle(t_coder *coder, int f, int s)
{
	t_heap	*first_heap;

	if (!grab_dongle(coder, f))
		return (0);
	log_action(coder, "has taken a dongle");
	if (coder->table->number_of_coders == 1)
		return (handle_single_coder(coder, f));
	if (!grab_dongle(coder, s))
	{
		first_heap = &coder->table->dongle_queues[f];
		heap_remove_coder(first_heap, coder->id);
		pthread_mutex_unlock(&coder->table->dongle_locks[f]);
		return (0);
	}
	log_action(coder, "has taken a dongle");
	return (1);
}
