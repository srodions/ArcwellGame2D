#ifndef H_KEYBOARD_H_
#define H_KEYBOARD_H_

typedef struct GameState gamestate_t;
typedef struct Keystates keystates_t;
typedef struct EntityManager e_manager_t;

extern keystates_t h_keyStates;

void H_InitKeyStates();
void H_HandleKeyStates(gamestate_t* pGameState, e_manager_t* pEntManager);

#endif /* H_KEYBOARD_H_ */
