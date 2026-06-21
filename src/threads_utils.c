/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgoh <vgoh@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 18:08:35 by vgoh              #+#    #+#             */
/*   Updated: 2026/06/19 01:23:20 by vgoh             ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

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
	int	has_limit;
	int	all_done;

	has_limit = (t->number_of_compiles_required != 0);
	all_done = (f_count == t->number_of_coders);
	if (has_limit && all_done)
	{
		pthread_mutex_lock(&t->stop_lock);
		t->all_finished = 1;
		t->stop_sim = 1;
		pthread_mutex_unlock(&t->stop_lock);
		return (1);
	}
	return (0);
}

int	check_limit_reached(t_coder *c, t_table *t)
{
	int	has_limit;
	int	limit_reached;

	pthread_mutex_lock(&t->stop_lock);
	has_limit = (t->number_of_compiles_required != 0);
	limit_reached = (c->compiles_done >= t->number_of_compiles_required);
	if (has_limit && limit_reached)
	{
		pthread_mutex_unlock(&t->stop_lock);
		return (1);
	}
	pthread_mutex_unlock(&t->stop_lock);
	return (0);
}
