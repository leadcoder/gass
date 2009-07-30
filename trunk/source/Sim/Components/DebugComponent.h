
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
#ifndef DEBUG_COMPONENT_H
#define DEBUG_COMPONENT_H

#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Common.h"
#include "Core/MessageSystem/Message.h"

namespace GASS
{
	class DebugComponent : public Reflection<DebugComponent,BaseSceneComponent>
	{
	public:
		DebugComponent(void);
		~DebugComponent(void);
		static void RegisterReflection();
		virtual void OnCreate();
	protected:
		void OnLoad(MessagePtr message);
		void OnChangeName(MessagePtr message);
		bool GetShowNodeName() const;
		void SetShowNodeName(bool value);
		bool m_ShowNodeName;
	private:
	};
}
#endif
