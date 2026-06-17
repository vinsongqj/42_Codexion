/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgoh <vgoh@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 14:41:00 by vgoh              #+#    #+#             */
/*   Updated: 2026/06/16 17:55:26 by vgoh             ###   ########.fr       */
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
	int	l_act;
	int	all_done;

	l_act = (t->number_of_compiles_required != 0);
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
