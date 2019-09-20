#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"

int main(int argc, char *argv[]){
    GameState game;
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    // random number
    srand(time(NULL));

    // start sdl
    SDL_Init(SDL_INIT_VIDEO);

    // start ttf
    TTF_Init();

    // create window
    window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              800, 600, 0);

    // initialize renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    game.renderer = renderer;
    game.window = window;

    // hide mouse
    SDL_ShowCursor(0);

    // process initial
    loadGame(&game);

    char *fontPath = malloc(strlen(MY_ROOT_DIR) + strlen("/fonts/Crazy-Pixel.ttf") + 1);
    strcpy(fontPath, MY_ROOT_DIR);
    strcat(fontPath, "/fonts/Crazy-Pixel.ttf");
    game.font = TTF_OpenFont(fontPath, 48);
    if(game.font == NULL){
        printf("Cannot find the font file!\n");
        exit(1);
    }
    free(fontPath);

    // game loop
    int done = 0;

    Uint32 tNow = SDL_GetTicks();
    Uint32 tPrev = SDL_GetTicks();
    while(!done){
        //deal with events
        done = ProcessEvent(window, &game);
        // move snake body
        moveBody(&game);
        //detect collision
        Collision(&game);
        // render it
        doRenderer(renderer, &game);
        // delay
        timer(&tPrev, &tNow);
    }

    TTF_CloseFont(game.font);

    // destroy window and renderer
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    TTF_Quit();

    SDL_Quit();
    return 0;
}



// process event
int ProcessEvent(SDL_Window *window, GameState *game){
    SDL_Event event;
    int done = 0;

    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_WINDOWEVENT_CLOSE:
                if(window){
                    window = NULL;
                    done = 1;
                }
            break;
            case SDL_KEYDOWN:
            {
                switch(event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        done = 1;
                    break;
                }
            }
            break;
            case SDL_QUIT:
                done = 1;
            break;
        }
    }

    if(game->stepsCount == 0){
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if(state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W]){
            if(game->snake.head->movement != moveDown){
                game->snake.head->movement = moveUp;
            }
        }
        else if(state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S]){
            if(game->snake.head->movement != moveUp){
                game->snake.head->movement = moveDown;
            }
        }
        else if(state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A]){
            if(game->snake.head->movement != moveRight){
                game->snake.head->movement = moveLeft;
            }
        }
        else if(state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D]){
            if(game->snake.head->movement != moveLeft){
                game->snake.head->movement = moveRight;
            }
        }
    }

    return done;
}


void loadGame(GameState *game){
    game->dead = 0;
    int centerVal[2] = {400, 300};
    int centerValDown[2] = {400, 320};
    game->snake.length = 2;
    game->snake.head = initialize(centerValDown, moveUp, NULL);
    game->snake.head = pushNode(game->snake.head, centerVal, moveUp);
    // not moving
    game->stepsCount = 0;
    // no score
    game->score = 0;
    setFruit(&game->fruit);
}


void doRenderer(SDL_Renderer *renderer, GameState *game){
    // draw background to grey
    SDL_SetRenderDrawColor(renderer, 116, 149, 201, 255);
    SDL_RenderClear(renderer);
    // draw head
    SDL_SetRenderDrawColor(renderer, 191, 70, 43, 255);
    struct Node *body = game->snake.head;
    SDL_Rect theHead = {body->value[0], body->value[1], SNAKE_WIDTH, SNAKE_HEIGHT};
    SDL_RenderFillRect(renderer, &theHead);
    body = body->Child;
    // change color to draw snake body
    SDL_SetRenderDrawColor(renderer, 0, 47, 124, 255);
    // draw every part of body
    while(body != NULL){
        SDL_Rect rect = {body->value[0], body->value[1], SNAKE_WIDTH, SNAKE_HEIGHT};
        SDL_RenderFillRect(renderer, &rect);
        body = body->Child;
    }
    // draw fruit
    if(!game->fruit.isSpecial){
        SDL_SetRenderDrawColor(renderer, 79, 198, 157, 255);
    }
    else{
        SDL_SetRenderDrawColor(renderer, 249, 140, 77, 255);
    }
    SDL_Rect food = {game->fruit.x, game->fruit.y, game->fruit.w, game->fruit.h};
    SDL_RenderFillRect(renderer, &food);
    // draw score
    drawScore(renderer, game);
    // show present renderer
    SDL_RenderPresent(renderer);
}

