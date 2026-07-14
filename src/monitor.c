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

static void	handle_burnout(t_sim *sim, int coder_id)
{
	int	j;

	log_event(sim, coder_id, "burned out");
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

void	*monitor_routine(void *arg)
{
	int		i;
	t_sim	*sim;

	sim = (t_sim *)arg;
	while (!is_sim_over(sim))
	{
		i = 0;
		while (i < sim->num_coders)
		{
			pthread_mutex_lock(&sim->coders[i].coder_mutex);
			if (!sim->coders[i].finished && get_time_ms()
				- sim->coders[i].last_compile_start > sim->time_to_burnout)
			{
				pthread_mutex_unlock(&sim->coders[i].coder_mutex);
				handle_burnout(sim, sim->coders[i].id);
				return (NULL);
			}
			pthread_mutex_unlock(&sim->coders[i].coder_mutex);
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}
