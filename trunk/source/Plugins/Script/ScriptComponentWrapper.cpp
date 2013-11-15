/* File : example.cpp */


#include <iostream>
#include "ScriptComponentWrapper.h"
#include "Sim/GASSBaseSceneComponent.h"

ScriptComponentWrapper::ScriptComponentWrapper() 
{

}

ScriptComponentWrapper::~ScriptComponentWrapper()
{
	
}

GASS::BaseSceneComponentPtr ScriptComponentWrapper::GetComponent() const
{
	return GASS::BaseSceneComponentPtr(m_Component);
}

float ScriptComponentWrapper::GetFloatAttribute(const std::string &name) const
{
	float value;
	GASS::BaseSceneComponentPtr comp = GetComponent();
	if(comp)
	{
		boost::any any_value;
		if(comp->GetPropertyByType(name,any_value))
		{
			value = boost::any_cast<float>(any_value);
		}
	}
	return value;
}

void ScriptComponentWrapper::SetFloatAttribute(const std::string &name, float value)
{
	GASS::BaseSceneComponentPtr comp = GetComponent();
	if(comp)
	{
		comp->SetPropertyByType(name,value);
	}
}

double ScriptComponentWrapper::GetDoubleAttribute(const std::string &name) const
{
	double value;
	GASS::BaseSceneComponentPtr comp = GetComponent();
	if(comp)
	{
		boost::any any_value;
		if(comp->GetPropertyByType(name,any_value))
		{
			value = boost::any_cast<double>(any_value);
		}
	}
	return value;
}

void ScriptComponentWrapper::SetDoubleAttribute(const std::string &name, double value)
{
	GASS::BaseSceneComponentPtr comp = GetComponent();
	if(comp)
	{
		comp->SetPropertyByType(name,value);
	}
}

int ScriptComponentWrapper::GetIntAttribute(const std::string &name) const
{
	int value;
	GASS::BaseSceneComponentPtr comp = GetComponent();
	if(comp)
	{
		boost::any any_value;
		if(comp->GetPropertyByType(name,any_value))
		{
			value = boost::any_cast<int>(any_value);
		}
	}
	return value;
}

void ScriptComponentWrapper::SetIntAttribute(const std::string &name, int value)
{
	GASS::BaseSceneComponentPtr comp = GetComponent();
	if(comp)
	{
		comp->SetPropertyByType(name,value);
	}
}

std::string ScriptComponentWrapper::GetStringAttribute(const std::string &name) const
{
	std::string value;
	GASS::BaseSceneComponentPtr comp = GetComponent();
	if(comp)
	{
		comp->GetPropertyByString(name,value);
	}
	return value;
}

void ScriptComponentWrapper::SetStringAttribute(const std::string &name, const std::string &value)
{
	GASS::BaseSceneComponentPtr comp = GetComponent();
	if(comp)
	{
		comp->SetPropertyByType(name,value);
	}
}

GASS::Vec3 ScriptComponentWrapper::GetVec3Attribute(const std::string &name) const
{
	GASS::Vec3 value;
	GASS::BaseSceneComponentPtr comp = GetComponent();
	if(comp)
	{
		boost::any any_value;
		if(comp->GetPropertyByType(name,any_value))
		{
			value = boost::any_cast<GASS::Vec3>(any_value);
		}
	}
	return value;
}

void ScriptComponentWrapper::SetVec3tAttribute(const std::string &name, GASS::Vec3  value)
{
	GASS::BaseSceneComponentPtr comp = GetComponent();
	if(comp)
	{
		comp->SetPropertyByType(name,value);
	}
}

void ScriptComponentWrapper::LogMessage(const std::string &message)
{
	std::cout << message << "\n";
}

