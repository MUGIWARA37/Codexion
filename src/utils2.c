/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhlou <rhlou@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/10 14:01:34 by rhlou             #+#    #+#             */
/*   Updated: 2026/07/26 13:40:14 by rhlou            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_valid_number(const char *str)
{
	int	i;

	i = 0;
	if (str[i] == '-' || str[i] == '+')
		i++;
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

void	clean_up_failed(t_sim *sim, int c_done, int d_done, int g_done)
{
	int	i;

	if (!sim)
		return ;
	i = -1;
	while (++i < c_done)
		pthread_mutex_destroy(&sim->coders[i].coder_mutex);
	i = -1;
	while (++i < d_done)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		free(sim->dongles[i].wait_queue.data);
	}
	if (g_done >= 4)
		pthread_mutex_destroy(&sim->fifo_mutex);
	if (g_done >= 3)
		pthread_mutex_destroy(&sim->start_mutex);
	if (g_done >= 2)
		pthread_mutex_destroy(&sim->stop_mutex);
	if (g_done >= 1)
		pthread_mutex_destroy(&sim->log_mutex);
}

int	init_global_mutexes(t_sim *sim)
{
	if (pthread_mutex_init(&sim->log_mutex, NULL))
		return (clean_up_failed(sim, sim->num_coders, sim->num_coders, 0), -1);
	if (pthread_mutex_init(&sim->stop_mutex, NULL))
		return (clean_up_failed(sim, sim->num_coders, sim->num_coders, 1), -1);
	if (pthread_mutex_init(&sim->start_mutex, NULL))
		return (clean_up_failed(sim, sim->num_coders, sim->num_coders, 2), -1);
	if (pthread_mutex_init(&sim->fifo_mutex, NULL))
		return (clean_up_failed(sim, sim->num_coders, sim->num_coders, 3), -1);
	return (0);
}
