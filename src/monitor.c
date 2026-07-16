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

static void	stop_simulation(t_sim *sim)
{
	int	j;

	pthread_mutex_lock(&sim->stop_mutex);
	sim->simulation_over = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	j = 0;
	while (j < sim->num_coders)
	{
		pthread_mutex_lock(&sim->dongles[j].mutex);
		pthread_cond_broadcast(&sim->dongles[j].cond);
		pthread_mutex_unlock(&sim->dongles[j].mutex);
		j++;
	}
}

static void	handle_burnout(t_sim *sim, int coder_id)
{
	log_event(sim, coder_id, "burned out");
	stop_simulation(sim);
}

void	*monitor_routine(void *arg)
{
	int		i;
	int		all_finished;
	t_sim	*sim;

	sim = (t_sim *)arg;
	while (!is_sim_over(sim))
	{
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
				return (NULL);
			}
			if (sim->coders[i].compile_count < sim->num_compiles_required)
				all_finished = 0;
			pthread_mutex_unlock(&sim->coders[i].coder_mutex);
			i++;
		}
		if (sim->num_compiles_required != -1 && all_finished)
		{
			stop_simulation(sim);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
