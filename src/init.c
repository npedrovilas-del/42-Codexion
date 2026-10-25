/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pneto-vi <pneto-vi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 23:30:00 by pneto-vi          #+#    #+#             */
/*   Updated: 2026/10/25 23:30:00 by pneto-vi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	mutex_verification(t_hub *hub)
{
	if (pthread_mutex_init(&hub->stop_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&hub->mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&hub->stop_mutex);
		return (1);
	}
	if (pthread_mutex_init(&hub->mutex_state, NULL) != 0)
	{
		pthread_mutex_destroy(&hub->mutex);
		pthread_mutex_destroy(&hub->stop_mutex);
		return (1);
	}
	return (0);
}

int	hub_alloc(t_hub *hub)
{
	hub->dongles = malloc(sizeof(t_dongle) * hub->number_coder);
	if (!hub->dongles)
		return (1);
	hub->coders = malloc(sizeof(t_coder) * hub->number_coder);
	if (!hub->coders)
	{
		free(hub->dongles);
		hub->dongles = NULL;
		return (1);
	}
	memset(hub->dongles, 0, sizeof(t_dongle) * hub->number_coder);
	memset(hub->coders, 0, sizeof(t_coder) * hub->number_coder);
	return (0);
}

static void	dongles_div(t_hub *hub, t_coder *coder, int i)
{
	t_dongle	*left;
	t_dongle	*right;

	left = &hub->dongles[i];
	right = &hub->dongles[(i + 1) % hub->number_coder];
	if (hub->number_coder == 1)
	{
		coder->first = left;
		coder->second = left;
		return ;
	}
	if (left->dongle_id < right->dongle_id)
	{
		coder->first = left;
		coder->second = right;
	}
	else
	{
		coder->first = right;
		coder->second = left;
	}
}

void	init_coders(t_hub *hub)
{
	int	i;

	i = 0;
	while (i < hub->number_coder)
	{
		hub->coders[i].coder_id = i + 1;
		hub->coders[i].compiles = 0;
		hub->coders[i].last_compile = hub->start;
		hub->coders[i].hub = hub;
		dongles_div(hub, &hub->coders[i], i);
		i++;
	}
}
