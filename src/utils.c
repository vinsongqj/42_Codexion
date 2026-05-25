/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgoh <vgoh@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 14:41:07 by vgoh              #+#    #+#             */
/*   Updated: 2026/05/25 14:41:14 by vgoh             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	ft_atoll(const char *str)
{
	long long	res;
	int			sign;
	int			i;

	res = 0;
	sign = 1;
	i = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
		res = (res * 10) + (str[i++] - '0');
	return (res * sign);
}

long long	get_time_in_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (0);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}

void	ft_usleep(long long time_in_ms, t_table *table)
{
	long long	start;

	start = get_time_in_ms();
	while ((get_time_in_ms() - start) < time_in_ms)
	{
		pthread_mutex_lock(&table->stop_lock);
		if (table->stop_sim)
		{
			pthread_mutex_unlock(&table->stop_lock);
			break ;
		}
		pthread_mutex_unlock(&table->stop_lock);
		usleep(500);
	}
}

void	cleanup_table(t_table *t)
{
	int	i;

	i = -1;
	if (t->dongle_queues)
	{
		while (++i < t->number_of_coders)
		{
			if (t->dongle_queues[i].nodes)
				free(t->dongle_queues[i].nodes);
			pthread_mutex_destroy(&t->dongle_queues[i].lock);
			pthread_mutex_destroy(&t->dongle_locks[i]);
		}
		free(t->dongle_queues);
	}
	pthread_mutex_destroy(&t->write_lock);
	pthread_mutex_destroy(&t->stop_lock);
	free(t->dongle_locks);
	free(t->coders);
	free(t->dongle_last);
}

int	is_higher(t_node a, t_node b)
{
	if (a.priority != b.priority)
		return (a.priority < b.priority);
	return (a.coder_id < b.coder_id);
}
