#include "h_input.h"
#include "g_entity.h"
#include "p_physics.h"
#include "typedefs.h"

bool H_IsKeyHeld(const inputstate_t* input, inputmask_t button)
{
    return (input->current & button) != 0;
}

bool H_IsKeyJustPressed(const inputstate_t* input, inputmask_t button)
{
    return ((input->current & button) != 0) && ((input->previous & button) == 0);
}

bool H_IsKeyJustReleased(const inputstate_t* input, inputmask_t button)
{
    return ((input->current & button) == 0) && ((input->previous & button) != 0);
}

void H_HandleKeyStates(gamestate_t* pGameState, e_manager_t* pEntManager, inputstate_t* input)
{
	if (H_IsKeyJustPressed(input, INPUT_EXIT))
		pGameState->isRunning = false;

	if (H_IsKeyJustPressed(input, INPUT_DEBUG))
		pGameState->isDebugMode = !pGameState->isDebugMode;

	if (H_IsKeyJustPressed(input, INPUT_PAUSE))
		pGameState->isPaused = !pGameState->isPaused;

	if (!pGameState->isPaused)
	{
		bool left  = H_IsKeyHeld(input, INPUT_LEFT);
		bool right = H_IsKeyHeld(input, INPUT_RIGHT);

		pEntManager->isMoving[PLAYER] = left || right;

		if (left && !right)
			pEntManager->sprites[PLAYER].direction = DIR_LEFT;
		else if (right && !left)
			pEntManager->sprites[PLAYER].direction = DIR_RIGHT;

		if (H_IsKeyJustPressed(input, INPUT_JUMP))
			G_EntityJump(pGameState, pEntManager, PLAYER);

		if (H_IsKeyJustPressed(input, INPUT_ATTACK))
			G_SetState(PLAYER, pEntManager, STATE_ATTACK);
	}
}


