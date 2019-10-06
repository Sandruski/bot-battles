#ifndef __MODULE_RENDERER_H__
#define __MODULE_RENDERER_H__

#include "Module.h"

namespace sand
{

	struct ComponentRenderer;

	//----------------------------------------------------------------------------------------------------
	class ModuleRenderer : public Module
	{
	public:
		static const char* GetName();

	public:
		ModuleRenderer();
		~ModuleRenderer() override;

		bool StartUp() override;
		bool ShutDown() override;

		bool Draw() const;

	private:
		void BeginDraw(const ComponentRenderer& renderer) const;
		void EndDraw(const ComponentRenderer& renderer) const;
	};
}

#endif
