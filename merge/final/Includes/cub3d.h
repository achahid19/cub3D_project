#ifndef CUB3D_H
# define CUB3D_H

# include "../lib/libft/libft.h"
#include "parsing.h"
# include <fcntl.h>
# include <errno.h>
#include "../mlx/mlx.h"
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <time.h>
# include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


#define MOUSE_SENSITIVITY 1

# define COLOR_RED      0xFF0000
# define COLOR_GREEN    0x00FF00
# define COLOR_BLUE     0x0000FF
# define COLOR_YELLOW   0xFFFF00
# define COLOR_CYAN     0x00FFFF
# define COLOR_MAGENTA  0xFF00FF
# define COLOR_ORANGE   0xFFA500
# define COLOR_PURPLE   0x800080
# define COLOR_WHITE    0xFFFFFF
# define COLOR_BLACK    0x000000
# define COLOR_GRAY     0x808080
# define COLOR_PINK     0xFFC0CB

#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 780
#define NUM_RAYS WINDOW_WIDTH
#define FOV_ANGLE (60 * (M_PI / 180))
// #define TILE_SIZE 256

#define COLLISION_MARGIN 5

//  #define KEY_UP 126
// #define KEY_DOWN 125
// #define KEY_LEFT 123
// #define KEY_RIGHT 124
// #define KEY_ESC 53 

#define KEY_W 119
#define KEY_S 115
#define KEY_A 97
#define KEY_D 100

#define KEY_ACTIVE_MOUSSE 65436

#define CIEL_IMAGE TRUE
#define DARKNESS 500
#define RADIUS 20

#define TRUE 1
#define FALSE 0

#define CIEL "texters/100.xpm"

typedef struct s_img
{
    void    *img_ptr;
    char    *image_pixel_ptr;
    int     bits_per_pixel;
    int     endian;
    int     line_len;
} t_img;

typedef struct s_ray {
    char c;
    float   rayAngle;
    float   wallHitX;
    float   wallHitY;
    float   distance;
    int     wasHitVertical;
    int     isRayFacingUp;
    int     isRayFacingDown;
    int     isRayFacingLeft;
    int     isRayFacingRight;
    int     wallHitContent;
} t_ray;

typedef struct s_data {
	int flage_mousse;
    t_cub3d     game;
    void        *mlx;
    void        *win;
    t_img       img;
    t_ray       rays[NUM_RAYS];
} t_data;

 #define KEY_UP 65362
 #define KEY_DOWN 65364
 #define KEY_LEFT 65361
 #define KEY_RIGHT 65363
 #define KEY_ESC 65307

# endif
