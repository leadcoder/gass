/****************************************************************************
*                                                                           *
* GASS                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: GASS@gmail.com                                               *
* Web page: http://n00b.dyndns.org/GASS                               *
*                                                                           *
* GASS is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the GASS license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 

#pragma once
#include "Sim/GASS.h"
#include "Plugins/Base/CoreMessages.h"
#include "AIMessages.h"

namespace GASS
{
	class RecastOffmeshMeshConnectionComponent : public Reflection<RecastOffmeshMeshConnectionComponent,BaseSceneComponent>
	{
	public:
		RecastOffmeshMeshConnectionComponent();
		virtual ~RecastOffmeshMeshConnectionComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		Vec3 GetStartPos() const {return m_StartPos;}
		Vec3 GetEndPos() const {return m_EndPos;}
		ADD_ATTRIBUTE(std::string,Mode)
		float GetRadius() const;
	protected:
		void OnStartNodeTransformation(TransformationNotifyMessagePtr message);
		void OnEndNodeTransformation(TransformationNotifyMessagePtr message);
		void UpdateConnectionLine();
		void SetVisible(bool value);
		bool GetVisible() const;
		void SetRadius(float value);
		
		ManualMeshDataPtr m_ConnectionLine;
		bool m_Visible;
		bool m_Initialized;
		float m_Radius;
		Vec3 m_StartPos;
		Vec3 m_EndPos;
		SceneObjectWeakPtr m_EndNode;
	};
	typedef boost::shared_ptr<RecastOffmeshMeshConnectionComponent> RecastOffmeshMeshConnectionComponentPtr;
	typedef boost::weak_ptr<RecastOffmeshMeshConnectionComponent> RecastOffmeshMeshConnectionComponentWeakPtr;
}

