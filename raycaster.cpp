#include "SDL.h"
#include <iostream>
#include <cmath>

#define WIDTH 1280
#define HEIGHT 640
#define PI M_PI
#define FOV 60.000
#define mapH 32
#define mapW 32

struct Player {
    float x;
    float y;
    int radius;
    float rotation;
    float moveSpeed;
    float rotationSpeed;
};

struct DeltaTime {
    uint32_t last_tick = 0;
    float delta = 0;

    void Calculate()
    {
        uint32_t tick = SDL_GetTicks();
        delta = tick - last_tick;
        last_tick = tick;
    }
};

int map[mapW][mapH] = { 
                {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,0,0,2,3,4,5,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1},
                {1,1,0,1,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1}, 
                {1,1,0,1,1,0,0,0,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1},
                {1,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0,0,0,1},
                {1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0,0,0,1},
                {1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1},
                {1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1},
                {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} };

bool running, fullscreen;
SDL_Rect tile, line;
SDL_Color wallCol,gridCol;
DeltaTime Delta;
SDL_Renderer* renderer;
SDL_Window* window;
int frameCount, timerFPS, lastFrame, fps;
Player player;

void init()
{
    //PLAYER

    player.x = 400;
    player.y = 100;
    player.radius = 16;
    player.rotation = 1.2;
    player.moveSpeed = 20;
    player.rotationSpeed = 8;

    //MAP

    tile.x = 0;
    tile.y = 0;
    tile.w = (WIDTH/2)/mapW;
    tile.h = HEIGHT/mapH;
    wallCol.r = 0;
    wallCol.g = 0;
    wallCol.b = 150;
    wallCol.a = 255;
    gridCol.r = 0;
    gridCol.g = 0;
    gridCol.b = 100;
    gridCol.a = 255;
}

void update()
{
    if (fullscreen) SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    if (!fullscreen) SDL_SetWindowFullscreen(window, 0);
}

float calcDist(float ax, float ay, float bx, float by, float angle )
{
    return(sqrt((bx-ax) * (bx-ax) + (by-ay) * (by-ay)));
}

