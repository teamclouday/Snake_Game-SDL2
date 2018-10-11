#ifndef GAME_H
#define GAME_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "Linked.h"

#define SNAKE_WIDTH 20
#define SNAKE_HEIGHT 20

#define moveUp 1
#define moveDown 2
#define moveLeft 3
#define moveRight 4

#define moveEveryStep 2

typedef struct{
    int length;
    struct Node *head;
} Snake;

typedef struct{
    int w, h;
    int x, y;
    // the special fruit that will reduce the length of the snake
    int isSpecial;
} Fruit;

typedef struct{
    SDL_Renderer *renderer;
    SDL_Window *window;
    Snake snake;
    Fruit fruit;
    int dead;
    int stepsCount;
    int score;
} GameState;



int ProcessEvent(SDL_Window *window, GameState *game);
void loadGame(GameState *game);
void doRenderer(SDL_Renderer *renderer, GameState *game);
void Collision(GameState *game);
void setFruit(Fruit *fruit);
void moveBody(GameState *game);
int correctXY(int num);
void restartGame(GameState *game);
void drawScore(SDL_Renderer *renderer, GameState *game);

#endif