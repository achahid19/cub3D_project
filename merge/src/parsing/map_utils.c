/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achahid- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 19:45:40 by achahid-          #+#    #+#             */
/*   Updated: 2024/12/01 19:45:42 by achahid-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <parsing.h>

void	check_spaces_validity(char **map, t_cub3d **cub3d_data);
void	check_char(char c, t_cub3d **cub3d);
t_bool	invalid_char_file(char *line);
t_bool	valid_map_char(char c);

void	check_spaces_validity(char **map, t_cub3d **cub3d_data)
{
	size_t	i;
	size_t	j;

	i = 0;
	while (map[i])
	{
		j = 0;
		while (map[i][j])
		{
			if (ft_isspace(map[i][j]) == true)
			{
				check_char(map[i][j + 1], cub3d_data);
				if (j != 0)
					check_char(map[i][j - 1], cub3d_data);
				if (i != 0)
					check_char(map[i - 1][j], cub3d_data);
				if ((int)i < (*cub3d_data)->map.height - 1)
					check_char(map[i + 1][j], cub3d_data);
			}
			j++;
		}
		i++;
	}
}

void	check_char(char c, t_cub3d **cub3d)
{
	if (c == '0' || c == 'E' || c == 'N' || c == 'S' || c == 'W')
		err_exit("Error\nInvalid map spaces\n", cub3d);
}

t_bool	invalid_char_file(char *line)
{
	while (*line && line)
	{
		if (ft_isspace(*line) == false)
			break ;
		line++;
	}
	return (*line != '1' && *line != '\0' && *line != ' ');
}

t_bool	valid_map_char(char c)
{
	return (c == '0' || c == '1' || c == 'N' || c == 'S'
		|| c == 'W' || c == 'E' || c == ' ');
}