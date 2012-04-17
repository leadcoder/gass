
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
#ifndef ARROW_GEOMETRY_COMPONENT_H
#define ARROW_GEOMETRY_COMPONENT_H

#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{
	class ArrowGeometryComponent : public Reflection<ArrowGeometryComponent,BaseSceneComponent>
	{
	public:
		ArrowGeometryComponent(void);
		~ArrowGeometryComponent(void);
		static void RegisterReflection();
		virtual void OnCreate();
		void SetTexture(const std::string &texture_name);
		std::string GetTexture() const;
	protected:
		void GenerateMesh();
		void OnLoad(MessagePtr message);
		void OnChangeName(MessagePtr message);
		Vec2 GetSize() const;
		void SetSize(const Vec2 &value);
		Vec4 GetColor() const{return m_Color;}
		void SetColor(const Vec4 &value){m_Color =value;}

		Vec2 m_Size;
		std::string  m_Texture;
		Vec4 m_Color;
	private:
	};
}
#endif
