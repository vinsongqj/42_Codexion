/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgoh <vgoh@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 18:08:12 by vgoh              #+#    #+#             */
/*   Updated: 2026/06/18 18:08:15 by vgoh             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

static int	check_burnout(t_table *t, int i)
{
	int			done;
	long long	time_diff;
	int			l_act;
	int			m_reach;

	pthread_mutex_lock(&t->stop_lock);
	l_act = (t->number_of_compiles_required != 0);
	m_reach = (t->coders[i].compiles_done >= t->number_of_compiles_required);
	done = (l_act && m_reach);
	time_diff = get_time_in_ms() - t->coders[i].last_compile;
	if (!done && (time_diff >= t->time_to_burnout))
	{
		if (t->stop_sim)
		{
			pthread_mutex_unlock(&t->stop_lock);
			return (1);
		}
		return (handle_burnout(t, i));
	}
	pthread_mutex_unlock(&t->stop_lock);
	return (done);
}

static int	check_limit_reached(t_coder *c, t_table *t)
{
	int	l_act;
	int	m_reach;

	pthread_mutex_lock(&t->stop_lock);
	l_act = (t->number_of_compiles_required != 0);
	m_reach = (c->compiles_done >= t->number_of_compiles_required);
	if (l_act && m_reach)
	{
		pthread_mutex_unlock(&t->stop_lock);
		return (1);
	}
	pthread_mutex_unlock(&t->stop_lock);
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_table	*t;
	int		i;
	int		f_count;

	t = (t_table *)arg;
	while (1)
	{
		if (should_stop_sim(t))
			return (NULL);
		i = -1;
		f_count = 0;
		while (++i < t->number_of_coders)
		{
			if (check_burnout(t, i))
				f_count++;
		}
		if (should_stop_sim(t))
			return (NULL);
		if (handle_all_done_check(t, f_count))
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
