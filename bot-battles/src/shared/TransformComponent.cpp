#include "TransformComponent.h"

#include "ComponentMemberTypes.h"
#include "MemoryStream.h"

namespace sand
{
	//----------------------------------------------------------------------------------------------------
	TransformComponent::TransformComponent()
		: m_position()
		, m_rotation(0.0f)
	{
	}

	//----------------------------------------------------------------------------------------------------
	TransformComponent::~TransformComponent()
	{
	}

	//----------------------------------------------------------------------------------------------------
	U32 TransformComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
	{
		U32 writtenState = 0;

		if (dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_POSITION)) {
			outputStream.WritePosition(m_position);
			writtenState |= static_cast<U32>(ComponentMemberType::TRANSFORM_POSITION);
		}
		if (dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_ROTATION)) {
			outputStream.Write(m_rotation);
			writtenState |= static_cast<U32>(ComponentMemberType::TRANSFORM_ROTATION);
		}

		return writtenState;
	}

	//----------------------------------------------------------------------------------------------------
	void TransformComponent::Read(InputMemoryStream& inputStream, U32 dirtyState)
	{
		if (dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_POSITION)) {
			inputStream.ReadPosition(m_position);
		}
		if (dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_ROTATION)) {
			inputStream.Read(m_rotation);
		}
	}
}
