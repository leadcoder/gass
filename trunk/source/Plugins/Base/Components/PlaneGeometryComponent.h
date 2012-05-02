
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
#ifndef PLANE_GEOMETRY_COMPONENT_H
#define PLANE_GEOMETRY_COMPONENT_H

#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{
	class PlaneGeometryComponent : public Reflection<PlaneGeometryComponent,BaseSceneComponent>
	{
	public:
		PlaneGeometryComponent(void);
		~PlaneGeometryComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		void SetTexture(const std::string &texture_name);
		std::string GetTexture() const;
	protected:
		void GenerateMesh();
		void OnLoad(MessagePtr message);
		void OnChangeName(MessagePtr message);
		Vec2 GetSize() const;
		void SetSize(const Vec2 &value);
		float GetTransparency() const;
		void SetTransparency(float value);

		Vec2 m_Size;
		std::string  m_Texture;
		float m_Transparency;
	private:
	};
}
#endif
