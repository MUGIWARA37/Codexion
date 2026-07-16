/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhlou <rhlou@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/05 13:34:27 by rhlou             #+#    #+#             */
/*   Updated: 2026/07/10 16:32:14 by rhlou            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	ft_swap_heap_entery(t_heap_entry *a, t_heap_entry *b)
{
	t_heap_entry	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

int	heap_init(t_heap *h, int capacity)
{
	if (!h)
		return (-1);
	h->capacity = capacity;
	h->size = 0;
	h->data = malloc(h->capacity * sizeof(t_heap_entry));
	if (!h->data)
		return (-1);
	return (0);
}

static int	is_higher_priority(t_heap_entry a, t_heap_entry b)
{
	if (a.priority != b.priority)
		return (a.priority < b.priority);
	if (a.compile_count != b.compile_count)
		return (a.compile_count < b.compile_count);
	return (a.coder_id < b.coder_id);
}

void	sift_up(t_heap *h, int i)
{
	int	parent_index;

	if (!h || !i)
		return ;
	while (i > 0)
	{
		parent_index = (i - 1) / 2;
		if (is_higher_priority(h->data[i], h->data[parent_index]))
		{
			ft_swap_heap_entery(&h->data[i], &h->data[parent_index]);
			i = parent_index;
		}
		else
			break ;
	}
}

void	sift_down(t_heap *h, int i)
{
	int	left;
	int	right;
	int	smallest;

	if (!h || !h->size)
		return ;
	while (2 * i + 1 < h->size)
	{
		left = 2 * i + 1;
		right = left + 1;
		smallest = i;
		if (left < h->size && is_higher_priority(h->data[left],
				h->data[smallest]))
			smallest = left;
		if (right < h->size && is_higher_priority(h->data[right],
				h->data[smallest]))
			smallest = right;
		if (smallest == i)
			break ;
		ft_swap_heap_entery(&h->data[i], &h->data[smallest]);
		i = smallest;
	}
}