void castRays()
{
    //sky
    line.h=HEIGHT/2;
    line.w=WIDTH/2;
    line.x=WIDTH/2;
    line.y=0;
    SDL_SetRenderDrawColor(renderer, 51, 171, 249, 255);
    SDL_RenderFillRect(renderer, &line);
   
    //floor
    line.h=HEIGHT/2;
    line.w=WIDTH/2;
    line.x=WIDTH/2;
    line.y=HEIGHT/2;
    SDL_SetRenderDrawColor(renderer, 88,57,39, 255);
    SDL_RenderFillRect(renderer, &line);

    int map_normalized_cord_x, map_normalized_cord_y, checkedDepth, viewDepth=32; 
    float ray_cord_x, ray_cord_y, ray_angle, offset_x, offset_y, shortest_hit_length;
    float resolution=0.0130899/2; //resolution coefficient calculated for 640x640 pseudo3D display

    //Casting Loop
    for (float beta = (FOV / 360 * 2 * PI / 2), count=0; beta > -(FOV / 360 * 2 * PI / 2); beta -= (resolution),count++)
    {
        int ray_amount=((FOV / 360 * 2 * PI / 2)/(resolution))*2;
        ray_angle=player.rotation+beta;
        
         //Angle normalization
        if (ray_angle <= 0)
        {
            ray_angle += 2 * PI;
        }
        else if (ray_angle >= 2 * PI)
        {
            ray_angle -= 2 * PI;
        }

        float arcTg=-1/tan(ray_angle);
        float negTg=-tan(ray_angle);
        checkedDepth=0;

 //Horizontal lines check section
        float horizon_ray_length=1000000, horizon_hit_x=player.x, horizon_hit_y=player.y;

        if (ray_angle > PI) //Looking down
        {
            ray_cord_y= (((int)(player.y/tile.w))*tile.w) + tile.w;
            ray_cord_x= (ray_cord_y - player.y) * arcTg + player.x;
            offset_y = tile.w;
            offset_x = offset_y * arcTg;
        }
        if (ray_angle < PI) //Looking up
        {
            ray_cord_y= (((int)(player.y/tile.w))*tile.w) - 0.0001;
            ray_cord_x= (ray_cord_y - player.y) * arcTg + player.x;
            offset_y = -tile.w;
            offset_x = offset_y * arcTg;
        }
        if (ray_angle == PI || ray_angle == 0 || ray_angle >= 2 * PI) //Orthogonal left or right (means no horizontal collision possible)
        {
            ray_cord_x=player.x;
            ray_cord_y=player.y;
            checkedDepth=viewDepth;
        }
        
            //Looking for horizontal ray collision
        
        while (checkedDepth < viewDepth) {
            map_normalized_cord_x = (int)(ray_cord_x/tile.w);
            map_normalized_cord_y = (int)(ray_cord_y/tile.w);
            int map_cords_multiplication = map_normalized_cord_x  * map_normalized_cord_y;
            

            if ((map_normalized_cord_x>=0&&map_normalized_cord_x<=(mapW-1))&&(map_normalized_cord_y>=0&&map_normalized_cord_y<=(mapH-1)))
            {
                if (map[map_normalized_cord_y][map_normalized_cord_x] > 0)
                {
                    horizon_hit_x=ray_cord_x;
                    horizon_hit_y=ray_cord_y;
                    horizon_ray_length=calcDist(player.x, player.y, horizon_hit_x, horizon_hit_y, ray_angle);
                    checkedDepth=viewDepth;
                }
                else
                {
                    ray_cord_x += offset_x;
                    ray_cord_y += offset_y;
                    checkedDepth++;
                }
            }
            else{   ray_cord_x += offset_x;
                    ray_cord_y += offset_y;
                    checkedDepth++;}
        }
       
       
 //Vertical lines check section
         checkedDepth=0;
        float vert_ray_length=1000000, vert_hit_x=player.x, vert_hit_y=player.y;

        if (ray_angle > PI/2 && ray_angle< 3*PI/2) //Looking down
        {
            ray_cord_x= (((int)(player.x/tile.w))*tile.w) - 0.0001;
            ray_cord_y= (ray_cord_x - player.x) * negTg + player.y;
            offset_x = -tile.w;
            offset_y = offset_x * negTg;
        }
        if (ray_angle < PI/2 || ray_angle > 3*PI/2) //Looking up
        {
            ray_cord_x= (((int)(player.x/tile.w))*tile.w) + tile.w;
            ray_cord_y= (ray_cord_x - player.x) * negTg + player.y;
            offset_x = tile.w;
            offset_y = offset_x * negTg;
        }
        if (ray_angle == PI/2 || ray_angle == 3*PI/2) //Orthogonal up or down (means no horizontal collision possible)
        {
            ray_cord_x=player.x;
            ray_cord_y=player.y;
            checkedDepth=viewDepth;
        }
        
            //Looking for vertical ray collision
        
        while (checkedDepth < viewDepth) {
            map_normalized_cord_x = (int)(ray_cord_x/tile.w);
            map_normalized_cord_y = (int)(ray_cord_y/tile.w);
            int map_cords_multiplication = map_normalized_cord_x  * map_normalized_cord_y;
            

            if ((map_normalized_cord_x>=0&&map_normalized_cord_x<=(mapW-1))&&(map_normalized_cord_y>=0&&map_normalized_cord_y<=(mapH-1)))
            {
                if (map[map_normalized_cord_y][map_normalized_cord_x] > 0)
                {
                    vert_hit_x=ray_cord_x;
                    vert_hit_y=ray_cord_y;
                    vert_ray_length=calcDist(player.x, player.y, vert_hit_x, vert_hit_y, ray_angle);
                    checkedDepth=viewDepth;
                }
                else
                {
                    ray_cord_x += offset_x;
                    ray_cord_y += offset_y;
                    checkedDepth++;
                }
            }
            else{   ray_cord_x += offset_x;
                    ray_cord_y += offset_y;
                    checkedDepth++;}
        }

         /*
            Checking which ray, horizontal or vertical, is shorter
        */
        
        if (vert_ray_length < horizon_ray_length)
        {
            ray_cord_x=vert_hit_x;
            ray_cord_y=vert_hit_y;
            shortest_hit_length=vert_ray_length;
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        }
        if(horizon_ray_length < vert_ray_length)
        {
            ray_cord_x=horizon_hit_x;
            ray_cord_y=horizon_hit_y;
            shortest_hit_length=horizon_ray_length;
            SDL_SetRenderDrawColor(renderer, 180, 0, 0, 255);
        }
        //Drawing ray
        SDL_RenderDrawLine(renderer, player.x, player.y, (int)ray_cord_x, (int)ray_cord_y);
        
        //Draw Pseudo3D display
        float fisheye_coefficent = player.rotation - ray_angle;
        if(fisheye_coefficent<0){fisheye_coefficent+=2*PI;}
        if(fisheye_coefficent>2*PI){fisheye_coefficent-=2*PI;}
        shortest_hit_length=shortest_hit_length*cos(fisheye_coefficent);
        float horizontal_line=(tile.w*HEIGHT)/shortest_hit_length;
        if(horizontal_line>HEIGHT){horizontal_line=HEIGHT;}
        float display_offset = (HEIGHT/2)-horizontal_line/2;
        
        int segment_width = ((WIDTH/2)/ray_amount);
        line.h=horizontal_line;
        line.w=segment_width;
        line.x=count*(segment_width)+WIDTH/2;
        line.y=display_offset;
        SDL_RenderFillRect(renderer, &line);
        
    }
        
    }

