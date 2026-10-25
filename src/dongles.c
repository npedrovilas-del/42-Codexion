/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pneto-vi <pneto-vi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 23:30:00 by pneto-vi          #+#    #+#             */
/*   Updated: 2026/10/25 23:30:00 by pneto-vi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	dongle_elig(t_dongle *d, t_coder *coder, long int now)
{
	t_request	*top;

	top = heap_verification(&d->wait);
	if (!top || top->id_code != coder->coder_id)
		return (0);
	if (d->holder >= 0)
		return (0);
	if (now < d->cooldown)
		return (0);
	return (1);
}

static int	queue(t_dongle *d, t_coder *coder)
{
	t_request	req;

	req.id_code = coder->coder_id;
	req.deadline = coder->last_compile + coder->hub->time_burnout;
	req.seq = (unsigned int)d->next_request++;
	return (heap_push(&d->wait, req));
}

static void	ms_to_abstime(long int ms_from_now, struct timespec *ts)
{
	struct timeval	tv;
	long int		usec;

	gettimeofday(&tv, NULL);
	usec = (long int)tv.tv_usec + ms_from_now * 1000L;
	ts->tv_sec = tv.tv_sec + usec / 1000000L;
	ts->tv_nsec = (usec % 1000000L) * 1000L;
}

int	dongle_take(t_dongle *d, t_coder *coder)
{
	struct timespec	ts;

	pthread_mutex_lock(&d->mutex);
	if (queue(d, coder) != 0)
	{
		pthread_mutex_unlock(&d->mutex);
		return (-1);
	}
	while (!hub_stop(coder->hub))
	{
		if (dongle_elig(d, coder, time_now()))
		{
			heap_pop(&d->wait, NULL);
			d->holder = coder->coder_id;
			pthread_mutex_unlock(&d->mutex);
			log_state(coder->hub, coder->coder_id, "has taken a dongle");
			return (0);
		}
		ms_to_abstime(1, &ts);
		pthread_cond_timedwait(&d->condition, &d->mutex, &ts);
	}
	heap_remover(&d->wait, coder->coder_id);
	pthread_mutex_unlock(&d->mutex);
	return (-1);
}

void	dongle_release(t_dongle *d, t_hub *hub)
{
	pthread_mutex_lock(&d->mutex);
	d->holder = -1;
	d->cooldown = time_now() + hub->dongle_cooldown;
	pthread_cond_broadcast(&d->condition);
	pthread_mutex_unlock(&d->mutex);
}
