
#pragma once

#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
namespace GASS
{
	class IBillboardComponent : public IGeometryComponent
	{
		GASS_DECLARE_CLASS_AS_INTERFACE(IBillboardComponent)
	public:
		virtual void SetColor(const ColorRGBA& color) = 0;
	};
}
