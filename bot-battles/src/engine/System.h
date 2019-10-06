#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "Memory.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class System
	{
	public:
		System();
		~System();

		void Update(F32 dt);
		//void NotifyComponent(Component*);
	};
}

#endif