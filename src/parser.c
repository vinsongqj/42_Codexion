/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgoh <vgoh@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 14:41:00 by vgoh              #+#    #+#             */
/*   Updated: 2026/06/16 17:43:32 by vgoh             ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "codexion.h"

static int	ft_is_valid_str(const char *str)
{
	int	i;

	i = 0;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int	ft_is_valid_int(const char *str, int *out)
{
	long long	val;

	if (!ft_is_valid_str(str))
		return (0);
	val = ft_atoll(str);
	if (val > 2147483647)
		return (0);
	*out = (int)val;
	return (1);
}

static int	parse_values(t_table *t, char **argv)
{
	int	*targets[7];
	int	i;

	targets[0] = &t->number_of_coders;
	targets[1] = &t->time_to_burnout;
	targets[2] = &t->time_to_compile;
	targets[3] = &t->time_to_debug;
	targets[4] = &t->time_to_refactor;
	targets[5] = &t->number_of_compiles_required;
	targets[6] = &t->dongle_cooldown;
	i = 0;
	while (i < 7)
	{
		if (!ft_is_valid_int(argv[i + 1], targets[i]))
			return (fprintf(stderr, "Error: Invalid argument %d\n", i + 1), 1);
		i++;
	}
	return (0);
}

int	parse_args(t_table *table, int argc, char **argv)
{
	if (argc != 9)
		return (fprintf(stderr, "Program must have 8 arguments\n"), 1);
	if (parse_values(table, argv))
		return (1);
	if (table->number_of_coders <= 0 || table->time_to_burnout <= 0
		|| table->time_to_compile <= 0 || table->time_to_debug <= 0
		|| table->time_to_refactor <= 0
		|| table->number_of_compiles_required < 0
		|| table->dongle_cooldown < 0)
		return (fprintf(stderr, "Error: Invalid parameter values\n"), 1);
	if (strcmp(argv[8], "edf") == 0)
		table->scheduler = 1;
	else if (strcmp(argv[8], "fifo") == 0)
		table->scheduler = 0;
	else
		return (fprintf(stderr, "Error: 8th arg must be 'fifo' or 'edf'\n"), 1);
	return (0);
}
