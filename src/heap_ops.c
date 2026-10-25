/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_ops.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pneto-vi <pneto-vi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 23:30:00 by pneto-vi          #+#    #+#             */
/*   Updated: 2026/10/25 23:30:00 by pneto-vi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_push(t_heap *h, t_request req)
{
	if (h->size >= h->max_capacity)
		return (-1);
	h->requests[h->size] = req;
	heap_sift_up(h, h->size);
	h->size++;
	return (0);
}

int	heap_pop(t_heap *h, t_request *out)
{
	if (h->size <= 0)
		return (-1);
	if (out)
		*out = h->requests[0];
	h->size--;
	if (h->size > 0)
	{
		h->requests[0] = h->requests[h->size];
		heap_sift_down(h, 0);
	}
	return (0);
}

int	heap_remover(t_heap *h, int coder_id)
{
	int	i;

	i = 0;
	while (i < h->size)
	{
		if (h->requests[i].id_code == coder_id)
		{
			h->size--;
			if (i < h->size)
			{
				h->requests[i] = h->requests[h->size];
				heap_sift_down(h, i);
				heap_sift_up(h, i);
			}
			return (0);
		}
		i++;
	}
	return (-1);
}
