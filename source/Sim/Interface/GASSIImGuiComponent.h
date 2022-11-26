#pragma once
#include "Sim/GASSCommon.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/GASSGraphicsMaterial.h"

namespace GASS
{
	class IImGuiComponent
	{
		GASS_DECLARE_CLASS_AS_INTERFACE(IImGuiComponent)
	public:
		virtual void DrawGui() =0;
	};
	typedef GASS_SHARED_PTR<IImGuiComponent> ImGuiComponentPtr;
}

