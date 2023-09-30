#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BALL_SIZE 15

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Rect paddle1, paddle2, ball;
SDL_Rect scoreLine;

int paddleSpeed = 0;
int paddle2Speed = 0;

int player1Score = 0; 
int player2Score = 0; 
int cpuScore = 0;    

int ballSpeedX = -1, ballSpeedY = -1;
int gameOver = 0;
int gameMode = 0; // 1 for Player vs Player, 2 for Player vs PC

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

    // Create surfaces for each line of text
    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Surface* textSurface1 = TTF_RenderText_Solid(font, "Press", textColor);
    SDL_Surface* textSurface2 = TTF_RenderText_Solid(font, "1. Player vs Player", textColor);
    SDL_Surface* textSurface3 = TTF_RenderText_Solid(font, "2. Player vs CPU", textColor);
    SDL_Surface* textSurface4 = TTF_RenderText_Solid(font, "3. ESC to exit", textColor);

    // Create textures from the surfaces
    SDL_Texture* textTexture1 = SDL_CreateTextureFromSurface(renderer, textSurface1);
    SDL_Texture* textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);
    SDL_Texture* textTexture3 = SDL_CreateTextureFromSurface(renderer, textSurface3);
    SDL_Texture* textTexture4 = SDL_CreateTextureFromSurface(renderer, textSurface4);

    // Create rects for each line of text
    SDL_Rect textRect1, textRect2, textRect3, textRect4;
    textRect1.x = (SCREEN_WIDTH - textSurface1->w - textSurface2->w) / 2;
    textRect2.x = textRect1.x;
    textRect3.x = textRect1.x;
    textRect4.x = textRect1.x;

    int totalTextHeight = textSurface1->h + textSurface2->h + textSurface3->h + textSurface4->h;
    int startY = (SCREEN_HEIGHT - totalTextHeight) / 2;
    textRect1.y = startY;
    textRect2.y = textRect1.y + textSurface1->h;
    textRect3.y = textRect2.y + textSurface2->h;
    textRect4.y = textRect3.y + textSurface3->h;

    textRect1.w = textSurface1->w;
    textRect1.h = textSurface1->h;
    textRect2.w = textSurface2->w;
    textRect2.h = textSurface2->h;
    textRect3.w = textSurface3->w;
    textRect3.h = textSurface3->h;
    textRect4.w = textSurface4->w;
    textRect4.h = textSurface4->h;

    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    // Copy the textures with the text to the renderer
    SDL_RenderCopy(renderer, textTexture1, NULL, &textRect1);
    SDL_RenderCopy(renderer, textTexture2, NULL, &textRect2);
    SDL_RenderCopy(renderer, textTexture3, NULL, &textRect3);
    SDL_RenderCopy(renderer, textTexture4, NULL, &textRect4);

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
                case SDLK_ESCAPE:
                    exit(0);
            }
        }
    }

    // Clean up
    SDL_FreeSurface(textSurface1);
    SDL_FreeSurface(textSurface2);
    SDL_FreeSurface(textSurface3);
    SDL_FreeSurface(textSurface4);
    SDL_DestroyTexture(textTexture1);
    SDL_DestroyTexture(textTexture2);
    SDL_DestroyTexture(textTexture3);
    SDL_DestroyTexture(textTexture4);
    TTF_CloseFont(font);
}

int setup() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    
    window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    paddle1.w = 15; paddle1.h = 80; paddle1.x = 20; paddle1.y = SCREEN_HEIGHT / 2 - paddle1.h / 2;
    paddle2.w = 15; paddle2.h = 80; paddle2.x = SCREEN_WIDTH - 20 - paddle2.w; paddle2.y = SCREEN_HEIGHT / 2 - paddle2.h / 2;
    ball.w = BALL_SIZE; ball.h = BALL_SIZE; ball.x = SCREEN_WIDTH / 2 - ball.w / 2; ball.y = SCREEN_HEIGHT / 2 - ball.h / 2;
    scoreLine.x = 0; scoreLine.y = 30; scoreLine.w = SCREEN_WIDTH; scoreLine.h = 2;

    return 0;
}