// function that move snake after key events are handled
void moveBody(GameState *game){
    if(!game->dead){
        game->stepsCount++;
        struct Node *body = game->snake.head;
        while(body != NULL){
            switch(body->movement){
                case moveUp:
                    body->value[1] -= moveEveryStep;
                break;
                case moveDown:
                    body->value[1] += moveEveryStep;
                break;
                case moveLeft:
                    body->value[0] -= moveEveryStep;
                break;
                case moveRight:
                    body->value[0] += moveEveryStep;
                break;
            }
            body = body->Child;
        }
        // if move on grid
        if(game->stepsCount == (SNAKE_WIDTH / moveEveryStep)){
            game->stepsCount = 0;
            // update movements of every parts
            struct Node *updateIt = game->snake.head->Child;
            // init an array to save the movements
            int moveArray[2] = {game->snake.head->movement, 0};
            while(updateIt != NULL){
                moveArray[1] = moveArray[0];
                moveArray[0] = updateIt->movement;
                //printf("%d, %d\n", moveArray[0], moveArray[1]);
                updateIt->movement = moveArray[1];
                updateIt = updateIt->Child;
            }
        }
    }
    // restart the game if dead
    else{
        restartGame(game);
    }
}

// detect for collision
void Collision(GameState *game){
    struct Node *body = game->snake.head;
    // detect walls
    // right wall and left wall
    if(*(body->value) + SNAKE_WIDTH > 800 || *(body->value) < 0){
        game->dead = 1;
    }
    // up wall and down wall
    else if(*(body->value + 1) < 0 || *(body->value + 1) + SNAKE_HEIGHT > 600){
        game->dead = 1;
    }

    // detect food
    if(*(body->value) == game->fruit.x && abs(*(body->value + 1) - game->fruit.y) <= SNAKE_HEIGHT){
        if(body->movement == moveUp){
            if(!game->fruit.isSpecial || game->snake.length <= 2){
                game->score++;
                int newNum[2] = {game->fruit.x, (int)(*(game->snake.head->value + 1) - SNAKE_HEIGHT)};
                game->snake.length++;
                game->snake.head = pushNode(body, newNum, body->movement);
                setFruit(&game->fruit);
            }
            else{
                game->score++;
                game->snake.length--;
                game->snake.head = game->snake.head->Child;
                setFruit(&game->fruit);
            }
        }
        else if(body->movement == moveDown){
            if(!game->fruit.isSpecial || game->snake.length <= 2){
                game->score++;
                int newNum[2] = {game->fruit.x, (int)(*(game->snake.head->value + 1) + SNAKE_HEIGHT)};
                game->snake.length++;
                game->snake.head = pushNode(body, newNum, body->movement);
                setFruit(&game->fruit);
            }
            else{
                game->score++;
                game->snake.length--;
                game->snake.head = game->snake.head->Child;
                setFruit(&game->fruit);
            }
        }
    }
    else if(*(body->value + 1) == game->fruit.y && abs(*(body->value) - game->fruit.x) <= SNAKE_WIDTH){
        if(body->movement == moveLeft){
            if(!game->fruit.isSpecial || game->snake.length <= 2){
                game->score++;
                int newNum[2] = {(int)(*(game->snake.head->value) - SNAKE_WIDTH), game->fruit.y};
                game->snake.length++;
                game->snake.head = pushNode(body, newNum, body->movement);
                setFruit(&game->fruit);
            }
            else{
                game->score++;
                game->snake.length--;
                game->snake.head = game->snake.head->Child;
                setFruit(&game->fruit);
            }
        }
        else if(body->movement == moveRight){
            if(!game->fruit.isSpecial || game->snake.length <= 2){
                game->score++;
                int newNum[2] = {(int)(*(game->snake.head->value) + SNAKE_WIDTH), game->fruit.y};
                game->snake.length++;
                game->snake.head = pushNode(body, newNum, body->movement);
                setFruit(&game->fruit);
            }
            else{
                game->score++;
                game->snake.length--;
                game->snake.head = game->snake.head->Child;
                setFruit(&game->fruit);
            }
        }
    }

    // detect whether it hits its own body
    if(game->snake.length > 3){
        // init detection point
        int detection[2] = {(int)(*(body->value)), (int)(*(body->value+1))};
        switch(body->movement){
            case moveUp:
                detection[0] += (SNAKE_WIDTH/2);
            break;
            case moveDown:
                detection[0] += (SNAKE_WIDTH/2);
            break;
            case moveLeft:
                detection[1] += (SNAKE_HEIGHT/2);
            break;
            case moveRight:
                detection[1] += (SNAKE_HEIGHT/2);
            break;
        }
        struct Node *afterBody = body->Child->Child;
        // check for every node after the third node
        while(afterBody != NULL){
            // test detection point X
            if(detection[0] > *(afterBody->value) && detection[0] < (*(afterBody->value)+SNAKE_WIDTH)){
                // test detection point Y
                if(detection[1] > *(afterBody->value+1) && detection[1] < (*(afterBody->value+1)+SNAKE_HEIGHT)){
                    game->dead = 1;
                    break;
                }
            }
            afterBody = afterBody->Child;
        }
    }
}


