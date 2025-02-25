/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cduquair <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 13:55:14 by cduquair          #+#    #+#             */
/*   Updated: 2024/08/23 12:30:55 by cduquair         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*getbfr(char *str)
{
	int		i;
	char	*cut;

	i = 0;
	if (!str)
		return (NULL);
	while (str[i] != '\n' && str[i])
		i++;
	cut = malloc((i + 2) * sizeof(char));
	if (!cut)
		return (NULL);
	i = 0;
	while (str[i] != '\n' && str[i])
	{
		cut[i] = str[i];
		i++;
	}
	if (str[i] == '\n')
	{
		cut[i] = '\n';
		cut[i + 1] = '\0';
	}
	else
		cut[i] = '\0';
	return (cut);
}

char	*getaft(char *str)
{
	int		i;
	int		j;
	char	*cut;

	i = 1;
	j = 0;
	while (str[i - 1] != '\n' && str[i] && str[0])
		i++;
	if (!str[0])
		i = 0;
	while (str[i + j])
		j++;
	cut = malloc((j + 1) * sizeof(char));
	if (!cut)
		return (NULL);
	j = 0;
	while (str[i + j])
	{
		cut[j] = str[i + j];
		j++;
	}
	cut[j] = '\0';
	free(str);
	str = NULL;
	return (cut);
}

int	ft_linelength(int fd)
{
	int			i;
	static char	buffer[1];

	i = 0;
	while (read(fd, &buffer, 1) && buffer[0] != '\n')
	{
		i++;
	}
	return (i);
}

char	*gnl_exec(char *temp, char *stash, int fd)
{
	int		i;

	i = read(fd, temp, BUFFER_SIZE);
	while (i > 0)
	{
		temp[i] = '\0';
		stash = ft_strjoin(stash, temp);
		if (ft_strchr(temp, '\n'))
			break ;
		i = read(fd, temp, BUFFER_SIZE);
	}
	free(temp);
	if (i < 0 || (i == 0 && (!stash || stash[0] == '\0')))
	{
		if (stash != NULL)
		{
			free(stash);
			stash = NULL;
		}
		return (NULL);
	}
	return (stash);
}
