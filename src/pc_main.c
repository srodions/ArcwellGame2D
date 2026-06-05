#if defined(PM_PC)		// PM stands for "platform"

#include "i_arcwell.h"
#include "i_system.h"
#include "typedefs.h"

int main(int argc, char* argv[])
{
	if (I_GameInit() < 0)
	{
		I_Destruct();
		return -1;
	}

	I_Loop();
	I_Destruct();

	return 0;
}

#endif /* PM_PC */

