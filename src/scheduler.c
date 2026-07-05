/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhlou <rhlou@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/05 13:34:27 by rhlou             #+#    #+#             */
/*   Updated: 2026/07/05 17:24:03 by rhlou            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


void ft_swap_heap_entery(t_heap_entry *a, t_heap_entry *b)
{
    t_heap_entry tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
    
}

int  heap_init(t_heap *h)
{
    if (!h)
        return (-1);

    h->capacity = 2;
    h->size = 0;
    h->data = malloc(h->capacity * sizeof(t_heap_entry));
    if (!h->data)
        return (-1);
    return (0);
}

void            sift_up(t_heap *h, int i)
{
    int parent_index;

    if (!h || !i)
        return ;
    while (i > 0)
    {
        parent_index = (i - 1)/2;
        if (h->data[i].priority < h->data[parent_index].priority
            || (h->data[i].priority == h->data[parent_index].priority
            && h->data[i].coder_id < h->data[parent_index].coder_id))
        {
            ft_swap_heap_entery(&h->data[i], &h->data[parent_index]);
            i = parent_index;
        }
        else
            break;
        
    }
    
}

void    sift_down(t_heap *h, int i)
{
    int left;
    int right;
    int smallest;

    if (!h || !h->size)
        return ;
    while (2 * i + 1 < h->size)
    {
        left = 2 * i + 1;
        right = left + 1;
        smallest = i;
        if (left < h->size && (h->data[left].priority < h->data[smallest].priority
                || (h->data[left].priority == h->data[smallest].priority
                && h->data[left].coder_id < h->data[smallest].coder_id)))
            smallest = left;
        if (right < h->size && (h->data[right].priority < h->data[smallest].priority
                || (h->data[right].priority == h->data[smallest].priority
                && h->data[right].coder_id < h->data[smallest].coder_id)))
            smallest = right;
        if (smallest == i)
            break;
        ft_swap_heap_entery(&h->data[i], &h->data[smallest]);
        i = smallest;
    }
}

int     heap_push(t_heap *h, long long priority, int coder_id)
{
    t_heap_entry new_entry;
    t_heap_entry *tmp;

    if (!h)
        return (-1);
    if (h->size == h->capacity)
    {
        h->capacity *= 2;
        tmp = realloc(h->data, h->capacity *sizeof(t_heap_entry));
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


t_heap_entry    heap_pop(t_heap *h)
{
    t_heap_entry empty;
    t_heap_entry data;

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

int     heap_peek_id(t_heap *h)
{
    if (!h || !h->data || !h->size)
        return (-1);
    return (h->data[0].coder_id);
}