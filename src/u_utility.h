#ifndef U_UTILITY_H_
#define U_UTILITY_H_

#include "typedefs.h"

// --- DEFINITIONS ---
void U_ReactionTimerStart(rtimer_t* pReactionTimer);
void U_ReactionTimerEnd(rtimer_t* pReactionTimer);
bool U_IsTimeToReact(rtimer_t* pReactionTimer);

// --- IMPLEMENTATIONS ---
#if defined(STB_REACTION_TIMER_IMPLEMENTATION)
void U_ReactionTimerStart(rtimer_t* pReactionTimer)
{
	pReactionTimer->currentTime = SDL_GetTicks();
}

void U_ReactionTimerEnd(rtimer_t* pReactionTimer)
{
	pReactionTimer->lastTime = pReactionTimer->currentTime;
}

bool U_IsTimeToReact(rtimer_t* pReactionTimer)
{
	return pReactionTimer->currentTime - pReactionTimer->lastTime >= pReactionTimer->reactionTime;
}
#endif /* STB_REACTION_TIMER_IMPLEMENTATION */

#endif /* U_UTILITY_H_ */
