#include "SDL.h"
#include <iostream>
#include <cmath>

#define WIDTH 1280
#define HEIGHT 640

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
    player.moveSpeed = 30;
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
    if (map[xcoord][ycoord] == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool collisionDetectionY(float addY)
{
    int xcoord, ycoord;
    xcoord = player.x/tile.w;
   // ycoord = (player.y + addY) / tile.h;
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
    if (keystates[SDL_SCANCODE_LEFT]) player.rotation-=player.rotationSpeed*Delta.delta/1000; //rotate counter clockwise
    if (keystates[SDL_SCANCODE_RIGHT]) player.rotation += player.rotationSpeed * Delta.delta / 1000; //rotate clockwise
    if (keystates[SDL_SCANCODE_COMMA]) //Strafe left
    {
        float Xadd, Yadd;
        Xadd = Yadd = 0;
        float rotation = player.rotation;
        rotation -= M_PI / 2;
        Xadd += (cos(rotation) * player.radius) * Delta.delta / 1000 * player.moveSpeed;
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
        rotation += M_PI / 2;
        Xadd += (cos(rotation) * player.radius) * Delta.delta / 1000 * player.moveSpeed;
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
        Yadd += (sin(player.rotation) * player.radius)*Delta.delta/1000* player.moveSpeed;
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
        Yadd -= (sin(player.rotation) * player.radius) * Delta.delta/1000 * player.moveSpeed;
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
        int yr = sin(rotation) * (radius);
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
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) std::cout << "B��d inicjalizacji SDLa" << std::endl;
    if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) std::cout << "B��d tworzenia okna" << std::endl;
    SDL_SetWindowTitle(window, "RAYCASTER");
    SDL_ShowCursor(1);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    init();
    while (running) 
    {
       
        Delta.Calculate();
        std::cout << Delta.delta<<std::endl;
        
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