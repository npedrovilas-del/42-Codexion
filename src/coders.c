/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pneto-vi <pneto-vi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 23:30:00 by pneto-vi          #+#    #+#             */
/*   Updated: 2026/10/25 23:30:00 by pneto-vi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	get_dongles(t_coder *coder)
{
	if (coder->hub->number_coder == 1)
	{
		if (dongle_take(coder->first, coder) != 0)
			return (-1);
		while (!hub_stop(coder->hub))
			usleep(200);
		dongle_release(coder->first, coder->hub);
		return (-1);
	}
	if (dongle_take(coder->first, coder) != 0)
		return (-1);
	if (dongle_take(coder->second, coder) != 0)
	{
		dongle_release(coder->first, coder->hub);
		return (-1);
	}
	return (0);
}

static void	release_dongles(t_coder *coder)
{
	dongle_release(coder->first, coder->hub);
	dongle_release(coder->second, coder->hub);
}

static int	do_compile(t_coder *coder)
{
	if (get_dongles(coder) != 0)
		return (-1);
	if (hub_stop(coder->hub))
	{
		release_dongles(coder);
		return (-1);
	}
	pthread_mutex_lock(&coder->hub->mutex_state);
	coder->last_compile = time_now();
	coder->compiles++;
	pthread_mutex_unlock(&coder->hub->mutex_state);
	log_state(coder->hub, coder->coder_id, "is compiling");
	precise_sleep(coder->hub->time_compile, coder->hub);
	release_dongles(coder);
	return (0);
}

static int	do_cycle(t_coder *coder)
{
	if (do_compile(coder) != 0)
		return (-1);
	if (hub_stop(coder->hub))
		return (-1);
	log_state(coder->hub, coder->coder_id, "is debugging");
	precise_sleep(coder->hub->time_debug, coder->hub);
	if (hub_stop(coder->hub))
		return (-1);
	log_state(coder->hub, coder->coder_id, "is refactoring");
	precise_sleep(coder->hub->time_refactor, coder->hub);
	return (0);
}

void	*coder_cycle(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->coder_id % 2 == 0)
		usleep(500);
	while (!hub_stop(coder->hub))
	{
		if (do_cycle(coder) != 0)
			break ;
	}
	return (NULL);
}
