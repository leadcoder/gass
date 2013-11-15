#pragma once
#include <string>
#include "Sim/GASS.h"
#include "LuaScriptComponent.h"

class ScriptComponentWrapper
{
	friend class GASS::LuaScriptComponent;
public:
	ScriptComponentWrapper();
	virtual ~ScriptComponentWrapper();

	//functions called from lua script
    float GetFloatAttribute(const std::string &name) const;
	void SetFloatAttribute(const std::string &name, float value);
	double GetDoubleAttribute(const std::string &name) const;
	void SetDoubleAttribute(const std::string &name, double value);
	int GetIntAttribute(const std::string &name) const;
	void SetIntAttribute(const std::string &name, int value);
	std::string GetStringAttribute(const std::string &name) const;
	void SetStringAttribute(const std::string &name, const std::string &value);
	GASS::Vec3 GetVec3Attribute(const std::string &name) const;
	void SetVec3tAttribute(const std::string &name, GASS::Vec3  value);
	void LogMessage(const std::string &message);
	GASS::BaseSceneComponentPtr GetComponent();
private:
	GASS::BaseSceneComponentWeakPtr m_Component;
};


