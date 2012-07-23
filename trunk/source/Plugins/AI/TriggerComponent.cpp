#include "TriggerComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"


namespace GASS
{
	TriggerComponent::TriggerComponent(void) :
		m_Initialized(false)
	{
		
	}	

	TriggerComponent::~TriggerComponent(void)
	{

	}

	void TriggerComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("TriggerComponent",new Creator<TriggerComponent, IComponent>);
		RegisterProperty<SceneObjectID>("ListenerID", &GetListenerID, &SetListenerID);
	}

	void TriggerComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(TriggerComponent::OnLoad,LoadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(TriggerComponent::OnUnload,UnloadComponentsMessage,1));
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<AISceneManager>()->Register(listener);
	}

	SceneObjectID  TriggerComponent::GetListenerID() const
	{
		return m_ListenerID;
	}

	void TriggerComponent::SetListenerID(const SceneObjectID &id)
	{
		m_ListenerID = id;
		if(m_Initialized)
		{
			SceneObjectPtr ret;
			std::vector<SceneObjectPtr> objects;
			m_AllListeners.clear();
			GetSceneObject()->GetScene()->GetRootSceneObject()->GetChildrenByID(objects,id,false,true);
			for(size_t i = 0; i < objects.size(); i++)
			{
				TriggerListener listener;
				listener.Inside = false;
				listener.Object = objects[i];
				m_AllListeners.push_back(listener);
			}
		}
	}

	void TriggerComponent::OnLoad(LoadComponentsMessagePtr message)
	{
		m_Initialized = true;
		SetListenerID(m_ListenerID);
	}

	void TriggerComponent::OnUnload(MessagePtr message)
	{
		m_Initialized = false;
	}

	void TriggerComponent::RegisterListener(SceneObjectPtr object)
	{
		TriggerListener listener;
		listener.Inside = false;
		listener.Object = object;
		m_AllListeners.push_back(listener);
	}

	void TriggerComponent::UnregisterListener(SceneObjectPtr object)
	{
		ListenerVector::iterator iter = m_AllListeners.begin();
		while(iter != m_AllListeners.end())
		{
			SceneObjectPtr list_obj((*iter).Object,boost::detail::sp_nothrow_tag());
			if(object == list_obj)
			{
				iter = m_AllListeners.erase(iter);
			}
			else
				iter++;
		}
	}

	void TriggerComponent::SceneManagerTick(double delta_time)
	{
		ListenerVector::iterator  iter = m_AllListeners.begin();
		while(iter != m_AllListeners.end())
		{
			SceneObjectPtr obj((*iter).Object,boost::detail::sp_nothrow_tag());
			if(obj)
			{
				const Vec3 pos = obj->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
				ShapePtr shape = GetSceneObject()->GetFirstComponentByClass<IShape>();
				if(shape)
				{
					if(shape->IsPointInside(pos))
					{
				
						if(!(*iter).Inside)
						{
						//enter
						(*iter).Inside = true;
						GetSceneObject()->PostMessage(MessagePtr(new TriggerEnterMessage(obj)));
						}
					}	
					else
					{
						if((*iter).Inside)
						{
							(*iter).Inside = false;
							GetSceneObject()->PostMessage(MessagePtr(new TriggerExitMessage(obj)));
						}
					}
				}
				iter++;
			}
			else
			{
				iter = m_AllListeners.erase(iter);
			}
		}
	}
}
