/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pneto-vi <pneto-vi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 23:30:00 by pneto-vi          #+#    #+#             */
/*   Updated: 2026/10/25 23:30:00 by pneto-vi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	fail_stop(t_hub *hub)
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

static int	start_threads(t_hub *hub)
{
	int	i;

	i = 0;
	while (i < hub->number_coder)
	{
		if (pthread_create(&hub->coders[i].thread, NULL,
				coder_cycle, &hub->coders[i]) != 0)
		{
			fail_stop(hub);
			while (--i >= 0)
				pthread_join(hub->coders[i].thread, NULL);
			pthread_join(hub->monitor, NULL);
			return (-1);
		}
		i++;
	}
	return (0);
}

static void	cleanup(t_hub *hub)
{
	dongles_remove(hub);
	free_hub(hub);
	mutex_remove(hub);
}

static int	setup(t_hub *hub, int argc, char **argv)
{
	if (parse_args(hub, argc, argv) != 0)
		return (1);
	if (mutex_verification(hub) != 0)
		return (1);
	if (hub_alloc(hub) != 0)
	{
		mutex_remove(hub);
		return (1);
	}
	if (init_dongles(hub) != 0)
	{
		free_hub(hub);
		mutex_remove(hub);
		return (1);
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_hub	hub;
	int		i;

	if (setup(&hub, argc, argv) != 0)
		return (1);
	hub.start = time_now();
	init_coders(&hub);
	if (pthread_create(&hub.monitor, NULL, monitor, &hub) != 0
		|| start_threads(&hub) != 0)
	{
		cleanup(&hub);
		return (1);
	}
	i = 0;
	while (i < hub.number_coder)
	{
		pthread_join(hub.coders[i].thread, NULL);
		i++;
	}
	pthread_join(hub.monitor, NULL);
	cleanup(&hub);
	return (0);
}
