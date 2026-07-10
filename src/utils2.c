/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhlou <rhlou@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/10 14:01:34 by rhlou             #+#    #+#             */
/*   Updated: 2026/07/10 16:32:52 by rhlou            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	signcheck(int sign)
{
	if (sign == -1)
		return (0);
	return (-1);
}

static int	space_skiper(const char *nptr)
{
	int	i;

	i = 0;
	while (nptr[i] == ' ' || (nptr[i] >= 9 && nptr[i] <= 13))
		i++;
	return (i);
}

int	ft_atoi(const char *nptr)
{
	int		i;
	int		sign;
	long	res;
	long	check;

	sign = 1;
	res = 0;
	i = space_skiper(nptr);
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			sign = -1;
		i++;
	}
	check = 0;
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		res = res * 10 + (nptr[i] - '0');
		if (res / 10 != check)
			return (signcheck(sign));
		check = res;
		i++;
	}
	return (res * sign);
}
