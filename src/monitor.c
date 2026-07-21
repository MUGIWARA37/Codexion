/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhlou <rhlou@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/10 13:50:48 by rhlou             #+#    #+#             */
/*   Updated: 2026/07/10 17:11:54 by rhlou            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	broadcast_all_dongles(t_sim *sim)
{
	int	j;

	j = 0;
	while (j < sim->num_coders)
	{
		pthread_mutex_lock(&sim->dongles[j].mutex);
		pthread_cond_broadcast(&sim->dongles[j].cond);
		pthread_mutex_unlock(&sim->dongles[j].mutex);
		j++;
	}
}

static void	stop_simulation(t_sim *sim)
{
	pthread_mutex_lock(&sim->stop_mutex);
	sim->simulation_over = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	broadcast_all_dongles(sim);
}

static void	handle_burnout(t_sim *sim, int coder_id)
{
	log_event(sim, coder_id, "burned out");
	stop_simulation(sim);
}

static int	check_all_coders(t_sim *sim)
{
	int	i;
	int	all_finished;

	i = 0;
	all_finished = 1;
	while (i < sim->num_coders)
	{
		pthread_mutex_lock(&sim->coders[i].coder_mutex);
		if (get_time_ms() - sim->coders[i].last_compile_start
			> sim->time_to_burnout)
		{
			pthread_mutex_unlock(&sim->coders[i].coder_mutex);
			handle_burnout(sim, sim->coders[i].id);
			return (-1);
		}
		if (sim->coders[i].compile_count < sim->num_compiles_required)
			all_finished = 0;
		pthread_mutex_unlock(&sim->coders[i].coder_mutex);
		i++;
	}
	return (all_finished);
}

void	*monitor_routine(void *arg)
{
	int		status;
	t_sim	*sim;

	sim = (t_sim *)arg;
	pthread_mutex_lock(&sim->start_mutex);
	pthread_mutex_unlock(&sim->start_mutex);
	while (!is_sim_over(sim))
	{
		status = check_all_coders(sim);
		if (status == -1)
			return (NULL);
		if (status == 1)
		{
			stop_simulation(sim);
			return (NULL);
		}
		broadcast_all_dongles(sim);
		usleep(1000);
	}
	return (NULL);
}
