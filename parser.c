/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npedro <npedro@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 00:32:41 by npedro            #+#    #+#             */
/*   Updated: 2026/10/21 15:34:16 by npedro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

bool	check_number(char *str)
{
	int		i;

	i = 0;
	if (str[0] == '\0')
		return (0);
	while (str[i])
	{
		if ((str[i] >= '0') && (str[i] <= '9'))
		{
			i++;
		}
		else
		{
			return (0);
		}
	}
	return (1);
}


void create_thread(t_data data)
{
	int i;

	i = 0;
	while(i < data.number_coders)
	{
		pthread_create();
		printf("%d", i);
		i++;
	}
}
int	main(int argc, char **argv)
{
	int		i;
	t_data	data;

	i = 1;
	if (argc != 9)
	{
		printf("\nThe program should have 9 parameters");
		return (0);
	}
	while (i < 8)
	{
		if (!(check_number(argv[i])))
		{
			if(argv[1] == '0')
			{
				printf("\nNumber of coders should be grater than 0");
				return(0);
			}
			printf("\nInvalid argument position: ");
			printf("%d", i);
			printf("\nArgument: ");
			printf("%s", argv[i]);
			return (0);
		}
		i++;
	}
	data.number_coders = atoi(argv[1]);
	data.time_burnout = atoi(argv[2]);
	data.time_compile = atoi(argv[3]);
	data.time_debug = atoi(argv[4]);
	data.time_refactor = atoi(argv[5]);
	data.number_compiles = atoi(argv[6]);
	data.dongle_cooldown = atoi(argv[7]);
	if (strcmp(argv[i], "fifo") && (strcmp(argv[i], "edf")))
	{
		printf("\nIncorrect scheduler: either fifo/edf");
		return (0);
	}
	else
		data.scheduler = argv[8];
	create_thread(data);
}
