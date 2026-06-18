/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgoh <vgoh@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 18:05:37 by vgoh              #+#    #+#             */
/*   Updated: 2026/06/19 01:03:53 by vgoh             ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "codexion.h"

static void	free_allocs(t_table *table)
{
	free(table->coders);
	free(table->dongle_locks);
	free(table->dongle_last);
	free(table->dongle_queues);
	table->coders = NULL;
	table->dongle_locks = NULL;
	table->dongle_last = NULL;
	table->dongle_queues = NULL;
}

static int	alloc_tables(t_table *table)
{
	size_t	len;

	len = table->number_of_coders;
	table->coders = malloc(sizeof(t_coder) * len);
	table->dongle_locks = malloc(sizeof(pthread_mutex_t) * len);
	table->dongle_last = malloc(sizeof(long long) * len);
	table->dongle_queues = malloc(sizeof(t_heap) * len);
	if (!table->coders || !table->dongle_locks || !table->dongle_last
		|| !table->dongle_queues)
		return (free_allocs(table), 1);
	return (0);
}

static long long	get_init_compile_time(t_table *table, int i)
{
	if ((i + 1) % 2 == 0)
		return (get_time_in_ms() + (table->time_to_compile / 2));
	return (get_time_in_ms());
}

static int	init_coders(t_table *table)
{
	int		i;
	size_t	len;

	i = -1;
	len = table->number_of_coders + 1;
	while (++i < table->number_of_coders)
	{
		table->coders[i].id = i + 1;
		table->coders[i].last_compile = get_init_compile_time(table, i);
		table->coders[i].compiles_done = 0;
		table->coders[i].left_dongle = i;
		table->coders[i].right_dongle = (i + 1) % table->number_of_coders;
		table->coders[i].table = table;
		table->dongle_last[i] = get_time_in_ms();
		pthread_mutex_init(&table->dongle_locks[i], NULL);
		table->dongle_queues[i].nodes = malloc(sizeof(t_node) * len);
		if (!table->dongle_queues[i].nodes)
			return (1);
		table->dongle_queues[i].size = 0;
		pthread_mutex_init(&table->dongle_queues[i].lock, NULL);
	}
	return (0);
}

int	init_table(t_table *table, int argc, char **argv)
{
	table->stop_sim = 0;
	table->all_finished = 0;
	if (parse_args(table, argc, argv))
		return (1);
	if (alloc_tables(table))
		return (1);
	if (init_coders(table))
		return (1);
	pthread_mutex_init(&table->stop_lock, NULL);
	pthread_mutex_init(&table->write_lock, NULL);
	table->start_time = get_time_in_ms();
	return (0);
}
