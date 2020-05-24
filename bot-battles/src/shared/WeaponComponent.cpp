#include "WeaponComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
WeaponComponent::WeaponComponent()
    : m_damage(0)
    , m_ammo(0)
    , m_origin(0.0f, 0.0f)
    , m_destination(0.0f, 0.0f)
    , m_hasHit(false)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void WeaponComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::WEAPON_DAMAGE)) {
        inputStream.Read(m_damage);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::WEAPON_AMMO)) {
        inputStream.Read(m_ammo);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::WEAPON_ORIGIN)) {
        inputStream.Read(m_origin);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::WEAPON_DESTINATION)) {
        inputStream.Read(m_destination);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::WEAPON_HIT)) {
        inputStream.Read(m_hasHit);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 WeaponComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(ComponentMemberType::WEAPON_DAMAGE)) {
        outputStream.Write(m_damage);
        writtenState |= static_cast<U32>(ComponentMemberType::WEAPON_DAMAGE);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::WEAPON_AMMO)) {
        outputStream.Write(m_ammo);
        writtenState |= static_cast<U32>(ComponentMemberType::WEAPON_AMMO);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::WEAPON_ORIGIN)) {
        outputStream.Write(m_origin);
        writtenState |= static_cast<U32>(ComponentMemberType::WEAPON_ORIGIN);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::WEAPON_DESTINATION)) {
        outputStream.Write(m_destination);
        writtenState |= static_cast<U32>(ComponentMemberType::WEAPON_DESTINATION);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::WEAPON_HIT)) {
        outputStream.Write(m_hasHit);
        writtenState |= static_cast<U32>(ComponentMemberType::WEAPON_HIT);
    }

    return writtenState;
}
#endif

//----------------------------------------------------------------------------------------------------
U32 WeaponComponent::Reload(U32 ammo)
{
    m_ammo += ammo;

    return 0;
}
}
