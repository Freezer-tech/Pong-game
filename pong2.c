#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BALL_SIZE 15

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Rect paddle1, paddle2, ball;

int paddleSpeed = 0;
int paddle2Speed = 0;

int ballSpeedX = -1, ballSpeedY = -1;
int gameOver = 0;
int gameMode = 0; // 1 for Player vs Player, 2 for Player vs PC

/*
void showStartScreen() {
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    // Display start screen message
    // Note: You'll need to replace this with actual text rendering code
    printf("Press 1 for Player vs Player\n");
    printf("Press 2 for Player vs PC\n");

    // Wait for user to press a key
    SDL_Event e;
    while(SDL_WaitEvent(&e)) {
        if(e.type == SDL_KEYDOWN) {
            switch(e.key.keysym.sym) {
                case SDLK_1: 
                    gameMode = 1; 
                    return;
                case SDLK_2: 
                    gameMode = 2; 
                    return;
            }
        }
    }
}
*/
void showStartScreen() {
    // Initialize SDL_ttf
    if(TTF_Init() == -1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(2);
    }

    // Load a font
    TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 24);
    if(font == NULL) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        // handle error
    }

    // Create a surface with the text
    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Press 1 for Player vs Player, 2 for Player vs PC", textColor);
    
    // Create a texture from the surface
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    // Create a rect at the center of the screen
    SDL_Rect textRect;
    textRect.x = (SCREEN_WIDTH - textSurface->w) / 2;
    textRect.y = (SCREEN_HEIGHT - textSurface->h) / 2;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;

    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    // Copy the texture with the text to the renderer
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    // Update the screen
    SDL_RenderPresent(renderer);

    // Wait for user to press a key
    SDL_Event e;
    while(SDL_WaitEvent(&e)) {
        if(e.type == SDL_KEYDOWN) {
            switch(e.key.keysym.sym) {
                case SDLK_1: 
                    gameMode = 1; 
                    return;
                case SDLK_2: 
                    gameMode = 2; 
                    return;
                }
        }
    }

    // Clean up
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
}

void setup() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    paddle1.w = 15; paddle1.h = 80; paddle1.x = 20; paddle1.y = SCREEN_HEIGHT / 2 - paddle1.h / 2;
    paddle2.w = 15; paddle2.h = 80; paddle2.x = SCREEN_WIDTH - 20 - paddle2.w; paddle2.y = SCREEN_HEIGHT / 2 - paddle2.h / 2;
    ball.w = BALL_SIZE; ball.h = BALL_SIZE; ball.x = SCREEN_WIDTH / 2 - ball.w / 2; ball.y = SCREEN_HEIGHT / 2 - ball.h / 2;
}

void draw() {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderFillRect(renderer, &paddle1);
    SDL_RenderFillRect(renderer, &paddle2);
    //SDL_RenderFillRect(renderer, &ball);

    int radius = ball.w / 2;
    int centerX = ball.x + radius;
    int centerY = ball.y + radius;

    for (int x = -radius; x <= radius; x++) {
        int height = sqrt(radius * radius - x * x);
        for (int y = -height; y <= height; y++) {
            SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
        }
    }
    
    SDL_RenderPresent(renderer);
}

void input() {
    SDL_Event e;
    while(SDL_PollEvent(&e) != 0) {
        if(e.type == SDL_QUIT) {
            gameOver = 1;
        }
        else if(e.type == SDL_KEYDOWN) {
            switch(e.key.keysym.sym) {
                case SDLK_UP: paddleSpeed = -2; break;
                case SDLK_DOWN: paddleSpeed = 2; break;
                case SDLK_w: paddle2Speed = -2; break; // W for Player 2 up
                case SDLK_s: paddle2Speed = 2; break; // S for Player 2 down
                case SDLK_ESCAPE: gameOver = 1; break; // ESC to quit the game
            }
        }
        else if(e.type == SDL_KEYUP) {
            switch(e.key.keysym.sym) {
                case SDLK_UP: 
                case SDLK_DOWN: paddleSpeed = 0; break;
                case SDLK_w:
                case SDLK_s: paddle2Speed = 0; break; // Stop Player 2 when W or S is released
            }
        }
    }
}

void logic() {
    // Update paddle1 position based on user input
    paddle1.y += paddleSpeed;
    if(paddle1.y < 0) { paddle1.y = 0; }
    if(paddle1.y > SCREEN_HEIGHT - paddle1.h) { paddle1.y = SCREEN_HEIGHT - paddle1.h; }

    // Update paddle2 position based on user input or AI
    if(gameMode == 1) {
        // In Player vs Player mode, update paddle2 position based on user input
        paddle2.y += paddle2Speed;
        if(paddle2.y < 0) { paddle2.y = 0; }
        if(paddle2.y > SCREEN_HEIGHT - paddle2.h) { paddle2.y = SCREEN_HEIGHT - paddle2.h; }
    } else if(gameMode == 2) {
        // In Player vs PC mode, move the computer's paddle towards the ball
        if(ball.y < paddle2.y) { paddle2.y -= 2; }
        else if(ball.y > paddle2.y + paddle2.h) { paddle2.y += 2; }
    }

    // Update ball position
    ball.x += ballSpeedX; ball.y += ballSpeedY;

    // If the ball hits the left edge of the screen, end the game
    if(ball.x < 0) { gameOver = 1; return; }
    if(ball.x > SCREEN_WIDTH - BALL_SIZE) { gameOver = 1; }

    // If the ball hits the top or bottom edge of the screen, reverse its direction
    if(ball.y < 0 || ball.y > SCREEN_HEIGHT - BALL_SIZE) { ballSpeedY *= -1; }

    // If the ball hits a paddle, reverse its direction
    if((ball.x < paddle1.x + paddle1.w && 
        ball.x + BALL_SIZE > paddle1.x && 
        ball.y < paddle1.y + paddle1.h && 
        BALL_SIZE + ball.y > paddle1.y) ||
       (ball.x < paddle2.x + paddle2.w && 
        ball.x + BALL_SIZE > paddle2.x && 
        ball.y < paddle2.y + paddle2.h && 
        BALL_SIZE + ball.y > paddle2.y)) {
        ballSpeedX *= -1;
    }
}

void endGame() {
    SDL_DestroyWindow(window);
    window = NULL;

    SDL_Quit();
}

int main(int argc, char* args[]) {
    setup();
    showStartScreen();

    while(!gameOver) {
        draw();
        input();
        logic();
        SDL_Delay(5);
    }

    endGame();

    return 0;
}