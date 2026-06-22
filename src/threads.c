/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgoh <vgoh@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 18:08:12 by vgoh              #+#    #+#             */
/*   Updated: 2026/06/19 02:14:19 by vgoh             ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "codexion.h"

static int	handle_burnout(t_table *t, int i)
{
	long long	ts;
	int			c_id;

	t->stop_sim = 1;
	ts = get_time_in_ms() - t->start_time;
	c_id = t->coders[i].id;
	pthread_mutex_lock(&t->write_lock);
	printf("%lld %d burned out\n", ts, c_id);
	pthread_mutex_unlock(&t->write_lock);
	pthread_mutex_unlock(&t->stop_lock);
	return (1);
}

static int	is_burned_out(t_table *t, int i)
{
	long long	time_diff;

	time_diff = get_time_in_ms() - t->coders[i].last_compile;
	if (time_diff >= t->time_to_burnout)
	{
		if (t->stop_sim)
		{
			pthread_mutex_unlock(&t->stop_lock);
			return (1);
		}
		return (handle_burnout(t, i));
	}
	pthread_mutex_unlock(&t->stop_lock);
	return (0);
}

static int	check_burnout(t_table *t, int i)
{
	int	has_limit;
	int	hit_limit;

	pthread_mutex_lock(&t->stop_lock);
	has_limit = (t->number_of_compiles_required != 0);
	hit_limit = (t->coders[i].compiles_done >= t->number_of_compiles_required);
	if (has_limit && hit_limit)
	{
		pthread_mutex_unlock(&t->stop_lock);
		return (1);
	}
	return (is_burned_out(t, i));
}

void	*monitor_routine(void *arg)
{
	t_table	*t;
	int		i;
	int		coders_done;

	t = (t_table *)arg;
	while (1)
	{
		if (should_stop_sim(t))
			return (NULL);
		i = -1;
		coders_done = 0;
		while (++i < t->number_of_coders)
		{
			if (check_burnout(t, i))
				coders_done++;
		}
		if (should_stop_sim(t))
			return (NULL);
		if (handle_all_done_check(t, coders_done))
			return (NULL);
		usleep(1000);
	}
}

void	*coder_routine(void *arg)
{
	t_coder	*c;
	t_table	*t;

	c = (t_coder *)arg;
	t = c->table;
	wait_for_start(t);
	if (c->id % 2 == 0)
		ft_usleep(t->time_to_compile / 2, t);
	while (1)
	{
		if (!compile_action(c))
			break ;
		log_action(c, "is debugging");
		ft_usleep(t->time_to_debug, t);
		log_action(c, "is refactoring");
		ft_usleep(t->time_to_refactor, t);
		if (check_limit_reached(c, t))
			break ;
	}
	return (NULL);
}
