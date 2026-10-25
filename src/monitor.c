/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pneto-vi <pneto-vi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 23:30:00 by pneto-vi          #+#    #+#             */
/*   Updated: 2026/10/25 23:30:00 by pneto-vi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	set_stopped(t_hub *hub)
{
	int	i;

	i = 0;
	pthread_mutex_lock(&hub->stop_mutex);
	hub->stopped = 1;
	pthread_mutex_unlock(&hub->stop_mutex);
	while (i < hub->number_coder)
	{
		pthread_mutex_lock(&hub->dongles[i].mutex);
		pthread_cond_broadcast(&hub->dongles[i].condition);
		pthread_mutex_unlock(&hub->dongles[i].mutex);
		i++;
	}
}

static int	check_burnouts(t_hub *hub)
{
	int			i;
	long int	now;
	long int	elapsed;

	i = 0;
	now = time_now();
	pthread_mutex_lock(&hub->mutex_state);
	while (i < hub->number_coder)
	{
		elapsed = now - hub->coders[i].last_compile;
		if (elapsed >= hub->time_burnout)
		{
			pthread_mutex_unlock(&hub->mutex_state);
			return (hub->coders[i].coder_id);
		}
		i++;
	}
	pthread_mutex_unlock(&hub->mutex_state);
	return (0);
}

static int	check_done(t_hub *hub)
{
	int	i;

	i = 0;
	pthread_mutex_lock(&hub->mutex_state);
	while (i < hub->number_coder)
	{
		if (hub->coders[i].compiles < hub->compiles_required)
		{
			pthread_mutex_unlock(&hub->mutex_state);
			return (0);
		}
		i++;
	}
	pthread_mutex_unlock(&hub->mutex_state);
	return (1);
}

void	*monitor(void *arg)
{
	t_hub	*hub;
	int		position;

	hub = (t_hub *)arg;
	while (!hub_stop(hub))
	{
		position = check_burnouts(hub);
		if (position > 0)
		{
			set_stopped(hub);
			log_burnout(hub, position);
			break ;
		}
		if (check_done(hub))
		{
			set_stopped(hub);
			break ;
		}
		usleep(500);
	}
	return (NULL);
}
