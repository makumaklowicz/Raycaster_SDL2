#include "SDL.h"
#include <iostream>
#include <cmath>

#define WIDTH 1280
#define HEIGHT 640
#define PI 3.14159
#define FOV 90.000

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

SDL_Rect tile;
SDL_Color wallCol,gridCol;
DeltaTime Delta;

int map[8][8] = { 
                {1,1,1,1,1,1,1,1},
                {1,1,1,0,0,0,0,1},
                {1,0,1,0,0,0,0,1},
                {1,0,1,1,0,0,0,1},
                {1,0,0,0,0,1,1,1},
                {1,0,0,0,1,1,0,1},
                {1,1,0,0,0,0,0,1},
                {1,1,1,1,1,1,1,1} };


bool running, fullscreen;
int viewDepth=8;


SDL_Renderer* renderer;
SDL_Window* window;

int frameCount, timerFPS, lastFrame, fps;
Player player;

void init()
{
    //PLAYER

    player.x = 400;
    player.y = 200;
    player.radius = 16;
    player.rotation = 0;
    player.moveSpeed = 20;
    player.rotationSpeed = 8;

    //MAP

    tile.x = 0;
    tile.y = 0;
    tile.w = 80;
    tile.h = 80;
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

void castRays()
{
    int xRayLength, yRayLength;
    xRayLength = yRayLength = 0;
    int xRayCord=0, yRayCord = 0;
    int xc = player.x / 80;
    int yc = player.y / 80;
    int xfloat=0;
    int yfloat=0;
    int rayl = 0, raylV = 10000, raylH = 10000, raylFull = 0, y0 = 0, x0 = 0;
    float ctg,tg;
    int depth = 0;
    bool piMulH = false, piMulV = false;
    float alpha = 0;
   
    

    //Horizontal check
    for (float beta = -(FOV / 360 * 2 * PI / 2); beta < (FOV / 360 * 2 * PI / 2); beta += 0.1)
    {
      
        alpha = beta;
        if (player.rotation+alpha <= 0)
        {
            alpha += 2 * PI;
        }
        else if (player.rotation+alpha >= 2 * PI)
        {
            alpha -= 2 * PI;
        }

        xRayLength = yRayLength = 0;
        xRayCord = 0, yRayCord = 0;
        xc = player.x / 80;
        yc = player.y / 80;
        xfloat = 0;
        yfloat = 0;
        rayl = 0, raylV = 10000, raylH = 10000, raylFull = 0, y0 = 0, x0 = 0;
        ctg, tg;
        depth = 0;
        piMulH = false, piMulV = false;


        if (player.rotation+alpha > PI)
        {
            ctg = 1 / tan(player.rotation+alpha - PI);
            yfloat = 80 - ((player.y / 80 - yc) * 80);
            xfloat = yfloat * ctg;
            raylH = sqrt(xfloat * xfloat + yfloat * yfloat);
            y0 = 80;
            x0 = y0 * ctg;
        }
        if (player.rotation+alpha < PI)
        {
            ctg = 1 / tan(PI - player.rotation+alpha);
            yfloat = (player.y / 80 - yc) * 80;
            xfloat = yfloat * ctg;
            raylH = sqrt(xfloat * xfloat + yfloat * yfloat);
            y0 = -80;
            x0 = y0 * ctg;
        }
        if (player.rotation+alpha > PI - 0.00001 && player.rotation+alpha < PI + 0.00001)
        {

            raylH = ((player.x / 80 - xc) * 80);
            y0 = 0;
            x0 = 80;
            piMulH = true;

        }
        if ((player.rotation+alpha > -0.00001 && player.rotation+alpha < 0.00001) || (player.rotation+alpha < 2 * PI + 0.004 && player.rotation+alpha > 2 * PI - 0.004))
        {

            raylH = 80 - (player.x / 80 - xc) * 80;
            y0 = 0;
            x0 = 80;
            piMulH = true;
        }
        while (depth < viewDepth && piMulH == false) {
            xRayCord = (player.x + (cos(player.rotation+alpha) * raylH)) / 80;
            if (player.rotation+alpha > PI) yRayCord = ((player.y + (-sin(player.rotation+alpha) * raylH)) / 80) + 1;
            if (player.rotation+alpha < PI) yRayCord = ((player.y + (-sin(player.rotation+alpha) * raylH)) / 80) - 1;
            if (yRayCord * xRayCord >= 0 && yRayCord * xRayCord <= 64)
            {
                if (map[xRayCord][yRayCord] == 0)
                {
                    raylH += sqrt(y0 * y0 + x0 * x0);
                }
                else
                {
                    break;
                }
            }
            depth++;
        }

        while (depth < viewDepth && piMulH == true)
        {
            yRayCord = (player.y / 80);
            if ((player.rotation+alpha > -0.00001 && player.rotation+alpha < 0.00001) || (player.rotation+alpha < 2 * PI && player.rotation+alpha > 2 * PI - 0.00002))xRayCord = (player.x + raylH - 1) / 80 + 1;
            if (player.rotation+alpha > PI - 0.00001 && player.rotation+alpha < PI + 0.00001)xRayCord = (player.x - raylH + 1) / 80 - 1;

            if (map[xRayCord][yRayCord] == 0)
            {
                raylH += x0;
            }
            else
            {
                break;
            }
            depth++;
            raylV = 10000;
        }

        piMulH == false;
        depth = 0;

        //Vertical check
        if (player.rotation+alpha > 3 * PI / 2 && player.rotation+alpha < 2 * PI - 0.004 || player.rotation+alpha < PI / 2 && player.rotation+alpha >0)
        {

            tg = tan(player.rotation+alpha);
            xfloat = 80 - ((player.x / 80 - xc) * 80);
            yfloat = xfloat * tg;
            raylV = sqrt(xfloat * xfloat + yfloat * yfloat);
            x0 = -80;
            y0 = x0 * tg;
        }

        if (player.rotation+alpha < 3 * PI / 2 && player.rotation+alpha>PI / 2)
        {
            tg = tan(player.rotation+alpha);
            xfloat = (player.x / 80 - xc) * 80;
            yfloat = xfloat * tg;
            raylV = sqrt(xfloat * xfloat + yfloat * yfloat);
            x0 = 80;
            y0 = x0 * tg;

        }

        if (player.rotation+alpha < (PI / 2) + 0.001 && player.rotation+alpha >(PI / 2) - 0.001)
        {
            raylV = (player.y / 80 - yc) * 80;
            x0 = 0;
            y0 = 80;
            piMulV = true;
        }

        if (player.rotation+alpha < (3 * PI / 2) + 0.001 && player.rotation+alpha >(3 * PI / 2) - 0.001)
        {
            raylV = 80 - ((player.y / 80 - yc) * 80);
            x0 = 0;
            y0 = 80;
            piMulV = true;
        }

        while (depth < viewDepth && piMulV == false) {
            yRayCord = ((player.y + (-sin(player.rotation+alpha) * raylV)) / 80);
            if (player.rotation+alpha > 3 * PI / 2 && player.rotation+alpha < 2 * PI || player.rotation+alpha < PI / 2 && player.rotation+alpha >0) xRayCord = ((player.x + (cos(player.rotation+alpha) * raylV)) / 80) + 1;
            if (player.rotation+alpha < 3 * PI / 2 && player.rotation+alpha>PI / 2) xRayCord = ((player.x + (cos(player.rotation+alpha) * raylV)) / 80) - 1;

            if (yRayCord * xRayCord >= 0 && yRayCord * xRayCord <= 64)
            {
                if (map[xRayCord][yRayCord] == 0)
                {
                    raylV += sqrt(y0 * y0 + x0 * x0);
                }
                else
                {
                    break;
                }
            }
            depth++;

        }

        while (depth < viewDepth && piMulV == true)
        {

            xRayCord = (player.x / 80);
            if (player.rotation+alpha < (PI / 2) + 0.001 && player.rotation+alpha >(PI / 2) - 0.001) yRayCord = (player.y - raylV + 1) / 80 - 1;
            if (player.rotation+alpha < (3 * PI / 2) + 0.001 && player.rotation+alpha >(3 * PI / 2) - 0.001) yRayCord = ((player.y + raylV - 1) / 80 + 1);
            if (map[xRayCord][yRayCord] == 0)
            {
                raylV += y0;
            }
            else
            {
                break;
            }
            depth++;
            raylH = 10000;

        }

        if (raylV > raylH)
        {
            rayl = raylH;
        }
        else
        {
            rayl = raylV;
        }

        xRayLength = cos(player.rotation+alpha) * rayl;
        yRayLength = -sin(player.rotation+alpha) * rayl;
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderDrawLine(renderer, player.x, player.y, player.x + xRayLength, player.y + yRayLength);
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
    int suppCordX = (player.x - 80) / 80;
    int suppCordY = (player.y + 80) / 80;
    int altsuppCordY = (player.y - 80) / 80;
    if (map[xcoord][ycoord] == 0)
    {
            return false;
        
    }
    else
    {
        if (addX > 0) {
            xcoord = (player.x/tile.w);
            player.x = ((xcoord*80)- player.radius) + tile.w;
        };
        if (addX < 0) {
            xcoord = (player.x / tile.w);
            player.x = ((xcoord * 80)+ player.radius);
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
    if (map[xcoord][ycoord] == 0)
    {
        return false;
    }
    else
    {
        if (addY > 0) {
            ycoord = (player.y / tile.h);
            player.y = ((ycoord * 80) - player.radius) + tile.w;
        };
        if (addY < 0) {
            ycoord = (player.y / tile.h);
            player.y = ((ycoord * 80) + player.radius);
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
            player.rotation = 2 * PI-0.00001;
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

void drawMap(SDL_Renderer* renderer, int map[8][8],int map_width, int map_height,SDL_Rect tile_param,SDL_Color wall_col, SDL_Color grid_col)
{

    for (int h = 0; h < map_width; h += tile_param.w)
    {

        for (int i = 0; i < map_height; i += tile_param.h)
        {
            if (map[h / 80][i / 80] == 1)
            {
                SDL_SetRenderDrawColor(renderer, wall_col.r, wall_col.g, wall_col.b, wall_col.a);
                SDL_RenderFillRect(renderer, &tile_param);

            }
            SDL_SetRenderDrawColor(renderer, grid_col.r, grid_col.g, grid_col.b, grid_col.a);
            SDL_RenderDrawRect(renderer, &tile_param);

            tile_param.y += tile_param.h;
        }
        tile_param.x += tile_param.w;
        tile_param.y = 0;

    }

}



void draw() 
{

    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 0);
    SDL_RenderFillRect(renderer,NULL);

    drawMap(renderer,map,640,640,tile,wallCol,gridCol);
    drawPlayer(renderer,player.x, player.y, player.radius,player.rotation);
    castRays();
    std::cout << player.rotation << std::endl;

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
        //std::cout << Delta.delta<<std::endl;
        
        lastFrame = SDL_GetTicks();
        if (lastFrame >= (lastTime + 1000))
        {
            lastTime = lastFrame;
            fps = frameCount;
            frameCount = 0;
        }
        //std::cout << fps << std::endl;
        
        update();
        input();
        draw();
       

    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

  return 60;
}