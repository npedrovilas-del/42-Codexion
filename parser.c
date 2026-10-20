/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npedro <npedro@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 00:32:41 by npedro            #+#    #+#             */
/*   Updated: 2026/10/21 00:43:41 by npedro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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

int	main(int argc, char **argv)
{
	int	i;

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
			printf("\nInvalid argument position: ");
			printf("%d", i);
			printf("\nArgument: ");
			printf("%s", argv[i]);
			return (0);
		}
		i++;
	}
	if (strcmp(argv[i], "fifo") && (strcmp(argv[i], "edf")))
	{
		printf("\nIncorrect scheduler: either fifo/edf");
		return (0);
	}
}
