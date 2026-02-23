#ifndef U_UTILITY_H_
#define U_UTILITY_H_

#include "typedefs.h"

/* --- DEFINITIONS --- */
void U_ReactionTimerStart(rtimer_t* pReactionTimer);
void U_ReactionTimerEnd(rtimer_t* pReactionTimer);
bool U_IsTimeToReact(rtimer_t* pReactionTimer);
void destruct(
	SDL_Window* pWindow, SDL_Renderer* pRenderer,
	font_t* pFont, e_manager_t* pEntManager,
	obj_manager_t* pObjManager, location_t* pLocation
);

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

/*
 * Checks whether is delta time reached reaction time.
 */
bool U_IsTimeToReact(rtimer_t* pReactionTimer)
{
	return pReactionTimer->currentTime - pReactionTimer->lastTime >= pReactionTimer->reactionTime;
}

/*
 * This method is a destructor of SDL components.
 * (Always need to be called in application crash or normal exit!!!)
 */
void destruct(SDL_Window* pWindow, SDL_Renderer* pRenderer, font_t* pFont, e_manager_t* pEntManager, obj_manager_t* pObjManager, location_t* pLocation)
{
	for (int i = 0; i < pEntManager->entitiesCount; ++i)
	{
		SDL_DestroyTexture(pEntManager->sprites[i].spriteImg);
		pEntManager->sprites[i].spriteImg = NULL;
	}

	for (int i = 0; i < pObjManager->objCount; ++i)
	{
		SDL_DestroyTexture(pObjManager->sprites[i].spriteImg);
		pObjManager->sprites[i].spriteImg = NULL;
	}

	if (pLocation->tileMap != NULL)
	{
		SDL_DestroyTexture(pLocation->tileMap);
		pLocation->tileMap = NULL;
	}

	if (pFont->textTexture != NULL)
	{
		SDL_DestroyTexture(pFont->textTexture);
		pFont->textTexture = NULL;
	}

	if (pFont->textSurface != NULL)
	{
		SDL_FreeSurface(pFont->textSurface);
		pFont->textSurface = NULL;
	}

	if (pFont->file)
	{
		TTF_CloseFont(pFont->file);
		pFont->file = NULL;
	}

	if (pRenderer != NULL)
		SDL_DestroyRenderer(pRenderer);

	if (pWindow != NULL)
		SDL_DestroyWindow(pWindow);

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}
#endif /* STB_REACTION_TIMER_IMPLEMENTATION */

#endif /* U_UTILITY_H_ */
