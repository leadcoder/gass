
/****************************************************************************
*                                                                           *
* HiFiEngine                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: hifiengine@gmail.com                                               *
* Web page: http://n00b.dyndns.org/HiFiEngine                               *
*                                                                           *
* HiFiEngine is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the HiFiEngine license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 
#ifndef SPHERE_GEOMETRY_COMPONENT_H
#define SPHERE_GEOMETRY_COMPONENT_H

#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSIShape.h"
#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{
	class SphereGeometryComponent : public Reflection<SphereGeometryComponent,BaseSceneComponent> , public IShape
	{
	public:
		SphereGeometryComponent(void);
		~SphereGeometryComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual bool IsPointInside(const Vec3 &point) const;
		virtual Vec3 GetRandomPoint() const;
		Float GetRadius() const;
	protected:
		void OnLoad(MessagePtr message);
		void OnChangeName(MessagePtr message);
		void SetRadius(Float value);
		void UpdateMesh();
	private:
		Float m_Radius;
	};
}
#endif
