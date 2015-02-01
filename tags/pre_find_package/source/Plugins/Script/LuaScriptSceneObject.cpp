/* File : example.cpp */


#include <iostream>
#include "LuaScriptSceneObject.h"
#include "ScriptComponentWrapper.h"

LuaScriptSceneObject::LuaScriptSceneObject() 
{
	

}

LuaScriptSceneObject::~LuaScriptSceneObject()
{
	
}

ScriptComponentWrapper* LuaScriptSceneObject::GetComponent(const std::string &name)
{
	return m_Components[name];
}

float LuaScriptSceneObject::GetFloatAttribute(const std::string &name) const
{
	float value;
	GASS::SceneObjectPtr so = GetSceneObject();
	if(so)
	{
		boost::any any_value;
		if(so->GetPropertyByType(name,any_value))
		{
			value = boost::any_cast<float>(any_value);
		}
	}
	return value;
}

void LuaScriptSceneObject::SetFloatAttribute(const std::string &name, float value)
{
	GASS::SceneObjectPtr so = GetSceneObject();
	if(so)
	{
		so->SetPropertyByType(name,value);
	}
}

double LuaScriptSceneObject::GetDoubleAttribute(const std::string &name) const
{
	double value;
	GASS::SceneObjectPtr so = GetSceneObject();
	if(so)
	{
		boost::any any_value;
		if(so->GetPropertyByType(name,any_value))
		{
			value = boost::any_cast<double>(any_value);
		}
	}
	return value;
}

void LuaScriptSceneObject::SetDoubleAttribute(const std::string &name, double value)
{
	GASS::SceneObjectPtr so = GetSceneObject();
	if(so)
	{
		so->SetPropertyByType(name,value);
	}
}

int LuaScriptSceneObject::GetIntAttribute(const std::string &name) const
{
	int value;
	GASS::SceneObjectPtr so = GetSceneObject();
	if(so)
	{
		boost::any any_value;
		if(so->GetPropertyByType(name,any_value))
		{
			value = boost::any_cast<int>(any_value);
		}
	}
	return value;
}

void LuaScriptSceneObject::SetIntAttribute(const std::string &name, int value)
{
	GASS::SceneObjectPtr so = GetSceneObject();
	if(so)
	{
		so->SetPropertyByType(name,value);
	}
}

std::string LuaScriptSceneObject::GetStringAttribute(const std::string &name) const
{
	std::string value;
	GASS::SceneObjectPtr so = GetSceneObject();
	if(so)
	{
		so->GetPropertyByString(name,value);
	}
	return value;
}

void LuaScriptSceneObject::SetStringAttribute(const std::string &name, const std::string &value)
{
	GASS::SceneObjectPtr so = GetSceneObject();
	if(so)
	{
		so->SetPropertyByType(name,value);
	}
}

GASS::Vec3 LuaScriptSceneObject::GetVec3Attribute(const std::string &name) const
{
	GASS::Vec3 value;
	GASS::SceneObjectPtr so = GetSceneObject();
	if(so)
	{
		boost::any any_value;
		if(so->GetPropertyByType(name,any_value))
		{
			value = boost::any_cast<GASS::Vec3>(any_value);
		}
	}
	return value;
}

void LuaScriptSceneObject::SetVec3tAttribute(const std::string &name, GASS::Vec3  value)
{
	GASS::SceneObjectPtr so = GetSceneObject();
	if(so)
	{
		so->SetPropertyByType(name,value);
	}
}

void LuaScriptSceneObject::LogMessage(const std::string &message)
{
	std::cout << message << "\n";
}

bool LuaScriptSceneObject::InsideObject(GASS::SceneObjectPtr obj) const
{
	GASS::SceneObjectPtr so = GetSceneObject();
	if(so)
	{
		GASS::LocationComponentPtr l1 = so->GetFirstComponentByClass<GASS::ILocationComponent>();
		GASS::LocationComponentPtr l2 = obj->GetFirstComponentByClass<GASS::ILocationComponent>();
		GASS::GeometryComponentPtr mesh = obj->GetFirstComponentByClass<GASS::IGeometryComponent>();
		if(l1 && l2 && mesh)
		{
			GASS::Sphere sphere = mesh->GetBoundingSphere();
			GASS::AABox box = mesh->GetBoundingBox();
			GASS::Vec3 p1 = l1->GetWorldPosition();
			GASS::Vec3 p2 = l2->GetWorldPosition();
			if((p1 - p2).FastLength() < sphere.m_Radius)
			{
				return true;
			}
		}
	}
	return false;
}

bool LuaScriptSceneObject::GetInsideTriggerObject(const std::string &object_id) const
{
	GASS::SceneObjectPtr so = GetSceneObject();
	if(so)
	{
		//Check if object is cached!
		ObjectCache::const_iterator iter = m_CachedObjects.find(object_id);
		if(iter != m_CachedObjects.end())
		{
			for(std::vector<GASS::SceneObjectWeakPtr>::const_iterator v_iter = iter->second.begin(); v_iter != iter->second.end(); v_iter++)
			{
				GASS::SceneObjectPtr obj(*v_iter,boost::detail::sp_nothrow_tag());
				if(obj)
				{
					bool value = InsideObject(obj);
					if(value) 
						return true;
				}

			}
		}
	}
	return false;
}

bool LuaScriptSceneObject::AddTriggerObjectByID(const std::string &object_id)
{
	GASS::SceneObjectPtr so = GetSceneObject();
	if(so)
	{
		//Try to find objects by id
		std::vector<GASS::SceneObjectPtr> objects;
		std::vector<GASS::SceneObjectWeakPtr> weak_objects;
		so->GetScene()->GetRootSceneObject()->GetChildrenByID(objects,object_id);
		if(objects.size() > 0)
		{
			for(int i = 0; i < objects.size(); i++)
			{
				weak_objects.push_back(weak_objects[i]);
			}
			m_CachedObjects[object_id] = weak_objects;
			return true;
		}
	}
	return false;
}


void LuaScriptSceneObject::Reset()
{
	m_CachedObjects.clear();
}

void LuaScriptSceneObject::SetSceneObject(GASS::SceneObjectPtr object)
{
	m_SceneObject = object;
	//create component wrappers!
	GASS::IComponentContainer::ComponentIterator comp_iter = object->GetComponents();
	while(comp_iter.hasMoreElements())
	{
		GASS::ComponentPtr comp = STATIC_PTR_CAST<GASS::IComponent>(comp_iter.getNext());
		GASS::BaseSceneComponentPtr temp_comp = DYNAMIC_PTR_CAST<GASS::BaseSceneComponent>(comp);
		if(temp_comp)
		{
			const std::string name =  temp_comp->GetRTTI()->GetClassName();
			ScriptComponentWrapper* wrapper = new ScriptComponentWrapper();
			wrapper->SetComponent(temp_comp);
			m_Components[name] = wrapper;
			//ComponentPtr template_comp = temp_comp->CreateCopy();
			//AddComponent(template_comp);
		}
	}


}