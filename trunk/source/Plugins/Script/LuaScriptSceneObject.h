#pragma once
#include <string>
#include "Sim/GASS.h"
#include "LuaScriptComponent.h"

class ScriptComponentWrapper;
class LuaScriptSceneObject
{
	friend class GASS::LuaScriptComponent;
public:
	LuaScriptSceneObject();
	virtual ~LuaScriptSceneObject();

	//functions called from lua script
	ScriptComponentWrapper* GetComponent(const std::string &name);

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
	bool GetInsideTriggerObject(const std::string &object_id) const;
	bool AddTriggerObjectByID(const std::string &object_id);
	
private:
	//functions called from LuaScriptComponent
	void SetSceneObject(GASS::SceneObjectPtr object);
	void Reset();
	bool InsideObject(GASS::SceneObjectPtr obj) const;
	GASS::SceneObjectPtr GetSceneObject() const {return GASS::SceneObjectPtr(m_SceneObject,boost::detail::sp_nothrow_tag());}
	GASS::SceneObjectWeakPtr m_SceneObject;
	typedef std::map<std::string,std::vector<GASS::SceneObjectWeakPtr> > ObjectCache;
	ObjectCache m_CachedObjects;
	std::map<std::string,ScriptComponentWrapper*> m_Components;
};