bool collisionDetectionX(float addX)
{
 int xcoord, ycoord;
    ycoord = player.y/tile.h;
    if (addX < 0) {
        xcoord = (player.x - player.radius + addX) / tile.w;
    }
    else
    {
        xcoord = (player.x + player.radius + addX) / tile.w;
    }
    int suppCordX = (player.x - tile.w) / tile.w;
    int suppCordY = (player.y + tile.w) / tile.w;
    int altsuppCordY = (player.y - tile.w) / tile.w;
    if (map[ycoord][xcoord] == 0)
    {
            return false;
        
    }
    else
    {
        if (addX > 0) {
            xcoord = (player.x/tile.w);
            player.x = ((xcoord*tile.w)- player.radius) + tile.w;
        };
        if (addX < 0) {
            xcoord = (player.x / tile.w);
            player.x = ((xcoord * tile.w)+ player.radius);
        };
        return true;
    }
}

bool collisionDetectionY(float addY)
{
    int xcoord, ycoord;
    xcoord = player.x/tile.w;
    if (addY < 0) {
        ycoord = (player.y - player.radius + addY) / tile.h;
    }
    else
    {
        ycoord = (player.y + player.radius + addY) / tile.h;
    }
    if (map[ycoord][xcoord] == 0)
    {
        return false;
    }
    else
    {
        if (addY > 0) {
            ycoord = (player.y / tile.h);
            player.y = ((ycoord * tile.w) - player.radius) + tile.w;
        };
        if (addY < 0) {
            ycoord = (player.y / tile.h);
            player.y = ((ycoord * tile.w) + player.radius);
        };
        return true;
    }
}

