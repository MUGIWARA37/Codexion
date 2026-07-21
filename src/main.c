/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhlou <rhlou@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/10 14:01:34 by rhlou             #+#    #+#             */
/*   Updated: 2026/07/21 10:25:37 by rhlou            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	clean_up(t_sim *sim)
{
	int	j;

	j = 0;
	while (j < sim->num_coders)
	{
		pthread_mutex_destroy(&sim->coders[j].coder_mutex);
		pthread_mutex_destroy(&sim->dongles[j].mutex);
		pthread_cond_destroy(&sim->dongles[j].cond);
		free(sim->dongles[j].wait_queue.data);
		j++;
	}
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->start_mutex);
	pthread_mutex_destroy(&sim->fifo_mutex);
	free(sim->coders);
	free(sim->dongles);
}

static void	join_threads(t_sim *sim, int created_coders)
{
	int	i;

	i = 0;
	while (i < created_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	pthread_mutex_lock(&sim->stop_mutex);
	sim->simulation_over = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	pthread_join(sim->monitor_thread, NULL);
	clean_up(sim);
}

static int	handle_thread_error(t_sim *sim, int i)
{
	fprintf(stderr, "Error: Failed to create coder thread\n");
	pthread_mutex_lock(&sim->stop_mutex);
	sim->simulation_over = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	pthread_mutex_unlock(&sim->start_mutex);
	join_threads(sim, i);
	return (1);
}

static int	run_routins(t_sim *sim)
{
	int	i;

	pthread_mutex_lock(&sim->start_mutex);
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim) != 0)
	{
		fprintf(stderr, "Error: Failed to create monitor thread\n");
		pthread_mutex_unlock(&sim->start_mutex);
		clean_up(sim);
		return (1);
	}
	i = -1;
	while (++i < sim->num_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine,
				&sim->coders[i]) != 0)
			return (handle_thread_error(sim, i));
	}
	sim->start_time = get_time_ms();
	pthread_mutex_unlock(&sim->start_mutex);
	join_threads(sim, i);
	return (0);
}

int	main(int argc, char **argv)
{
	t_sim	sim;

	if (argc != 9)
	{
		fprintf(stderr,
			"Usage: ./codexion num_coders time_to_burnout time_to_compile ");
		fprintf(stderr,
			"time_to_debug time_to_refactor num_compiles dongle_cooldown ");
		fprintf(stderr, "scheduler(fifo|edf)\n");
		return (1);
	}
	if (init_sim(&sim, argc, argv) == -1)
	{
		fprintf(stderr, "Error: invalid arguments\n");
		return (1);
	}
	if (run_routins(&sim) == 1)
		return (1);
	return (0);
}