void draw() {
    // Initialize SDL_ttf
    if(TTF_Init() == -1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(2);
    }

    // Load a font
    TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 18);
    if(font == NULL) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        // handle error
    }

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderFillRect(renderer, &paddle1);
    SDL_RenderFillRect(renderer, &paddle2);
    SDL_RenderFillRect(renderer, &scoreLine);

    int radius = ball.w / 2;
    int centerX = ball.x + radius;
    int centerY = ball.y + radius;

    for (int x = -radius; x <= radius; x++) {
        int height = sqrt(radius * radius - x * x);
        for (int y = -height; y <= height; y++) {
            SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
        }
    }
    /*
    // Draw "Player 1" and "Player 2" text based on gameMode
    if (gameMode == 1) {
        // Player vs Player mode, show both players
        SDL_Color textColor = {0, 0, 0, 255};
        SDL_Surface* textSurface1 = TTF_RenderText_Solid(font, "Player 1", textColor);
        SDL_Surface* textSurface2 = TTF_RenderText_Solid(font, "Player 2", textColor);
        SDL_Surface* textSurface3 = TTF_RenderText_Solid(font, "Score", textColor);
        SDL_Texture* textTexture1 = SDL_CreateTextureFromSurface(renderer, textSurface1);
        SDL_Texture* textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);
        SDL_Texture* textTexture3 = SDL_CreateTextureFromSurface(renderer, textSurface3);
        SDL_Rect textRect1;
        SDL_Rect textRect2;
        SDL_Rect textRect3;
        textRect1.x = 10;  // Posizione X per Player 1
        textRect1.y = 0;   // Posizione Y per Player 1
        textRect1.w = textSurface1->w;
        textRect1.h = textSurface1->h;
        textRect2.x = 550;  // Posizione X per Player 2
        textRect2.y = 0;    // Posizione Y per Player 2
        textRect2.w = textSurface2->w;
        textRect2.h = textSurface2->h;
        textRect3.x = 300;  // Posizione X di Score
        textRect3.y = 0;   // Posizione Y di Score
        textRect3.w = textSurface3->w;
        textRect3.h = textSurface3->h;

        SDL_RenderCopy(renderer, textTexture1, NULL, &textRect1);
        SDL_RenderCopy(renderer, textTexture2, NULL, &textRect2);
        SDL_RenderCopy(renderer, textTexture3, NULL, &textRect3);
    } 
    else if (gameMode == 2) {
        // Player vs CPU mode, show Player 1 and CPU
        SDL_Color textColor = {0, 0, 0, 255};
        SDL_Surface* textSurface1 = TTF_RenderText_Solid(font, "Player 1", textColor);
        SDL_Surface* textSurface2 = TTF_RenderText_Solid(font, "CPU", textColor);
        SDL_Surface* textSurface3 = TTF_RenderText_Solid(font, "Score", textColor);
        SDL_Texture* textTexture1 = SDL_CreateTextureFromSurface(renderer, textSurface1);
        SDL_Texture* textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);
        SDL_Texture* textTexture3 = SDL_CreateTextureFromSurface(renderer, textSurface3);
        SDL_Rect textRect1;
        SDL_Rect textRect2;
        SDL_Rect textRect3;
        textRect1.x = 10;  // Posizione X per Player 1
        textRect1.y = 0;   // Posizione Y per Player 1
        textRect1.w = textSurface1->w;
        textRect1.h = textSurface1->h;
        textRect2.x = 550;  // Posizione X per CPU
        textRect2.y = 0;    // Posizione Y per CPU
        textRect2.w = textSurface2->w;
        textRect2.h = textSurface2->h;
        textRect3.x = 300;  // Posizione X di Score
        textRect3.y = 0;   // Posizione Y di Score
        textRect3.w = textSurface3->w;
        textRect3.h = textSurface3->h;

        SDL_RenderCopy(renderer, textTexture1, NULL, &textRect1);
        SDL_RenderCopy(renderer, textTexture2, NULL, &textRect2);
        SDL_RenderCopy(renderer, textTexture3, NULL, &textRect3);
}
*/
    // Draw the score directly
    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Surface* scoreSurface = NULL;
    SDL_Texture* scoreTexture = NULL;
    SDL_Rect scoreRect;

    // Render Player 1 score
    char player1ScoreText[32];
    snprintf(player1ScoreText, sizeof(player1ScoreText), "Player 1: %d", player1Score);
    scoreSurface = TTF_RenderText_Solid(font, player1ScoreText, textColor);
    scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    scoreRect.x = 10; // X position for Player 1 score
    scoreRect.y = 0; // Y position for Player 1 score
    scoreRect.w = scoreSurface->w;
    scoreRect.h = scoreSurface->h;
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);

    // Render Player 2 score or CPU score based on game mode
    char player2ScoreText[32];
    if (gameMode == 1) {
        snprintf(player2ScoreText, sizeof(player2ScoreText), "Player 2: %d", player2Score);
    } else {
        snprintf(player2ScoreText, sizeof(player2ScoreText), "CPU: %d", cpuScore);
    }
    scoreSurface = TTF_RenderText_Solid(font, player2ScoreText, textColor);
    scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    scoreRect.x = SCREEN_WIDTH - scoreSurface->w - 10; // X position for Player 2 or CPU score
    scoreRect.y = 0; // Y position for Player 2 or CPU score
    scoreRect.w = scoreSurface->w;
    scoreRect.h = scoreSurface->h;
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);

    // Free surfaces and textures
    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);
        
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

