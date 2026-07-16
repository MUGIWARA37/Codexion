/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhlou <rhlou@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/10 14:01:34 by rhlou             #+#    #+#             */
/*   Updated: 2026/07/13 12:14:01 by rhlou            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_push(t_heap *h, long long priority, int compile_count,
		int coder_id)
{
	t_heap_entry	new_entry;

	if (!h || h->size == h->capacity)
		return (-1);
	new_entry.coder_id = coder_id;
	new_entry.priority = priority;
	new_entry.compile_count = compile_count;
	h->data[h->size] = new_entry;
	h->size++;
	sift_up(h, h->size - 1);
	return (0);
}

t_heap_entry	heap_pop(t_heap *h)
{
	t_heap_entry	empty;
	t_heap_entry	data;

	if (!h || !h->size)
	{
		empty.priority = -1;
		empty.compile_count = -1;
		empty.coder_id = -1;
		return (empty);
	}
	data = h->data[0];
	h->data[0] = h->data[h->size - 1];
	h->size--;
	sift_down(h, 0);
	return (data);
}

int	heap_peek_id(t_heap *h)
{
	if (!h || !h->data || !h->size)
		return (-1);
	return (h->data[0].coder_id);
}
