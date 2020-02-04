#include "WeaponComponent.h"

namespace sand
{    
    //----------------------------------------------------------------------------------------------------
	WeaponComponent::WeaponComponent()
	{
	}

#ifdef _CLIENT
    //----------------------------------------------------------------------------------------------------
    void WeaponComponent::Read(InputMemoryStream& /*inputStream*/, U32 /*dirtyState*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
    {
        // TODO
    }
#elif defined(_SERVER)
    //----------------------------------------------------------------------------------------------------
    U32 WeaponComponent::Write(OutputMemoryStream& /*outputStream*/, U32 /*dirtyState*/) const
    {
        // TODO
    }
#endif
}