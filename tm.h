#ifndef _TM_H
#define _TM_H

#include <stddef.h>

typedef enum TM_Direction {
    TM_DIR_LEFT = 0,
    TM_DIR_RIGHT = 1,
    TM_DIR_UP = 2,
    TM_DIR_DOWN = 3
} TM_Direction;

typedef size_t TM_Symbol;
typedef size_t TM_State;

typedef struct TM_Transition {
    TM_Direction direction;
    TM_State newState;
    TM_Symbol newSymbol;
} TM_Transition;

typedef struct TM_Def {
    size_t numStates;
    size_t numSymbols;   

    TM_Transition *delta;
} TM_Def;

typedef struct TM_Config {
    const TM_Def *def; 

    size_t tapeWidth;
    size_t tapeHeight;
    TM_Symbol *tape;
    size_t tapePosX, tapePosY;

    TM_State state;
} TM_Config;

TM_Def tm_def_create_random(size_t numStates, size_t numSymbols); 
void tm_def_free(TM_Def *); 
void tm_def_print(const TM_Def *);
int tm_def_save(const TM_Def *, const char *filename);
int tm_def_load(const char *filename, TM_Def *def);

TM_Config tm_config_create(const TM_Def *def,
                           size_t tapeWidth, size_t tapeHeight);
void tm_config_free(TM_Config *);
void tm_config_step(TM_Config *, int nSteps);
void tm_config_print(const TM_Config *);

#endif

