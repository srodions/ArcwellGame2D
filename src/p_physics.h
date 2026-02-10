#ifndef P_PHYSICS_H_
#define P_PHYSICS_H_

#define FLOOR_DISTANCE 512

// --- DEFINITIONS ---
void P_EntityFallJump(e_manager_t* pEntManager, gamestate_t* pGameState);

// --- IMPLEMENTATIONS ---
#if defined(STB_PHYSICS_IMPLEMENTATION)

static const float 	gravity = 2200.0f;
static const float 	jump_force = -800.0f;
static const float 	player_speed = 150.0f;
static const float 	camera_speed = 6.0f;
static const float 	knockback_strength = 300.0f;

void P_EntityFallJump(e_manager_t* pEntManager, gamestate_t* pGameState)
{
	double dt = pGameState->deltaTime;

	for (int i = 0; i < pEntManager->entitiesCount; ++i)
	{
		if (pEntManager->isFalling[i])
			pEntManager->velocities[i].gravityAccel += gravity * dt;
		else
			pEntManager->velocities[i].gravityAccel = 0.0f;

		pEntManager->transforms[i].logY += pEntManager->velocities[i].gravityAccel * dt;

		if (pEntManager->transforms[i].logY >= FLOOR_DISTANCE)
		{
			pEntManager->transforms[i].logY = (float) FLOOR_DISTANCE;
			pEntManager->isFalling[i] = false;
		}
		else pEntManager->isFalling[i] = true;
	}
}
#endif /* STB_PHYSICS_IMPLEMENTATION */

#endif /* P_PHYSICS_H_ */
