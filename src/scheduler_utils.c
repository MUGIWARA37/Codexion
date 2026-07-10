/* ************************************************************************** */
/*																			*/
/*														:::		::::::::   */
/*   main.c												:+:		:+:	:+:   */
/*													+:+ +:+			+:+		*/
/*   By: rhlou <rhlou@student.42.fr>				+#+  +:+		+#+		*/
/*												+#+#+#+#+#+   +#+			*/
/*   Created: 2026/07/10 14:01:34 by rhlou				#+#	#+#				*/
/*   Updated: 2026/07/10 14:44:15 by rhlou			###   ########.fr		*/
/*																			*/
/* ************************************************************************** */
#include "codexion.h"

int	heap_push(t_heap *h, long long priority, int coder_id)
{
	t_heap_entry	new_entry;
	t_heap_entry	*tmp;

	if (!h)
		return (-1);
	if (h->size == h->capacity)
	{
		h->capacity *= 2;
		tmp = realloc(h->data, h->capacity * sizeof(t_heap_entry));
		if (!tmp)
		{
			h->capacity /= 2;
			return (-1);
		}
		h->data = tmp;
	}
	new_entry.coder_id = coder_id;
	new_entry.priority = priority;
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
