
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

#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{
	class DebugComponent : public Reflection<DebugComponent,BaseSceneComponent>
	{
	public:
		DebugComponent(void);
		~DebugComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
	protected:
		void OnLoad(MessagePtr message);
		void OnChangeName(MessagePtr message);
		void OnSettings(GASS::MessagePtr message);
		bool m_ShowNodeName;
		bool GetShowNodeName() const;
		void SetShowNodeName(bool value);
	private:
	};

	typedef boost::shared_ptr<DebugComponent> DebugComponentPtr;
}
#endif
