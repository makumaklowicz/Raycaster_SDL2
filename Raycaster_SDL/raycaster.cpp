#include "SDL.h"
#include <iostream>
#include <cmath>

#define WIDTH 1280
#define HEIGHT 720

struct Player {
    int x;
    int y;
    int radius;
  
};

int map[8][8] = { 
                {1,1,1,1,1,1,1,1},
                {1,1,1,0,0,0,0,1},
                {1,0,1,0,0,0,0,1},
                {1,0,1,1,0,0,0,1},
                {1,0,0,0,0,1,1,1},
                {1,0,0,0,1,1,0,1},
                {1,1,0,0,0,0,0,1},
                {1,1,1,1,1,1,1,1,} };


bool running, fullscreen;

SDL_Renderer* renderer;
SDL_Window* window;

int frameCount, timerFPS, lastFrame, fps;
Player player;

void init()
{
    player.x = 80;
    player.y = 80;
    player.radius = 16;
}

void update()
{
    if (fullscreen) SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    if (!fullscreen) SDL_SetWindowFullscreen(window, 0);
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
    if (keystates[SDL_SCANCODE_LEFT]) player.x-=4;
    if (keystates[SDL_SCANCODE_RIGHT]) player.x += 4;
    if (keystates[SDL_SCANCODE_UP]) player.y -= 4;
    if (keystates[SDL_SCANCODE_DOWN]) player.y += 4;
}

void drawPlayer(SDL_Renderer* renderer, int x, int y, int radius)
{
    
    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w; 
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius))
            {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawLine(renderer, x, y, x, (y - radius+((radius/10))));
    }
}



void draw() 
{

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer,NULL);
    SDL_Rect rect;



    rect.x = 0;
    rect.y = 0;
    rect.w = 80;
    rect.h = 80;
    for (int h=0;h < 640;h+=80)
    {
       
        for (int i = 0; i < 640; i += 80)
        {
            if (map[i / 80][h / 80] == 1)
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderFillRect(renderer, &rect);
               
            }
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);
            
            rect.y += 80;
        }
        rect.x += 80;
        rect.y= 0;
       
    }
    

    drawPlayer(renderer,player.x, player.y, player.radius);

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
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) std::cout << "B³¹d inicjalizacji SDLa" << std::endl;
    if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) std::cout << "B³¹d tworzenia okna" << std::endl;
    SDL_SetWindowTitle(window, "RAYCASTER");
    SDL_ShowCursor(1);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    init();
    while (running) 
    {
        
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