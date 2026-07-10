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

void	dongle_acquire(t_dongle *dongle, long long priority, int coder_id,
		t_sim *sim)
{
	pthread_mutex_lock(&dongle->mutex);
	heap_push(&dongle->wait_queue, priority, coder_id);
	while (!is_sim_over(sim))
	{
		if (dongle->is_available == 1
			&& heap_peek_id(&dongle->wait_queue) == coder_id
			&& get_time_ms() >= dongle->released_at + sim->dongle_cooldown)
			break ;
		else
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
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
