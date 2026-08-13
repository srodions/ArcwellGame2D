#if defined(PM_PC)		// PM stands for "platform"

#include "i_arcwell.h"
#include "i_system.h"
#include "r_renderer.h"
#include "typedefs.h"

int main(int argc, char* argv[])
{
	if (I_GameInit() < 0)
	{
		I_GameExit();
		return -1;
	}

	I_GameLoop();
	I_GameExit();

	return 0;
}

#endif /* PM_PC */

