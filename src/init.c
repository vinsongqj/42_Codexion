/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgoh <vgoh@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 18:05:37 by vgoh              #+#    #+#             */
/*   Updated: 2026/06/18 18:05:41 by vgoh             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	alloc_tables(t_table *table)
{
	size_t	len;

	len = table->number_of_coders;
	table->coders = malloc(sizeof(t_coder) * len);
	if (!table->coders)
		return (1);
	table->dongle_locks = malloc(sizeof(pthread_mutex_t) * len);
	if (!table->dongle_locks)
		return (1);
	table->dongle_last = malloc(sizeof(long long) * len);
	if (!table->dongle_last)
		return (1);
	table->dongle_queues = malloc(sizeof(t_heap) * len);
	if (!table->dongle_queues)
		return (1);
	return (0);
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
		table->coders[i].last_compile = get_time_in_ms();
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