// load food
void setFruit(Fruit *fruit){
    fruit->isSpecial = 0;
    fruit->w=SNAKE_WIDTH;
    fruit->h=SNAKE_HEIGHT;
    fruit->x=(rand()%(800/SNAKE_WIDTH - 2) + 1)*(SNAKE_WIDTH);
    fruit->y=(rand()%(600/SNAKE_HEIGHT - 2) + 1)*(SNAKE_HEIGHT);
    if(rand()%10 == 8){
        fruit->isSpecial = 1;
    }
}

// a function to correct the X and Y value of the snake body
// so that it moves on the grid
int correctXY(int num){
    if(num % SNAKE_WIDTH != 0){
        int remaind = num % SNAKE_WIDTH;
        if(remaind > (int)(num / 2)){
            num += (SNAKE_WIDTH - remaind);
        }
        else{
            num -= remaind;
        }
        return num;
    }
    else{
        return num;
    }
}


// restart the game when is dead
void restartGame(GameState *game){
    // wait for 2 seconds
    SDL_Delay(2000);
    // free the snake linked list
    freeList(game->snake.head);
    // redo the load
    loadGame(game);
}

// draw current score
void drawScore(SDL_Renderer *renderer, GameState *game){
    // init score as string
    char scoreStr[4];
    sprintf(scoreStr, "%d", game->score);
    // init the number len
    int numLen;
    if(game->score < 10){
        numLen = 1;
    }
    else if(game->score < 100){
        numLen = 2;
    }
    else{
        numLen = 3;
    }
    SDL_Color black = {0,0,0,255};
    SDL_Surface *scoreSur = TTF_RenderText_Solid(game->font, scoreStr, black);
    SDL_Texture *scoreTex = SDL_CreateTextureFromSurface(renderer, scoreSur);
    SDL_Rect scoreRect = {10, -30, 20*numLen, 100};
    SDL_RenderCopy(renderer, scoreTex, NULL, &scoreRect);
    SDL_FreeSurface(scoreSur);
    SDL_DestroyTexture(scoreTex);
}

// a function to free every node in the linked list
void freeList(struct Node* head){
    struct Node *tmp;
    while(head != NULL){
        tmp = head;
        head = head->Child;
        free(tmp);
    }
}

void timer(Uint32 *prev, Uint32 *now)
{
    *now = SDL_GetTicks();
    Uint32 delta = *now - *prev;
    if(delta < SPF)
        SDL_Delay((Uint32)SPF - delta);
    *prev = SDL_GetTicks();
}