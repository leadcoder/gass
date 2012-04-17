
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
#ifndef BOX_GEOMETRY_COMPONENT_H
#define BOX_GEOMETRY_COMPONENT_H

#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{
	class BoxGeometryComponent : public Reflection<BoxGeometryComponent,BaseSceneComponent>
	{
	public:
		BoxGeometryComponent(void);
		~BoxGeometryComponent(void);
		static void RegisterReflection();
		virtual void OnCreate();
	protected:
		void OnLoad(MessagePtr message);
		void OnChangeName(MessagePtr message);
		Vec3 GetSize() const;
		void SetSize(const Vec3 &value);
		Vec3 m_Size;
	private:
	};
}
#endif
