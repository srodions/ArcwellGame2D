#ifndef U_UTILITY_H_
#define U_UTILITY_H_

/* --- DEFINITIONS --- */
void U_ReactionTimerStart(rtimer_t* pReactionTimer);
void U_ReactionTimerEnd(rtimer_t* pReactionTimer);
void U_ReactionTimerReset(rtimer_t* pReactionTimer);
bool U_IsTimeToReact(rtimer_t* pReactionTimer);

/* --- IMPLEMENTATIONS --- */
#if defined(STB_UTILITY_IMPLEMENTATION)
/*
 * Starts the timer from current number of milliseconds since SDL library initialization.
 */
void U_ReactionTimerStart(rtimer_t* pReactionTimer)
{
	pReactionTimer->currentTime = SDL_GetTicks();
}

void U_ReactionTimerEnd(rtimer_t* pReactionTimer)
{
	pReactionTimer->lastTime = pReactionTimer->currentTime;
}

void U_ReactionTimerReset(rtimer_t* pReactionTimer)
{
    pReactionTimer->lastTime = SDL_GetTicks();
}

/*
 * Checks whether is delta time reached reaction time.
 */
bool U_IsTimeToReact(rtimer_t* pReactionTimer)
{
	return pReactionTimer->currentTime - pReactionTimer->lastTime >= pReactionTimer->reactionTime;
}
#endif /* STB_UTILITY_IMPLEMENTATION */

#endif /* U_UTILITY_H_ */
