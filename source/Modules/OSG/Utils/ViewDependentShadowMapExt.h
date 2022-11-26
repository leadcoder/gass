#pragma once

#include "Modules/OSG/OSGCommon.h"

namespace osgShadow
{
	class ExportOSG ViewDependentShadowMapExt : public ViewDependentShadowMap
	{
	public:
		void cull(osgUtil::CullVisitor& cv) override;
		void createShaders() override;
	};
}