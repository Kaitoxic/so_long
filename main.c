#include "minilibx-linux/mlx.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "get_next_line.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define Escape 0xff1b
#define wback_sprite "./sprites/char/elf_back_idle/elf_back_idle.xpm"
#define wfront_sprite "./sprites/char/elf_front_idle/elf_front_idle.xpm"
#define wleft_sprite "./sprites/char/elf_side02_idle/elf_side02_idle.xpm"
#define wright_sprite "./sprites/char/elf_side01_idle/elf_side01_idle.xpm"
#define FLOOR "./sprites/floor_brown.xpm"
#define COINS "./sprites/coins.xpm"
#define LAVA "./sprites/lava.xpm"

typedef struct s_img_data
{
	void	*img_ptr;
	char	*addr;
	int		bitsperpxl;
	int		line_length;
	int		endian;
}	t_img_data;

typedef struct s_player
{
	int	x;
	int	y;
	int coins;
}	t_player;

typedef struct s_mlx_data
{
	void		*mlx_ptr;
	void		*win_ptr;
	char		**map_array;
	t_player	player;
	t_img_data	img;
}	t_mlx_data;

int	set_spawn_position(t_mlx_data *data, int x, int y)
{
	mlx_put_image_to_window(data->mlx_ptr, data->win_ptr, data->img.img_ptr, x, y);
	return (1);
}

void	put_img(t_mlx_data data, int x, int y)
{
	mlx_put_image_to_window(data.mlx_ptr, data.win_ptr, data.img.img_ptr, x, y);
}

int	load_images(t_mlx_data *data, char *img)
{
	data->img.img_ptr = mlx_xpm_file_to_image(data->mlx_ptr, img,
			&data->img.line_length, &data->img.line_length);
	if (!data->img.img_ptr)
	{
		printf("Error\n");
		mlx_destroy_window(data->mlx_ptr, data->win_ptr);
		mlx_destroy_display(data->mlx_ptr);
		free(data->mlx_ptr);
		exit(-1);
	}
	data->img.addr = mlx_get_data_addr(data->img.img_ptr,
			&data->img.bitsperpxl, &data->img.line_length, &data->img.endian);
	return (1);
}

char	**generate_map(char *map, char **map_array)
{
	int		fd;
	char	line;
	int 	x;
	int 	y;

	x = 0;
	y = 1; // 1 to avoid segfault (count the first line)
	fd = open(map, 0);
	if (fd < 0)
	{
		printf("Error\n");
		return (NULL);
	}
	while (read(fd, &line, 1)) // Refaire avec GNL
	{
		if (line == '\n')
		{
			x = 0;
			y++;
		}
		else
			x++;
	}
	close(fd);
	map_array = (char **)malloc(sizeof(char *) * (y + 1));
	if (!map_array)
	{
		printf("Error\n");
		return (NULL);
	}
	for (int i = 0; i < y; i++)
	{
		map_array[i] = (char *)malloc(sizeof(char) * (x + 1));
		if (!map_array[i])
		{
			printf("Error\n");
			return (NULL);
		}
	}
	printf("Map array generated %d %d\n", x, y);
	return (map_array);
}

char	**complete_map(char *map, t_mlx_data *data)
{
	int		fd;
	char	*line;
	int 	x;
	int 	y;
	int		i;

	x = 0;
	y = 0;
	data->map_array = generate_map(map, data->map_array);
	printf("Map generated\n");
	fd = open(map, 0);
	line = get_next_line(fd);
	while (line)
	{
		i = 0;
		while (line[i])
		{
			if (line[i] == '\n')
			{
				x = 0;
				printf("\n");
				y++;
			}
			else
			{
				data->map_array[y][x] = line[i];
				printf("%c", data->map_array[y][x]);
				x++;
			}
			i++;
		}
		free(line);
		data->map_array[y][x] = '\0';
		line = get_next_line(fd);
	}
	data->map_array[y + 1] = 0;
	printf("Map completed\n");
	close(fd);
	return (data->map_array);
}

t_player	read_map(char *map, t_mlx_data *data) // refaire la lecture de la map via le tableau
{
	int		fd;
	char	line;
	int		x;
	int		y;

	x = 0;
	y = 0;
	fd = open(map, 0);
	if (fd < 0)
	{
		printf("Error\n");
		return (data->player);
	}
	while (read(fd, &line, 1))
	{
		if (line == '1')
		{
			load_images(data, LAVA);
			put_img(*data, x, y);
			x += 32;
		}
		else if (line == '0')
		{
			load_images(data, FLOOR);
			put_img(*data, x, y);
			x += 32;
		}
		else if (line == 'P')
		{
			data->player.x = x;
			data->player.y = y;
			load_images(data, wleft_sprite);
			set_spawn_position(data, x, y);
			put_img(*data, x, y);
			x += 32;
		}
		else if (line == 'E')
		{
			load_images(data, wfront_sprite);
			put_img(*data, x, y);
			x += 32;
		}
		else if (line == 'C')
		{
			load_images(data, COINS);
			put_img(*data, x + 7, y + 7);
			x += 32;
		}
		else if (line == '\n')
		{
			x = 0;
			y += 32;
		}
		else
			printf("Error\n");
	}
	close(fd);
	return (data->player);
}

