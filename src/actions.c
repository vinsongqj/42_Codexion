/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgoh <vgoh@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 18:04:53 by vgoh              #+#    #+#             */
/*   Updated: 2026/06/18 18:04:58 by vgoh             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	execute_compile(t_coder *coder, int f, int s)
{
	pthread_mutex_lock(&coder->table->stop_lock);
	coder->last_compile = get_time_in_ms();
	pthread_mutex_unlock(&coder->table->stop_lock);
	log_action(coder, "is compiling");
	ft_usleep(coder->table->time_to_compile, coder->table);
	pthread_mutex_lock(&coder->table->stop_lock);
	coder->compiles_done++;
	coder->table->dongle_last[f] = get_time_in_ms();
	coder->table->dongle_last[s] = get_time_in_ms();
	pthread_mutex_unlock(&coder->table->stop_lock);
	pthread_mutex_unlock(&coder->table->dongle_locks[f]);
	pthread_mutex_unlock(&coder->table->dongle_locks[s]);
}

int	compile_action(t_coder *coder)
{
	int	f;
	int	s;

	f = coder->left_dongle;
	s = coder->right_dongle;
	if (coder->id % 2 == 0)
	{
		f = coder->right_dongle;
		s = coder->left_dongle;
	}
	if (!try_grab_dongle(coder, f, s))
		return (0);
	execute_compile(coder, f, s);
	return (1);
}
