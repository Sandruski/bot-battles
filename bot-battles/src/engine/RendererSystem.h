#ifndef __RENDERER_SYSTEM_H__
#define __RENDERER_SYSTEM_H__

#include "System.h"

namespace sand
{

	struct RendererComponent;

	//----------------------------------------------------------------------------------------------------
	class RendererSystem : public System
	{
	public:
		static SystemType GetType() { return SystemType::RENDERER; }

	public:
		RendererSystem();
		~RendererSystem() override;

		bool StartUp() override;
		bool Render() override;
		bool ShutDown() override;

	private:
		void BeginDraw(const RendererComponent& renderer) const;
		void EndDraw(const RendererComponent& renderer) const;
	};
}

#endif
