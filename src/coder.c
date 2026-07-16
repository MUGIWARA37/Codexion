/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhlou <rhlou@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 13:14:03 by rhlou             #+#    #+#             */
/*   Updated: 2026/07/14 13:44:15 by rhlou            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long long	get_priority(t_coder *coder, t_sim *sim)
{
	long long	priority;

	if (sim->use_edf)
	{
		pthread_mutex_lock(&coder->coder_mutex);
		priority = coder->last_compile_start + sim->time_to_burnout;
		pthread_mutex_unlock(&coder->coder_mutex);
	}
	else
	{
		pthread_mutex_lock(&sim->fifo_mutex);
		priority = --sim->fifo_counter;
		pthread_mutex_unlock(&sim->fifo_mutex);
	}
	return (priority);
}

static int	check_and_update_timer(t_coder *coder)
{
	pthread_mutex_lock(&coder->coder_mutex);
	while (!is_sim_over(coder->sim) && get_time_ms()
		- coder->last_compile_start > coder->sim->time_to_burnout)
	{
		pthread_mutex_unlock(&coder->coder_mutex);
		usleep(100);
		pthread_mutex_lock(&coder->coder_mutex);
	}
	if (is_sim_over(coder->sim))
	{
		pthread_mutex_unlock(&coder->coder_mutex);
		return (1);
	}
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&coder->coder_mutex);
	return (0);
}

static int	perform_compile(t_coder *coder, t_dongle *first, t_dongle *second)
{
	t_sim	*sim;

	sim = coder->sim;
	if (check_and_update_timer(coder))
	{
		dongle_release(second);
		dongle_release(first);
		return (1);
	}
	log_event(sim, coder->id, "is compiling");
	ft_msleep(sim->time_to_compile, sim);
	coder->compile_count++;
	dongle_release(second);
	dongle_release(first);
	if (sim->num_compiles_required != -1
		&& coder->compile_count >= sim->num_compiles_required)
	{
		pthread_mutex_lock(&coder->coder_mutex);
		coder->finished = 1;
		pthread_mutex_unlock(&coder->coder_mutex);
		return (1);
	}
	return (0);
}

static void	coder_loop(t_coder *coder, t_dongle *first, t_dongle *second)
{
	long long	priority;
	int			a[2];

	if (coder->sim->num_compiles_required == 0)
		return ;
	while (!is_sim_over(coder->sim))
	{
		priority = get_priority(coder, coder->sim);
		a[0] = dongle_acquire(first, priority, coder);
		a[1] = a[0] && dongle_acquire(second, priority, coder);
		if (!a[1])
		{
			if (a[0])
				dongle_release(first);
			break ;
		}
		if (perform_compile(coder, first, second))
			break ;
		log_event(coder->sim, coder->id, "is debugging");
		ft_msleep(coder->sim->time_to_debug, coder->sim);
		log_event(coder->sim, coder->id, "is refactoring");
		ft_msleep(coder->sim->time_to_refactor, coder->sim);
	}
}

void	*coder_routine(void *arg)
{
	t_dongle	*f;
	t_dongle	*s;
	t_coder		*c;

	c = (t_coder *)arg;
	if (c->sim->num_coders == 1)
	{
		dongle_acquire(&c->sim->dongles[0], get_priority(c, c->sim),
			c);
		ft_msleep(c->sim->time_to_burnout + 10, c->sim);
		dongle_release(&c->sim->dongles[0]);
		return (NULL);
	}
	f = &c->sim->dongles[c->id - 1];
	s = &c->sim->dongles[c->id % c->sim->num_coders];
	if (c->id % 2 != 0)
	{
		f = &c->sim->dongles[c->id % c->sim->num_coders];
		s = &c->sim->dongles[c->id - 1];
	}
	coder_loop(c, f, s);
	return (NULL);
}