void input() 
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT) running = false;
    }
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    if (keystates[SDL_SCANCODE_ESCAPE]) running = false;
    if (keystates[SDL_SCANCODE_F11]) fullscreen = !fullscreen;
    if (keystates[SDL_SCANCODE_LEFT])//rotate counter clockwise
    {
        float rotation= player.rotationSpeed * Delta.delta / 1000;
        if (player.rotation+rotation >= 2 * PI) {
            player.rotation = 0;
        }
        else
        {
            player.rotation += rotation;
        }
    }
    if (keystates[SDL_SCANCODE_RIGHT])//rotate clockwise
    {
        float rotation=player.rotationSpeed* Delta.delta / 1000;
        if (player.rotation-rotation <= 0)
        {
            player.rotation = 2 * PI;
        }
        else
        {
            player.rotation -= rotation;
        }
    }//rotate clockwise
    if (keystates[SDL_SCANCODE_COMMA]) //Strafe left
    {
        float Xadd, Yadd;
        Xadd = Yadd = 0;
        float rotation = player.rotation;
        rotation -= PI / 2;
        Xadd -= (cos(rotation) * player.radius) * Delta.delta / 1000 * player.moveSpeed;
        Yadd += (sin(rotation) * player.radius) * Delta.delta / 1000 * player.moveSpeed;
        if (collisionDetectionX(Xadd) == false)
        {
            player.x += Xadd;
        }
        if (collisionDetectionY(Yadd) == false)
        {
            player.y += Yadd;
        }
        
    }
    if (keystates[SDL_SCANCODE_PERIOD]) //Strafe right
    {
        float Xadd, Yadd;
        Xadd = Yadd = 0;
        float rotation = player.rotation;
        rotation += PI / 2;
        Xadd -= (cos(rotation) * player.radius) * Delta.delta / 1000 * player.moveSpeed;
        Yadd += (sin(rotation) * player.radius) * Delta.delta / 1000 * player.moveSpeed;
        if (collisionDetectionX(Xadd) == false)
        {
            player.x += Xadd;
        }
        if (collisionDetectionY(Yadd) == false)
        {
            player.y += Yadd;
        }
    }
    if (keystates[SDL_SCANCODE_UP]) //Move forward ("with" rotation vector)
    {
        float Xadd, Yadd;
        Xadd = Yadd = 0;
        Xadd += (cos(player.rotation) * player.radius)*Delta.delta/1000* player.moveSpeed;
        Yadd -= (sin(player.rotation) * player.radius)*Delta.delta/1000* player.moveSpeed;
        if (collisionDetectionX(Xadd) == false)
        {
            player.x += Xadd;
        }
        if (collisionDetectionY(Yadd) == false)
        {
            player.y += Yadd;
        }
    }
    if (keystates[SDL_SCANCODE_DOWN]) //Move backward ("against" rotation vector)
    {
        float Xadd, Yadd;
        Xadd = Yadd = 0;
        Xadd -= (cos(player.rotation) * player.radius) * Delta.delta/1000 * player.moveSpeed;
        Yadd += (sin(player.rotation) * player.radius) * Delta.delta/1000 * player.moveSpeed;
        if (collisionDetectionX(Xadd) == false)
        {
            player.x += Xadd;
        }
        if (collisionDetectionY(Yadd) == false)
        {
            player.y += Yadd;
        }
    }

}

void drawPlayer(SDL_Renderer* renderer, int x, int y, int radius, float rotation)
{
    
    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w; 
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius))
            {
                SDL_SetRenderDrawColor(renderer, 220, 220, 0, 255);
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
        int xr = cos(rotation) * (radius);
        int yr = -sin(rotation) * (radius);
        SDL_SetRenderDrawColor(renderer, 230, 75, 0, 255);
        SDL_RenderDrawLine(renderer, x, y, x+xr, y+yr);
    }
}

void drawMap(SDL_Renderer* renderer, int map[mapW][mapH],int map_width, int map_height,SDL_Rect tile_param,SDL_Color wall_col, SDL_Color grid_col)
{

    for (int h = 0; h < map_width; h += tile_param.w)
    {

        for (int i = 0; i < map_height; i += tile_param.h)
        {
            if (map[h / tile.w][i / tile.w] > 0)
            {
                SDL_SetRenderDrawColor(renderer, wall_col.r, wall_col.g, wall_col.b, wall_col.a);
                SDL_RenderFillRect(renderer, &tile_param);

            }
            SDL_SetRenderDrawColor(renderer, grid_col.r, grid_col.g, grid_col.b, grid_col.a);
            SDL_RenderDrawRect(renderer, &tile_param);

            tile_param.x += tile_param.h;
        }
        tile_param.y += tile_param.w;
        tile_param.x = 0;

    }

}

void draw() 
{

    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 0);
    SDL_RenderFillRect(renderer,NULL);

    drawMap(renderer,map,(WIDTH/2),HEIGHT,tile,wallCol,gridCol);
    drawPlayer(renderer,player.x, player.y, player.radius,player.rotation);
    castRays();
    frameCount++;
    timerFPS = SDL_GetTicks() - lastFrame;
    if (timerFPS < (1000 / 60)) 
    {
        SDL_Delay((1000/60)-timerFPS);
    }
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[])
{
    running = 1;
    fullscreen = 0;
    static int lastTime = 0;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) std::cout << "SDL Init Error" << std::endl;
    if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) std::cout << "SDL Window Create Failed" << std::endl;
    SDL_SetWindowTitle(window, "RAYCASTER");
    SDL_ShowCursor(1);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    init();
    while (running) 
    {
       
        Delta.Calculate();
        lastFrame = SDL_GetTicks();
        if (lastFrame >= (lastTime + 1000))
        {
            lastTime = lastFrame;
            fps = frameCount;
            frameCount = 0;
        }
        std::cout << fps << std::endl;
        
        update();
        input();
        draw();
       

    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

  return 60;
}