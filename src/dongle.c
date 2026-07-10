/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhlou <rhlou@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 12:19:32 by rhlou             #+#    #+#             */
/*   Updated: 2026/07/10 16:34:27 by rhlou            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	wait_for_dongle(t_dongle *dongle, int coder_id, t_sim *sim)
{
	struct timespec	ts;
	long long		wake_time;

	while (!is_sim_over(sim))
	{
		if (dongle->is_available == 1
			&& heap_peek_id(&dongle->wait_queue) == coder_id)
		{
			wake_time = dongle->released_at + sim->dongle_cooldown;
			if (get_time_ms() >= wake_time)
				break ;
			ts.tv_sec = wake_time / 1000;
			ts.tv_nsec = (wake_time % 1000) * 1000000;
			pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
		}
		else
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
}

void	dongle_acquire(t_dongle *dongle, long long priority, int coder_id,
		t_sim *sim)
{
	pthread_mutex_lock(&dongle->mutex);
	heap_push(&dongle->wait_queue, priority, coder_id);
	wait_for_dongle(dongle, coder_id, sim);
	if (is_sim_over(sim))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return ;
	}
	heap_pop(&dongle->wait_queue);
	dongle->is_available = 0;
	pthread_mutex_unlock(&dongle->mutex);
}

void	dongle_release(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_available = 1;
	dongle->released_at = get_time_ms();
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