void	drop_sprite(t_mlx_data *data, int x, int y)
{
	if (data->map_array[y / 32][x / 32] == '0')
	{
		load_images(data, FLOOR);
		put_img(*data, x, y);
	}
	else if (data->map_array[y / 32][x / 32] == '1')
	{
		load_images(data, LAVA);
		put_img(*data, x, y);
	}
	else if (data->map_array[y / 32][x / 32] == 'E')
	{
		load_images(data, wfront_sprite);
		put_img(*data, x, y);
	}
	else if (data->map_array[y / 32][x / 32] == 'C')
	{
		load_images(data, LAVA);
		put_img(*data, x + 7, y + 7);
	}
}

int	check_all_collected(t_mlx_data *data)
{
	int		fd;
	char	line;
	int		x;
	int		y;

	y = 0;
	while (data->map_array[y] != 0)
	{
		x = 0;
		while (data->map_array[y][x])
		{
			printf("Map array: %c\n", data->map_array[y][x]);
			if (data->map_array[y][x] == 'C')
				return (0);
			x++;
		}
		y++;
	}
	return (1);
}

int	check_borders(t_mlx_data *data, int keycode)
{
	if (data->map_array[data->player.y / 32][data->player.x / 32] == '1')
	{
		printf("Error\n");
		return (0);
	}
	else if (data->map_array[data->player.y / 32][data->player.x / 32] == 'E' && data->player.coins == 1)
	{
		printf("You win\n");
		exit(0);
	}
	else if (data->map_array[data->player.y / 32][data->player.x / 32] == 'C')
	{
		data->map_array[data->player.y / 32][data->player.x / 32] = '0';
		printf("You collected a coin\n");
		if (check_all_collected(data) == 1)
			data->player.coins = 1;
	}
	if (data->map_array[data->player.y / 32 + 1][data->player.x / 32] == '1'  && keycode == 0x0073)
	{
		printf("Degage\n");
		return (0);
	}
	else if (data->map_array[data->player.y / 32 - 1][data->player.x / 32] == '1' && keycode == 0x0077)
	{
		printf("Degage\n");
		return (0);
	}
	else if (data->map_array[data->player.y / 32][data->player.x / 32 + 1] == '1' && keycode == 0x0064)
	{
		printf("Degage\n");
		return (0);
	}
	else if (data->map_array[data->player.y / 32][data->player.x / 32 - 1] == '1' && keycode == 0x0061)
	{
		printf("Degage\n");
		return (0);
	}
	return (1);
}

int	handle_input(int keycode, t_mlx_data *data)
{
	printf("Keycode: %d\n", keycode);
	if (keycode == Escape)
	{
		mlx_destroy_window(data->mlx_ptr, data->win_ptr);
		mlx_destroy_display(data->mlx_ptr);
		free(data->mlx_ptr);
		exit(1);
	}
	else if (keycode == 0x0077) // w
	{
		if (check_borders(data, keycode) == 1)
		{
			drop_sprite(data, data->player.x, data->player.y);
			data->player.y -= 32;
			load_images(data, wback_sprite);
			put_img(*data, data->player.x, data->player.y);
		}
	}
	else if (keycode == 0x0073) // s
	{
		if (check_borders(data, keycode) == 1)
		{
			drop_sprite(data, data->player.x, data->player.y);
			data->player.y += 32;
			load_images(data, wfront_sprite);
			put_img(*data, data->player.x, data->player.y);
		}
	}
	else if (keycode == 0x0061) // a
	{
		if (check_borders(data, keycode) == 1)
		{
			drop_sprite(data, data->player.x, data->player.y);
			data->player.x -= 32;
			load_images(data, wleft_sprite);
			put_img(*data, data->player.x, data->player.y);
		}
	}
	else if (keycode == 0x0064) // d
	{
		//mlx_clear_window(data->mlx_ptr, data->win_ptr);
		if (check_borders(data, keycode) == 1)
		{
			drop_sprite(data, data->player.x, data->player.y);
			data->player.x += 32;
			printf("Player position: %d %d map : %c\n", data->player.x, data->player.y, data->map_array[data->player.y / 32][data->player.x / 32]);
			load_images(data, wright_sprite);
			put_img(*data, data->player.x, data->player.y);
		}
	}
	return (0);
}

int main()
{
	t_mlx_data	data;
	void		*img;

	data.mlx_ptr = mlx_init();
	if (!data.mlx_ptr)
		return (-1);
	data.win_ptr = mlx_new_window(data.mlx_ptr, WIN_WIDTH, WIN_HEIGHT, "So Long");
	if (!data.win_ptr)
	{
		mlx_destroy_display(data.mlx_ptr);
		free(data.mlx_ptr);
		return (-1);
	}
	complete_map("map.ber", &data);
	read_map("map.ber", &data); // mettre la map en argument
	printf("Player position: %d %d\n", data.player.x, data.player.y);
	mlx_key_hook(data.win_ptr, handle_input, &data);
	mlx_loop(data.mlx_ptr);
	mlx_destroy_window(data.mlx_ptr, data.win_ptr);
	mlx_destroy_display(data.mlx_ptr);
	free(data.mlx_ptr);
	return (0);
}