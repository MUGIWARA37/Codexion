/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhlou <rhlou@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 12:19:32 by rhlou             #+#    #+#             */
/*   Updated: 2026/07/24 12:00:39 by rhlou            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	wait_for_dongle(t_dongle *dongle, int coder_id, t_sim *sim)
{
	struct timespec	ts;
	long long		target;

	while (!is_sim_over(sim))
	{
		if (dongle->is_available == 1
			&& heap_peek_id(&dongle->wait_queue) == coder_id)
		{
			target = dongle->released_at + sim->dongle_cooldown;
			if (get_time_ms() >= target)
				break ;
			set_target_timespec(target, &ts);
			pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
			continue ;
		}
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
}

static int	wait_and_pop_dongle(t_dongle *dongle, t_coder *coder)
{
	pthread_mutex_lock(&dongle->mutex);
	wait_for_dongle(dongle, coder->id, coder->sim);
	if (is_sim_over(coder->sim))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	heap_pop(&dongle->wait_queue);
	dongle->is_available = 0;
	pthread_mutex_unlock(&dongle->mutex);
	return (1);
}

int	dongle_acquire(t_dongle *dongle, long long priority,
		t_coder *coder)
{
	pthread_mutex_lock(&dongle->mutex);
	if (heap_push(&dongle->wait_queue, priority, coder->compile_count,
			coder->id) == -1)
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	pthread_mutex_unlock(&dongle->mutex);
	return (wait_and_pop_dongle(dongle, coder));
}

int	dongles_acquire(t_dongle *first, t_dongle *second, long long priority,
		t_coder *coder)
{
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	heap_push(&first->wait_queue, priority, coder->compile_count, coder->id);
	heap_push(&second->wait_queue, priority, coder->compile_count, coder->id);
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	if (!wait_and_pop_dongle(first, coder))
		return (0);
	if (!wait_and_pop_dongle(second, coder))
	{
		dongle_release(first);
		return (0);
	}
	return (1);
}

void	dongle_release(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_available = 1;
	dongle->released_at = get_time_ms();
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
