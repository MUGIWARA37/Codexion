/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhlou <rhlou@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/05 12:05:03 by rhlou             #+#    #+#             */
/*   Updated: 2026/07/10 16:32:36 by rhlou            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	ft_msleep(long long ms, t_sim *sim)
{
	long long	i;

	if (ms <= 0)
		return ;
	i = 0;
	while (i < ms)
	{
		if (is_sim_over(sim))
			return ;
		usleep(1000);
		i++;
	}
}

int	is_sim_over(t_sim *sim)
{
	int	val;

	pthread_mutex_lock(&sim->stop_mutex);
	val = sim->simulation_over;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (val);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	int	i;

	if (!s1 && !s2)
		return (0);
	if (!s1)
		return (-1 * s2[0]);
	if (!s2)
		return (s1[0]);
	i = 0;
	while (s1[i] && s2[i])
	{
		if (s1[i] != s2[i])
			return (s1[i] - s2[i]);
		i++;
	}
	return (s1[i] - s2[i]);
}
