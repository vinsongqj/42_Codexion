/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgoh <vgoh@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 18:06:37 by vgoh              #+#    #+#             */
/*   Updated: 2026/06/18 18:06:41 by vgoh             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	start_simulation(t_table *t)
{
	pthread_mutex_lock(&t->stop_lock);
	t->start_time = get_time_in_ms();
	pthread_mutex_unlock(&t->stop_lock);
}

static void	join_and_cleanup(t_table *table, pthread_t monitor)
{
	int	i;

	pthread_join(monitor, NULL);
	i = -1;
	while (++i < table->number_of_coders)
		pthread_join(table->coders[i].thread_id, NULL);
	cleanup_table(table);
}

int	main(int argc, char **argv)
{
	t_table		table;
	pthread_t	monitor;
	t_coder		*coder;
	int			i;

	if (init_table(&table, argc, argv))
		return (1);
	i = -1;
	while (++i < table.number_of_coders)
	{
		coder = &table.coders[i];
		if (pthread_create(&coder->thread_id, NULL, &coder_routine, coder))
		{
			cleanup_table(&table);
			return (1);
		}
	}
	if (pthread_create(&monitor, NULL, &monitor_routine, &table))
	{
		cleanup_table(&table);
		return (1);
	}
	start_simulation(&table);
	join_and_cleanup(&table, monitor);
	return (0);
}
