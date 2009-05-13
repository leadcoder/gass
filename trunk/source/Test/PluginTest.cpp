// CoreTest.cpp : Defines the entry point for the console application.
//
#include <boost/bind.hpp>

#include "Core/Object/IComponent.h"
#include "Core/Math/Vector.h"
#include "Core/Utils/Factory.h"
#include "Core/Utils/Log.h"
#include "Core/Utils/Serialize.h"
#include "Core/Utils/Timer.h"
#include "Core/Object/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"

#include "Core/PluginSystem/PluginManager.h"

#include <stdio.h>
#include <iostream>
#include <fstream>

class PluginComponent : public HiFi::RTTIClass<PluginComponent, HiFi::IComponent>
{
public:
	static void RegisterReflection()
	{
		HiFi::ComponentFactory::GetPtr()->Register("PluginComponent",new HiFi::Creator<PluginComponent, IComponent>);
		RegisterProperty<float>("Size", &PluginComponent::GetSize, &PluginComponent::SetSize);
	}
	float GetSize()const {return m_Size;}
	void SetSize(const float &size){m_Size = size;}
	void OnCreate()
	{
		
	}
private:
	float m_Size;
};





