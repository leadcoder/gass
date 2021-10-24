#pragma once

#include <osgShadow/ViewDependentShadowMap>

namespace osgShadow
{
	class ViewDependentShadowMapExt : public ViewDependentShadowMap
	{
	public:
		void cull(osgUtil::CullVisitor& cv) override;
		void createShaders() override;
	};
}