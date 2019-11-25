#ifndef __TEXT_COMPONENT_H__
#define __TEXT_COMPONENT_H__

#include "NetComponent.h"
#include "TextResource.h"

namespace sand {

	//----------------------------------------------------------------------------------------------------
	struct TextComponent : public NetComponent {

		static ComponentType GetType() { return ComponentType::TEXT; }
		static TextComponent* Instantiate() { return new TextComponent(); }

		TextComponent();
		~TextComponent();

		U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
		void Read(InputMemoryStream& inputStream, U32 dirtyState) override;

		std::shared_ptr<TextResource> m_text;
	};
}

#endif
