
#ifndef SENSOR_COMPONENT
#define SENSOR_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include <set>

namespace GASS
{
	struct TriggerListener
	{
		SceneObjectWeakPtr Object;
		bool Inside;
	};
	
	class TriggerComponent :  public Reflection<TriggerComponent,BaseSceneComponent> 
	{
	public:
		TriggerComponent(void);
		~TriggerComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void SceneManagerTick(double delta_time);
		void RegisterListener(SceneObjectPtr object);
		void UnregisterListener(SceneObjectPtr object);
	protected:
		bool IsInside(SceneObjectWeakPtr obj) const;
		bool RemoveInside(SceneObjectWeakPtr obj);

	private:
		SceneObjectID  GetListenerID() const;
		void SetListenerID(const SceneObjectID &id);
	
		bool m_Initialized;
		typedef std::vector<TriggerListener> ListenerVector;
		ListenerVector m_AllListeners;
		SceneObjectID m_ListenerID;
	};

	typedef SPTR<TriggerComponent> TriggerComponentPtr;
}
#endif