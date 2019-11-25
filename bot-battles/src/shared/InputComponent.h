#ifndef __INPUT_COMPONENT_H__
#define __INPUT_COMPONENT_H__

#include "ComponentMemberTypes.h"
#include "MemoryStream.h"
#include "NetComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct InputComponent : public NetComponent {

    static ComponentType GetType() { return ComponentType::INPUT; }
    static InputComponent* Instantiate() { return new InputComponent(); }

	InputComponent();
	~InputComponent();

	void Copy(const InputComponent& other);

	U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
	void Read(InputMemoryStream& inputStream, U32 dirtyState) override;

    Vec2 m_acceleration;
    float m_angularAcceleration;
};
}

#endif
