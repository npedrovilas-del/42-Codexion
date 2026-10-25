/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pneto-vi <pneto-vi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 23:30:00 by pneto-vi          #+#    #+#             */
/*   Updated: 2026/10/25 23:30:00 by pneto-vi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	search_better(t_heap *heap, t_request *a, t_request *b)
{
	if (heap->mode == 0)
	{
		if (a->seq != b->seq)
			return (a->seq < b->seq);
		return (a->id_code < b->id_code);
	}
	else
	{
		if (a->deadline != b->deadline)
			return (a->deadline < b->deadline);
		if (a->seq != b->seq)
			return (a->seq < b->seq);
		return (a->id_code < b->id_code);
	}
}

void	heap_swap(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	heap_sift_up(t_heap *h, int i)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!search_better(h, &h->requests[i], &h->requests[parent]))
			break ;
		heap_swap(&h->requests[i], &h->requests[parent]);
		i = parent;
	}
}

void	heap_sift_down(t_heap *h, int i)
{
	int	left;
	int	right;
	int	curr;

	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		curr = i;
		if (left < h->size
			&& search_better(h, &h->requests[left], &h->requests[curr]))
			curr = left;
		if (right < h->size
			&& search_better(h, &h->requests[right], &h->requests[curr]))
			curr = right;
		if (curr == i)
			break ;
		heap_swap(&h->requests[i], &h->requests[curr]);
		i = curr;
	}
}

t_request	*heap_verification(t_heap *h)
{
	if (h->size <= 0)
		return (NULL);
	return (&h->requests[0]);
}
