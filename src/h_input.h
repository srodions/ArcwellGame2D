#ifndef H_INPUT_H_
#define H_INPUT_H_

#include <stdbool.h>
#include "g_constants.h"

typedef struct GameState gamestate_t;
typedef struct EntityManager e_manager_t;
typedef struct InputState inputstate_t;
typedef enum InputMask inputmask_t;

bool H_IsKeyHeld(const inputstate_t* input, inputmask_t button);
bool H_IsKeyJustPressed(const inputstate_t* input, inputmask_t button);
bool H_IsKeyJustReleased(const inputstate_t* input, inputmask_t button);
void H_HandleKeyStates(gamestate_t* pGameState, e_manager_t* pEntManager, inputstate_t* input);

#endif /* H_INPUT_H_ */
