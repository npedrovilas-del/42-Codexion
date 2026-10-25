/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pneto-vi <pneto-vi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 23:30:00 by pneto-vi          #+#    #+#             */
/*   Updated: 2026/10/25 23:30:00 by pneto-vi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	parse_number(const char *s, int *out)
{
	long	n;
	int		i;

	if (!s || !s[0])
		return (-1);
	i = 0;
	n = 0;
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (-1);
		n = n * 10 + (s[i] - '0');
		if (n > (long)INT_MAX)
			return (-1);
		i++;
	}
	*out = n;
	return (0);
}

static int	parse_long(const char *s, long int *out)
{
	int	tmp;

	if (parse_number(s, &tmp) != 0)
		return (-1);
	*out = tmp;
	return (0);
}

static int	parse_scheduler(const char *s, int *out)
{
	if (strcmp(s, "fifo") == 0)
	{
		*out = 0;
		return (0);
	}
	if (strcmp(s, "edf") == 0)
	{
		*out = 1;
		return (0);
	}
	return (-1);
}

int	parse_args(t_hub *hub, int argc, char **argv)
{
	if (argc != 9)
		return (1);
	memset(hub, 0, sizeof(*hub));
	if (parse_number(argv[1], &hub->number_coder) != 0)
		return (1);
	if (hub->number_coder < 1)
		return (1);
	if (parse_long(argv[2], &hub->time_burnout) != 0
		|| parse_long(argv[3], &hub->time_compile) != 0
		|| parse_long(argv[4], &hub->time_debug) != 0
		|| parse_long(argv[5], &hub->time_refactor) != 0)
		return (1);
	if (parse_number(argv[6], &hub->compiles_required) != 0
		|| parse_number(argv[7], &hub->dongle_cooldown) != 0)
		return (1);
	if (parse_scheduler(argv[8], &hub->scheduler) != 0)
		return (1);
	return (0);
}
