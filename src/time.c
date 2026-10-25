/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pneto-vi <pneto-vi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 23:30:00 by pneto-vi          #+#    #+#             */
/*   Updated: 2026/10/25 23:30:00 by pneto-vi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long int	time_now(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long int)tv.tv_sec * 1000LL + (long int)tv.tv_usec / 1000LL);
}

long int	time_since_start(t_hub *hub)
{
	return (time_now() - hub->start);
}

void	precise_sleep(long int ms, t_hub *hub)
{
	long int	start;

	start = time_now();
	while (time_now() - start < ms)
	{
		if (hub_stop(hub))
			return ;
		usleep(200);
	}
}
