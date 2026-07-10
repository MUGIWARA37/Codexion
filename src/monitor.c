/* ************************************************************************** */
/*																			*/
/*														:::		::::::::   */
/*   monitor.c											:+:		:+:	:+:   */
/*													+:+ +:+			+:+		*/
/*   By: rhlou <rhlou@student.42.fr>				+#+  +:+		+#+		*/
/*												+#+#+#+#+#+   +#+			*/
/*   Created: 2026/07/10 13:50:48 by rhlou				#+#	#+#				*/
/*   Updated: 2026/07/10 14:00:05 by rhlou			###   ########.fr		*/
/*																			*/
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
	while (1)
	{
		i = 0;
		while (i < sim->num_coders)
		{
			if (get_time_ms()
				- sim->coders[i].last_compile_start > sim->time_to_burnout)
			{
				handle_burnout(sim, sim->coders[i].id);
				return (NULL);
			}
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}
