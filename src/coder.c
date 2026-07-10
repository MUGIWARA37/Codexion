/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhlou <rhlou@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 13:14:03 by rhlou             #+#    #+#             */
/*   Updated: 2026/07/10 16:34:23 by rhlou            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long long	get_priority(t_coder *coder, t_sim *sim)
{
	long long	priority;

	if (sim->use_edf)
		priority = coder->last_compile_start + sim->time_to_burnout;
	else
	{
		pthread_mutex_lock(&sim->fifo_mutex);
		priority = sim->fifo_counter++;
		pthread_mutex_unlock(&sim->fifo_mutex);
	}
	return (priority);
}

static int	perform_compile(t_coder *coder, t_dongle *first, t_dongle *second)
{
	t_sim	*sim;

	sim = coder->sim;
	coder->last_compile_start = get_time_ms();
	log_event(sim, coder->id, "is compiling");
	ft_msleep(sim->time_to_compile, sim);
	coder->compile_count++;
	dongle_release(second);
	dongle_release(first);
	if (sim->num_compiles_required != -1
		&& coder->compile_count >= sim->num_compiles_required)
		return (1);
	return (0);
}

static void	coder_loop(t_coder *coder, t_dongle *first, t_dongle *second)
{
	t_sim		*sim;
	long long	priority;

	sim = coder->sim;
	while (!is_sim_over(sim))
	{
		priority = get_priority(coder, sim);
		dongle_acquire(first, priority, coder->id, sim);
		dongle_acquire(second, priority, coder->id, sim);
		if (is_sim_over(sim))
		{
			dongle_release(second);
			dongle_release(first);
			break ;
		}
		if (perform_compile(coder, first, second))
			break ;
		log_event(sim, coder->id, "is debugging");
		ft_msleep(sim->time_to_debug, sim);
		log_event(sim, coder->id, "is refactoring");
		ft_msleep(sim->time_to_refactor, sim);
	}
}

void	*coder_routine(void *arg)
{
	t_dongle	*first;
	t_dongle	*second;
	t_coder		*coder;
	t_sim		*sim;

	coder = (t_coder *)arg;
	sim = coder->sim;
	if (coder->id % 2 == 0)
	{
		first = &sim->dongles[coder->id - 1];
		second = &sim->dongles[coder->id % sim->num_coders];
	}
	else
	{
		first = &sim->dongles[coder->id % sim->num_coders];
		second = &sim->dongles[coder->id - 1];
	}
	coder_loop(coder, first, second);
	return (NULL);
}
