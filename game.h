#ifndef GAME_H
#define GAME_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "Linked.h"

#include <stdlib.h>
#include <string.h>

#define SNAKE_WIDTH 20
#define SNAKE_HEIGHT 20

#define moveUp 1
#define moveDown 2
#define moveLeft 3
#define moveRight 4

#define moveEveryStep 2

#define SPF (1000 / 60)

#ifndef MY_ROOT_DIR
#define MY_ROOT_DIR "."
#endif

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
    TTF_Font *font;
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
void freeList(struct Node* head);
void timer(Uint32 *prev, Uint32 *now);

#endif