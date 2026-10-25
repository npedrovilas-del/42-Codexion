/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pneto-vi <pneto-vi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 23:30:00 by pneto-vi          #+#    #+#             */
/*   Updated: 2026/10/25 23:30:00 by pneto-vi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	hub_stop(t_hub *hub)
{
	int	stopped;

	pthread_mutex_lock(&hub->stop_mutex);
	stopped = hub->stopped;
	pthread_mutex_unlock(&hub->stop_mutex);
	return (stopped);
}

void	log_state(t_hub *hub, int coder_id, const char *msg)
{
	pthread_mutex_lock(&hub->mutex);
	if (!hub_stop(hub))
		printf("%ld %d %s\n", time_since_start(hub), coder_id, msg);
	pthread_mutex_unlock(&hub->mutex);
}

void	log_burnout(t_hub *hub, int coder_id)
{
	pthread_mutex_lock(&hub->mutex);
	printf("%ld %d burned out\n", time_since_start(hub), coder_id);
	pthread_mutex_unlock(&hub->mutex);
}
