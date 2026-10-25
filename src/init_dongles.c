/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pneto-vi <pneto-vi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 23:30:00 by pneto-vi          #+#    #+#             */
/*   Updated: 2026/10/25 23:30:00 by pneto-vi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_one_dongle(t_dongle *d, int id, int mode, int n)
{
	d->dongle_id = id;
	d->holder = -1;
	d->cooldown = 0;
	d->next_request = 0;
	if (pthread_mutex_init(&d->mutex, NULL) != 0)
		return (1);
	if (pthread_cond_init(&d->condition, NULL) != 0)
	{
		pthread_mutex_destroy(&d->mutex);
		return (1);
	}
	d->wait.requests = malloc(sizeof(t_request) * (n + 1));
	if (!d->wait.requests)
	{
		pthread_cond_destroy(&d->condition);
		pthread_mutex_destroy(&d->mutex);
		return (1);
	}
	d->wait.size = 0;
	d->wait.max_capacity = n + 1;
	d->wait.mode = mode;
	return (0);
}

int	init_dongles(t_hub *hub)
{
	int	i;

	i = 0;
	while (i < hub->number_coder)
	{
		if (init_one_dongle(&hub->dongles[i], i, hub->scheduler,
				hub->number_coder) != 0)
		{
			while (--i >= 0)
			{
				free(hub->dongles[i].wait.requests);
				pthread_cond_destroy(&hub->dongles[i].condition);
				pthread_mutex_destroy(&hub->dongles[i].mutex);
			}
			return (1);
		}
		i++;
	}
	return (0);
}

void	dongles_remove(t_hub *hub)
{
	int	i;

	i = 0;
	while (i < hub->number_coder)
	{
		free(hub->dongles[i].wait.requests);
		hub->dongles[i].wait.requests = NULL;
		pthread_cond_destroy(&hub->dongles[i].condition);
		pthread_mutex_destroy(&hub->dongles[i].mutex);
		i++;
	}
}

void	mutex_remove(t_hub *hub)
{
	pthread_mutex_destroy(&hub->stop_mutex);
	pthread_mutex_destroy(&hub->mutex);
	pthread_mutex_destroy(&hub->mutex_state);
}

void	free_hub(t_hub *hub)
{
	if (hub->dongles)
		free(hub->dongles);
	if (hub->coders)
		free(hub->coders);
	hub->dongles = NULL;
	hub->coders = NULL;
}
