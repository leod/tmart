#include "tm.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

TM_Def tm_def_create_random(size_t numStates, size_t numSymbols) {
    size_t numTransitions = numStates * numSymbols;
    TM_Transition *delta = malloc(sizeof(TM_Transition) * numTransitions);

    TM_State state;
    TM_Symbol symbol;
    for (state = 0; state < numStates; state++) {
        for (symbol = 0; symbol < numSymbols; symbol++) {
            TM_Transition *trans = &delta[state * numSymbols + symbol];

            trans->direction = rand() % 4;
            trans->newState = rand() % numStates;
            trans->newSymbol = rand() % (numSymbols - 1) + 1;
        }
    }

    TM_Def def = {
        numStates,
        numSymbols,
        delta
    };

    return def;
}

void tm_def_free(TM_Def *def) {
    free(def->delta);
    def->delta = NULL;
}

void tm_def_print(const TM_Def *def) {
    TM_State state;
    TM_Symbol symbol;

    for (state = 0; state < def->numStates; state++) {
        printf("%d: ", state);

        for (symbol = 0; symbol < def->numSymbols; symbol++) {
            TM_Transition *trans = &def->delta[state * def->numSymbols + symbol];
            
            printf("%d -> (%d, %d, %d), ", symbol, trans->newState,
                   trans->newSymbol, trans->direction);
        }

        printf("\n");
    }
}

int tm_def_save(const TM_Def *def, const char *filename) {
    FILE *file = fopen(filename, "w+");
    if (!file)
        return 0;

    fprintf(file, "%d,%d\n", def->numStates, def->numSymbols);

    TM_State state;
    TM_Symbol symbol;
    for (state = 0; state < def->numStates; state++) {
        for (symbol = 0; symbol < def->numSymbols; symbol++) {
            TM_Transition *trans = &def->delta[state * def->numSymbols + symbol];
            
            fprintf(file, "%d,%d,%d\n", trans->newState, trans->newSymbol,
                    trans->direction);
        }
    }

    fclose(file);
    return 1;
}

int tm_def_load(const char *filename, TM_Def *def) {
    FILE *file = fopen(filename, "r");
    if (!file)
        return 0;

    size_t numStates, numSymbols;
    fscanf(file, "%d,%d\n", &numStates, &numSymbols);

    size_t numTransitions = numStates * numSymbols;
    TM_Transition *delta = malloc(sizeof(TM_Transition) * numTransitions);

    TM_State state;
    TM_Symbol symbol;
    for (state = 0; state < numStates; state++) {
        for (symbol = 0; symbol < numSymbols; symbol++) {
            TM_Transition *trans = &delta[state * numSymbols + symbol];

            fscanf(file, "%d,%d,%d\n", &trans->newState, &trans->newSymbol,
                   &trans->direction);
        }
    }

    def->numStates = numStates;
    def->numSymbols = numSymbols;
    def->delta = delta;

    if (ferror(file)) {
        fclose(file);
        return 0;
    }

    fclose(file);
    return 1;
}

TM_Config tm_config_create(const TM_Def *def,
                           size_t tapeWidth, size_t tapeHeight) {
    TM_Symbol *tape = malloc(sizeof(size_t) * tapeWidth * tapeHeight);
    memset(tape, 0, sizeof(size_t) * tapeWidth * tapeHeight);

    TM_Config config = {
        def,
        tapeWidth,
        tapeHeight,
        tape,
        tapeWidth / 2,
        tapeHeight / 2,
        0
    };

    return config;
}

void tm_config_free(TM_Config *tm) {
    free(tm->tape);
    tm->tape = NULL;
}

void tm_config_step(TM_Config *config, int nSteps) {
    const TM_Def *def = config->def;

    TM_State curState = config->state;
    size_t tapePosX = config->tapePosX,
           tapePosY = config->tapePosY;

    while (nSteps-- > 0) {
        size_t tapeIndex = tapePosY * config->tapeWidth + tapePosX;
        TM_Symbol curSymbol = config->tape[tapeIndex];

        size_t transIndex = curState * def->numSymbols + curSymbol;
        TM_Transition *transition = &def->delta[transIndex];

        curState = transition->newState;
        config->tape[tapeIndex] = transition->newSymbol;

        switch (transition->direction) {
        case TM_DIR_LEFT:
            if (tapePosX > 0)
                tapePosX--;
            else
                tapePosX = config->tapeWidth - 1;
            break;
        case TM_DIR_UP:
            if (tapePosY > 0)
                tapePosY--;
            else
                tapePosY = config->tapeHeight - 1;
            break;
        case TM_DIR_RIGHT:
            if (tapePosX < config->tapeWidth - 1)
                tapePosX++;
            else
                tapePosX = 0;
            break;
        case TM_DIR_DOWN:
            if (tapePosY < config->tapeHeight - 1)
                tapePosY++;
            else
                tapePosY = 0;
            break;
        }
    }

    config->state = curState;
    config->tapePosX = tapePosX;
    config->tapePosY = tapePosY;
}

void tm_config_print(const TM_Config *config) {
    printf("[%d|%d] (%d|%d) @%d\n", config->tapeWidth, config->tapeHeight,
           config->tapePosX, config->tapePosY, config->state);
}