void resetBall() {
    ball.x = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
    ball.y = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
    // Set the initial ball speed (you can customize this part)
    ballSpeedX = -1;  // Initial direction
    ballSpeedY = -1;  // Initial direction
}

void logic() {
    // Update paddle1 position based on user input
    paddle1.y += paddleSpeed;
    // Checks whether paddle1 has reached or exceeded the scoreLine
    if (paddle1.y < scoreLine.y + scoreLine.h) {  
        paddle1.y = scoreLine.y + scoreLine.h;  // Set the position of paddle1 just above the scoreLine
    }
    if (paddle1.y < 0) { paddle1.y = 0; }
    if (paddle1.y > SCREEN_HEIGHT - paddle1.h) { paddle1.y = SCREEN_HEIGHT - paddle1.h; }

    // Update paddle2 position based on user input or AI
    if (gameMode == 1) {
    // In Player vs Player mode, update paddle2 position based on user input
        paddle2.y += paddle2Speed;
    // Check whether paddle2 has reached or exceeded the scoreLine
    if (paddle2.y < scoreLine.y + scoreLine.h) {  
        paddle2.y = scoreLine.y + scoreLine.h;  // Set the position of paddle2 just above the scoreLine
    }
    if (paddle2.y < 0) { paddle2.y = 0; }
    if (paddle2.y > SCREEN_HEIGHT - paddle2.h) { paddle2.y = SCREEN_HEIGHT - paddle2.h; }
        } else if (gameMode == 2) {
    // In Player vs PC mode, move the computer's paddle towards the ball
    if (ball.y < paddle2.y) { paddle2.y -= 2; }
    else if (ball.y > paddle2.y + paddle2.h) { paddle2.y += 2; }
}

    /*
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
    */

    // Update ball position
    ball.x += ballSpeedX; ball.y += ballSpeedY;
    
    /*
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
    */

    // Check if the ball hits the left edge of the screen
    if (ball.x < 0) {
        // Increment the score for the appropriate player (based on gameMode)
        if (gameMode == 1) {
            player2Score++; // In Player vs Player mode or Player vs CPU mode
            }
        else if (gameMode == 2) {
            cpuScore++;
        }
        resetBall();    // Reset the ball to the center
    } 
    // Check if the ball hits the right edge of the screen
    else if (ball.x > SCREEN_WIDTH - BALL_SIZE) {
        if (gameMode == 1) {
            // In Player vs Player mode, increment Player 1's score
            player1Score++;
        } 
        else if (gameMode == 2) {
            // In Player vs CPU mode, increment CPU's score
            //cpuScore++;
            player1Score++;
        }
    resetBall(); // Reset the ball to the center
    }
    // The ball hit the top or bottom edge, change the vertical direction
    else if (ball.y < 0 || ball.y > SCREEN_HEIGHT - BALL_SIZE) { ballSpeedY *= -1; } 
    // The ball hit the paddle1, change the horizontal direction
    else if (ball.x < paddle1.x + paddle1.w && ball.x + BALL_SIZE > paddle1.x &&
        ball.y < paddle1.y + paddle1.h && BALL_SIZE + ball.y > paddle1.y) {ballSpeedX *= -1; } 
    // The ball hit the paddle2, change the horizontal direction
    else if (ball.x < paddle2.x + paddle2.w && ball.x + BALL_SIZE > paddle2.x &&
        ball.y < paddle2.y + paddle2.h && BALL_SIZE + ball.y > paddle2.y) {ballSpeedX *= -1;  } 
    // The ball hit the scoreLine, change the vertical direction
    else if (ball.y < scoreLine.y + scoreLine.h && ball.y + BALL_SIZE > scoreLine.y) { ballSpeedY *= -1; } 
}

void endGame() {
    SDL_DestroyWindow(window);
    window = NULL;

    SDL_Quit();
}

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char* args[]) {
    setup();
    resetBall();
    showStartScreen();
    // Check Windows subsystem only when necessary
    #ifdef _WIN32
    FreeConsole();
    #endif
    
    while(!gameOver) {
        draw();
        input();
        logic();
        SDL_Delay(5);
    }

    endGame();

    return 0;
}

// Check Windows subsystem only when necessary
#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return main(__argc, __argv);
}
#endif
