#if defined(PM_PC)		// PM stands for "platform"

#include "i_arcwell.h"
#include "i_system.h"
#include "r_renderer.h"
#include "typedefs.h"

int main(int argc, char* argv[])
{
	if (I_GameInit() < 0)
	{
		R_Destruct();
		I_SDL_Destruct();
		return -1;
	}

	I_Loop();

	R_Destruct();
	I_SDL_Destruct();
	return 0;
}

#endif /* PM_PC */

