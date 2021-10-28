
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
		virtual void SetSize(float width, float height) = 0;
		virtual float GetWidth() const = 0;
		virtual void SetWidth(float width) = 0;
		virtual float GetHeight() const = 0;
		virtual void SetHeight(float height) = 0;

	};
}
