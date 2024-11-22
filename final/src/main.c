#include "../Includes/cub3d.h"

char    **ft_split(char const *s, char c);
void    cleanup_map(t_map *map);
int     close_window(t_data *data);
char    *get_next_line(int fd);
char    *ft_strjoin(char *s1, char *s2);

double normalizeAngle(double angle)
{
    angle = fmod(angle, 2 * M_PI);
    if (angle < 0)
        angle += 2 * M_PI;
    return angle;
}

void safe_free(void **ptr)
{
    if (ptr && *ptr)
    {
        free(*ptr);
        *ptr = NULL;
    }
}

void cleanup_map(t_map *map)
{
    if (!map || !map->grid)
        return;

    for (int i = 0; map->grid[i]; i++)
        safe_free((void**)&map->grid[i]);
    safe_free((void**)&map->grid);
}

int close_window(t_data *data)
{
    if (!data)
        return -1;

    if (data->map)
        cleanup_map(data->map);
    if (data->win)
        mlx_destroy_window(data->mlx, data->win);
    if (data->img.img_ptr)
        mlx_destroy_image(data->mlx, data->img.img_ptr);
    if (data->player)
        free(data->player);
    exit(0);
}

void my_pixel_put(t_img *img, int x, int y, int color)
{
    if (x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT)
    {
        int offset = (y * img->line_len) + (x * (img->bits_per_pixel / 8));
        *((unsigned int *)(img->image_pixel_ptr + offset)) = color;
    }
}

int create_trgb(int t, int r, int g, int b)
{
    return (t << 24) | (r << 16) | (g << 8) | b;
}

