/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key_events.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 19:27:25 by zel-khad          #+#    #+#             */
/*   Updated: 2024/11/28 11:06:25 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cub3d.h>

int	key_press(int keycode, t_data *data)
{
	if (!data)
		return (-1);
	if (keycode == KEY_UP || keycode == KEY_W)
		data->game.player.walkdirection = 1;
	else if (keycode == KEY_DOWN || keycode == KEY_S)
		data->game.player.walkdirection = -1;
	else if (keycode == KEY_LEFT || keycode == KEY_A)
		data->game.player.turndirection = -1;
	else if (keycode == KEY_RIGHT || keycode == KEY_D)
		data->game.player.turndirection = 1;
	else if (keycode == KEY_ESC)
		cleanup(data);
	else if (keycode == KEY_ACTIVE_MOUSSE)
		data->flage_mousse *= -1;
	return (0);
}

int	key_release(int keycode, t_data *data)
{
	if (!data)
		return (-1);
	if (keycode == KEY_UP || keycode == KEY_DOWN || keycode == KEY_W
		|| keycode == KEY_S)
		data->game.player.walkdirection = 0;
	else if (keycode == KEY_LEFT || keycode == KEY_RIGHT || keycode == KEY_A
		|| keycode == KEY_D)
		data->game.player.turndirection = 0;
	return (0);
}

int	mouse_move(int x, int y, t_data *data)
{
	int		delta_x;
	double	rotationangle;

	if (data->flage_mousse == -1)
		return (0);
	if (!data)
		return (-1);
	delta_x = x - data->game.last_mouse_x;
	data->game.player.rotationangle += delta_x * MOUSE_SENSITIVITY * (M_PI
			/ 180);
	rotationangle = normalizeangle(data->game.player.rotationangle);
	data->game.player.rotationangle = rotationangle;
	data->game.last_mouse_x = x;
	return (0);
}