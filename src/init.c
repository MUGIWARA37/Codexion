/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhlou <rhlou@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 09:29:49 by rhlou             #+#    #+#             */
/*   Updated: 2026/07/16 11:48:40 by rhlou            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->num_coders)
	{
		pthread_mutex_init(&sim->dongles[i].mutex, NULL);
		pthread_cond_init(&sim->dongles[i].cond, NULL);
		sim->dongles[i].is_available = 1;
		sim->dongles[i].released_at = 0;
		if (heap_init(&sim->dongles[i].wait_queue, sim->num_coders) == -1)
			return (-1);
		i++;
	}
	return (0);
}

static void	fill_args(t_sim *sim, char **argv)
{
	sim->num_coders = ft_atoi(argv[1]);
	sim->time_to_burnout = ft_atoi(argv[2]);
	sim->time_to_compile = ft_atoi(argv[3]);
	sim->time_to_debug = ft_atoi(argv[4]);
	sim->time_to_refactor = ft_atoi(argv[5]);
	sim->num_compiles_required = ft_atoi(argv[6]);
	sim->dongle_cooldown = ft_atoi(argv[7]);
	sim->use_edf = ft_strcmp(argv[8], "edf") == 0;
}

static int	parce_args(t_sim *sim, int argc, char **argv)
{
	int	i;

	if (!argv || !*argv || argc != 9)
		return (-1);
	i = 1;
	while (i < 8)
	{
		if (!is_valid_number(argv[i]))
			return (-1);
		i++;
	}
	if (ft_strcmp(argv[8], "edf") != 0 && ft_strcmp(argv[8], "fifo") != 0)
	{
		fprintf(stderr, "Error: scheduler must be 'fifo' or 'edf'\n");
		return (-1);
	}
	fill_args(sim, argv);
	if (sim->num_coders <= 0 || sim->time_to_burnout < 0
		|| sim->time_to_compile < 0 || sim->time_to_debug < 0
		|| sim->time_to_refactor < 0 || sim->num_compiles_required < 0
		|| sim->dongle_cooldown < 0)
		return (-1);
	return (1);
}

static int	init_sim_util(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->num_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].compile_count = 0;
		sim->coders[i].finished = 0;
		pthread_mutex_init(&sim->coders[i].coder_mutex, NULL);
		sim->coders[i].last_compile_start = get_time_ms();
		sim->coders[i].sim = sim;
		i++;
	}
	if (init_dongles(sim) == -1)
		return (-1);
	pthread_mutex_init(&sim->log_mutex, NULL);
	pthread_mutex_init(&sim->stop_mutex, NULL);
	pthread_mutex_init(&sim->fifo_mutex, NULL);
	sim->simulation_over = 0;
	sim->fifo_counter = 0;
	sim->start_time = get_time_ms();
	return (0);
}

int	init_sim(t_sim *sim, int argc, char **argv)
{
	if (parce_args(sim, argc, argv) == -1)
		return (-1);
	sim->coders = malloc(sim->num_coders * sizeof(t_coder));
	if (!sim->coders)
		return (-1);
	sim->dongles = malloc(sim->num_coders * sizeof(t_dongle));
	if (!sim->dongles)
	{
		free(sim->coders);
		return (-1);
	}
	if (init_sim_util(sim) == -1)
		return (-1);
	return (0);
}