double distanceBetweenPoints(double x1, double y1, double x2, double y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

int has_wall_at(t_map *map, double x, double y)
{
    if (!map || !map->grid || x < 0 || x >= map->map_width * map->tile_size 
        || y < 0 || y >= map->map_height * map->tile_size)
        return 1;

    int mapX = (int)(x / map->tile_size);
    int mapY = (int)(y / map->tile_size);

    return (map->grid[mapY][mapX] == '1');
}

void get_map_resolution(t_map *map)
{
    if (!map || !map->grid)
        return;

    map->map_height = 0;
    map->map_width = 0;
    for (int i = 0; map->grid[i] != NULL; i++)
    {
        map->map_height++;
        int row_length = ft_strlen(map->grid[i]);
        if (row_length > map->map_width)
            map->map_width = row_length;
    }
}

int unit_circle(float angle, char c)
{
    if (c == 'x')
    {
        if (angle > 0 && angle < M_PI)
            return (1);
    }
    else if (c == 'y')
    {
        if (angle > (M_PI / 2) && angle < (3 * M_PI) / 2)
            return (1);
    }
    return (0);
}

int inter_check(t_data *data, float angle, float *inter, float *step, int is_horizon)
{
    if (is_horizon)
    {
        if (angle > 0 && angle < M_PI)
        {
            *inter += data->map->tile_size;
            return (-1);
        }
        *step *= -1;
    }
    else
    {
        if (!(angle > M_PI / 2 && angle < 3 * M_PI / 2)) 
        {
            *inter += data->map->tile_size;
            return (-1);
        }
        *step *= -1;
    }
    return (1);
}

int wall_hit(float x, float y, t_data *data)
{
    int x_m, y_m;

    if (x < 0 || x >= data->map->map_width * TILE_SIZE || y < 0 || y >= data->map->map_height * TILE_SIZE)
        return 0; 
    x_m = floor(x / data->map->tile_size);
    y_m = floor(y / data->map->tile_size);


    if (y_m >= data->map->map_height || x_m >= data->map->map_width)
        return (0);

    if (data->map->grid[y_m] && x_m < (int)ft_strlen(data->map->grid[y_m]))
        if (data->map->grid[y_m][x_m] == '1')
            return (0);

    return (1);
}

float get_h_inter(t_data *data, float angle)
{
    float h_x, h_y, x_step, y_step;
    int pixel;

    y_step = data->map->tile_size;
    x_step = data->map->tile_size / tan(angle);
    h_y = floor(data->player->y / data->map->tile_size) * data->map->tile_size;
    pixel = inter_check(data, angle, &h_y, &y_step, 1);
    h_x = data->player->x + (h_y - data->player->y) / tan(angle);

    if ((unit_circle(angle, 'y') && x_step > 0) || (!unit_circle(angle, 'y') && x_step < 0))
        x_step *= -1;

    while (wall_hit(h_x, h_y - pixel, data))
    {
        h_x += x_step;
        h_y += y_step;
    }

    return sqrt(pow(h_x - data->player->x, 2) + pow(h_y - data->player->y, 2));
}

float get_v_inter(t_data *data, float angle)
{
    float v_x, v_y, x_step, y_step;
    int pixel;

    x_step = data->map->tile_size; 
    y_step = data->map->tile_size * tan(angle);
    v_x = floor(data->player->x / data->map->tile_size) * data->map->tile_size;
    pixel = inter_check(data, angle, &v_x, &x_step, 0);
    v_y = data->player->y + (v_x - data->player->x) * tan(angle);

    if ((unit_circle(angle, 'x') && y_step < 0) || (!unit_circle(angle, 'x') && y_step > 0))
        y_step *= -1;

    while (wall_hit(v_x - pixel, v_y, data))
    {
        v_x += x_step;
        v_y += y_step;
    }

    return sqrt(pow(v_x - data->player->x, 2) + pow(v_y - data->player->y, 2));
}

void castRay(t_data *data, float rayAngle, int stripId)
{
    float h_inter, v_inter;
    rayAngle = normalizeAngle(rayAngle);

    h_inter = get_h_inter(data, rayAngle);
    v_inter = get_v_inter(data, rayAngle);

    if (v_inter <= h_inter)
    {
        if (data->rays->rayAngle > FOV_ANGLE / 2)
            data->rays->c = 'W';
        else
            data->rays->c = 'N';
        data->rays[stripId].distance = v_inter;
        data->rays[stripId].wasHitVertical = TRUE;
    }
    else
    {
        data->rays[stripId].distance = h_inter;
        data->rays[stripId].wasHitVertical = FALSE;
    }

    data->rays[stripId].rayAngle = rayAngle;
}

int my_mlx_pixel_get(int flage,t_data *data, double wallX, int y, int wallHeight) {

    int tex_x = (int)(wallX * data->texter[flage].texter_with);
    int tex_y = (int)((y * data->texter[flage].texter_height) / wallHeight);
    
    tex_x = tex_x % data->texter[flage].texter_with;
    tex_y = tex_y % data->texter[flage].texter_height;
    
    if (!data || tex_x < 0 || tex_y < 0 || 
        tex_x >= WINDOW_HEIGHT || 
        tex_y >= WINDOW_WIDTH)     
    {         
        return 0;
    }           

    char *pixel_ptr = data->texter[flage].image_pixel_ptr + 
                     (tex_y * data->texter[flage].line_len + 
                      tex_x * (data->texter[flage].bits_per_pixel / 8));

    return *(int*)pixel_ptr;
}




void drawing_east(t_data *data){
    for (int x = 0; x < WINDOW_WIDTH; x++) {
        for (int y = 0; y < WINDOW_HEIGHT / 2; y++) {
            my_pixel_put(&data->img, x, y, create_trgb(0, 100, 100, 100));
        }
    }
}

void drawing_floor(t_data *data){
        for (int x = 0; x < WINDOW_WIDTH; x++) {
        for (int y = WINDOW_HEIGHT / 2; y < WINDOW_HEIGHT; y++) {
            my_pixel_put(&data->img, x, y, create_trgb(0, 50, 50, 50));
        }
    }
}


void render_walls(t_data *data) {
    int texture_id;
    int wallHeight;
    int wallTop;
    int wallBottom;
    int texColor;
    float darkness;

    for (int i = 0; i < NUM_RAYS; i++) {
        float perpDistance = data->rays[i].distance * 
                           cos(data->rays[i].rayAngle - data->player->rotationAngle);
        
        wallHeight = (int)((WINDOW_HEIGHT / perpDistance) * data->map->tile_size);
        
        wallTop = (WINDOW_HEIGHT / 2) - (wallHeight / 2);
        
        wallBottom = (WINDOW_HEIGHT / 2) + (wallHeight / 2);
        if (wallBottom >= WINDOW_HEIGHT) wallBottom = WINDOW_HEIGHT - 1;

        double rayAngle = normalizeAngle(data->rays[i].rayAngle);
        
        if (data->rays[i].wasHitVertical) {
            if (cos(rayAngle) > 0) {
                texture_id = 3; 
            } else {
                texture_id = 2; 
            }
        } else {
            if (sin(rayAngle) > 0) {
                texture_id = 0;
            } else {
                texture_id = 1; 
            }
        }

        double wallX;
        if (data->rays[i].wasHitVertical) {
            wallX = fmod(data->player->y + data->rays[i].distance * 
                        sin(data->rays[i].rayAngle), data->map->tile_size) / data->map->tile_size;
        } else {
            wallX = fmod(data->player->x + data->rays[i].distance * 
                        cos(data->rays[i].rayAngle), data->map->tile_size) / data->map->tile_size;
        }

        for (int y = wallTop; y < wallBottom; y++) {
            texColor = my_mlx_pixel_get(texture_id, data, wallX, y - wallTop, wallHeight);
            
            darkness = 1.0f + (perpDistance / (data->map->tile_size * 5));
            
            int r = ((texColor >> 16) & 0xFF) / darkness;
            int g = ((texColor >> 8) & 0xFF) / darkness;
            int b = (texColor & 0xFF) / darkness;
            
            r = (r < 0) ? 0 : (r > 255) ? 255 : r;
            g = (g < 0) ? 0 : (g > 255) ? 255 : g;
            b = (b < 0) ? 0 : (b > 255) ? 255 : b;

            int finalColor = create_trgb(0, r, g, b);
            my_pixel_put(&data->img, i, y, finalColor);
        }
    }
    mlx_put_image_to_window(data->mlx, data->win, data->img.img_ptr, 0, 0);
}
void castAllRays(t_data *data) {
    float rayAngle = data->player->rotationAngle - (FOV_ANGLE / 2);
    for (int stripId = 0; stripId < NUM_RAYS; stripId++) {
        castRay(data, rayAngle, stripId);
        rayAngle += FOV_ANGLE / NUM_RAYS;
    }
}

int update_player(t_player *player, t_map *map)
{
    if (!player || !map)
        return -1;

    double moveStep;
    double newPlayerX;
    double newPlayerY;
    player->rotationAngle = normalizeAngle(player->rotationAngle + 
                           player->turnDirection * player->rotationSpeed);

    moveStep = player->walkDirection * player->moveSpeed;
    newPlayerX = player->x + cos(player->rotationAngle) * moveStep;
    newPlayerY = player->y + sin(player->rotationAngle) * moveStep;

    if (!has_wall_at(map, newPlayerX, newPlayerY))
    {
        player->x = newPlayerX;
        player->y = newPlayerY;
    }
    return 0;
}

int key_press(int keycode, t_data *data)
{
    if (!data || !data->player)
        return -1;

    if (keycode == KEY_UP)
        data->player->walkDirection = 1;
    else if (keycode == KEY_DOWN)
        data->player->walkDirection = -1;
    else if (keycode == KEY_LEFT)
        data->player->turnDirection = -1;
    else if (keycode == KEY_RIGHT)
        data->player->turnDirection = 1;
    else if (keycode == KEY_ESC)
        close_window(data);
    return 0;
}

int key_release(int keycode, t_data *data)
{
    if (!data || !data->player)
        return -1;

    if (keycode == KEY_UP || keycode == KEY_DOWN)
        data->player->walkDirection = 0;
    else if (keycode == KEY_LEFT || keycode == KEY_RIGHT)
        data->player->turnDirection = 0;
    return 0;
}

int read_map(t_data *data, char *map_path)
{
    int fd = open(map_path, O_RDONLY);
    if (fd == -1)
        return 0;

    char *line;
    char *content = NULL;
    char *temp;

    while ((line = get_next_line(fd)))
    {
        temp = ft_strjoin(content, line);
        free(line);
        content = temp;
        if (!content)
            break;
    }

    if (!content)
    {
        close(fd);
        return 0;
    }

    data->map->grid = ft_split(content, '\n');
    free(content);
    close(fd);

    if (!data->map->grid)
        return 0;

    get_map_resolution(data->map);
    data->map->tile_size = TILE_SIZE;
    return 1;
}

int game_loop(t_data *data) {
    update_player(data->player, data->map);
    mlx_clear_window(data->mlx, data->win);
    castAllRays(data);
    memset(data->img.image_pixel_ptr, 0,  WINDOW_WIDTH * WINDOW_HEIGHT * (data->img.bits_per_pixel / 8));
    drawing_east(data);
    drawing_floor(data);
    render_walls(data);
    return 0;
}

void init_player(t_data *data){
    int startX;
    int startY;

    data->player->radius = 3;
    data->player->turnDirection = 0;
    data->player->walkDirection = 0;
    data->player->rotationAngle = M_PI / 2;
    data->player->moveSpeed = 5;
    data->player->rotationSpeed = 2 * (M_PI / 180);

    startX = data->map->map_width * data->map->tile_size / 2;
    startY = data->map->map_height * data->map->tile_size / 2;

    data->player->x = startX;
    data->player->y = startY;
    if (has_wall_at(data->map, data->player->x, data->player->y))
    {
        data->player->x = startX - data->map->tile_size;
        data->player->y = startY - data->map->tile_size;
    }

}


void init_textures(t_data *data){
    data->texter[0].img_ptr = mlx_xpm_file_to_image(data->mlx, NO, &data->texter[0].texter_with, &data->texter[0].texter_height);

    data->texter[0].image_pixel_ptr = mlx_get_data_addr(data->texter[0].img_ptr, 
                                              &data->texter[0].bits_per_pixel,
                                              &data->texter[0].line_len,
                                              &data->texter[0].endian);


    data->texter[1].img_ptr = mlx_xpm_file_to_image(data->mlx, VO, &data->texter[1].texter_with, &data->texter[1].texter_height);

    data->texter[1].image_pixel_ptr = mlx_get_data_addr(data->texter[1].img_ptr, 
                                              &data->texter[1].bits_per_pixel,
                                              &data->texter[1].line_len,
                                              &data->texter[1].endian);
    
    data->texter[2].img_ptr = mlx_xpm_file_to_image(data->mlx, WO, &data->texter[2].texter_with, &data->texter[2].texter_height);

    data->texter[2].image_pixel_ptr = mlx_get_data_addr(data->texter[2].img_ptr, 
                                              &data->texter[2].bits_per_pixel,
                                              &data->texter[2].line_len,
                                              &data->texter[2].endian);
    data->texter[3].img_ptr = mlx_xpm_file_to_image(data->mlx, EO, &data->texter[3].texter_with, &data->texter[3].texter_height);

    data->texter[3].image_pixel_ptr = mlx_get_data_addr(data->texter[1].img_ptr, 
                                              &data->texter[3].bits_per_pixel,
                                              &data->texter[3].line_len,
                                              &data->texter[3].endian);
}

void init_game(t_data *data)
{
    if (!data || !data->map)
        return;

    data->mlx = mlx_init();
    if (!data->mlx)
        return;

    data->win = mlx_new_window(data->mlx, WINDOW_WIDTH, WINDOW_HEIGHT, "Raycasting");
    if (!data->win)
        return;

    data->img.img_ptr = mlx_new_image(data->mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!data->img.img_ptr)
        return;

    data->img.image_pixel_ptr = mlx_get_data_addr(data->img.img_ptr, 
                                              &data->img.bits_per_pixel,
                                              &data->img.line_len,
                                              &data->img.endian);
}


bool init(t_data *data, char *argv){
    data->player = malloc(sizeof(t_player));
    if (!data->player)
        return 0;
    if (!read_map(data, argv))
    {
        fprintf(stderr, "Error: Failed to read map file\n");
        return 0;
    }
    init_game(data);
    init_player(data);
    init_textures(data);
    if (!data->mlx || !data->win || !data->player)
    {
        cleanup_map(data->map);
        if (data->mlx) {
            if (data->win) mlx_destroy_window(data->mlx, data->win);
            if (data->player) free(data->player);
        }
        fprintf(stderr, "Error: Failed to initialize game\n");
        return 0;
    }
    return(1);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <map_file>\n", argv[0]);
        return 1;
    }

    t_data data;
    t_map map;

    memset(&data, 0, sizeof(t_data));
    memset(&map, 0, sizeof(t_map));
    data.map = &map;

    if (!init(&data, argv[1]))
        return 1;

    mlx_loop_hook(data.mlx, game_loop, &data);
    mlx_hook(data.win, 2, 1L<<0, key_press, &data);
    mlx_hook(data.win, 3, 1L<<1, key_release, &data);
    mlx_hook(data.win, 17, 0, close_window, &data);
    mlx_loop(data.mlx);

    cleanup_map(&map);
    free(data.player);
    mlx_destroy_image(data.mlx, data.img.img_ptr);
    mlx_destroy_window(data.mlx, data.win);

    return 0;
}
