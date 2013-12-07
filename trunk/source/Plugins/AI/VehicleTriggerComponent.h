
#ifndef VEHICLE_TRIGGER_COMPONENT
#define VEHICLE_TRIGGER_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include <set>

namespace GASS
{
	struct ActivationObject
	{
		SceneObjectWeakPtr Object;
		bool Inside;
	};
	
	class VehicleTriggerComponent :  public Reflection<VehicleTriggerComponent,BaseSceneComponent> 
	{
	public:
		VehicleTriggerComponent(void);
		~VehicleTriggerComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void SceneManagerTick(double delta_time);
		void RegisterListener(SceneObjectPtr object);
		void UnregisterListener(SceneObjectPtr object);
		bool IsActive() const;
		std::vector<SceneObjectPtr>  _GetGroupEnumeration() const;
		std::vector<SceneObjectPtr>  _GetControllerEnumeration() const;
	protected:
		bool IsInside(SceneObjectWeakPtr obj) const;
		bool RemoveInside(SceneObjectWeakPtr obj);
		
	private:
		ADD_PROPERTY(bool,Repeatedly);
		ADD_PROPERTY(bool,Present);
		ADD_PROPERTY(bool,Strict);
		ADD_PROPERTY(std::vector<SceneObjectRef>,ActivationGroups);
		ADD_PROPERTY(std::vector<SceneObjectRef>,ActivationControllers);
		void OnScenarioEvent(ScenarioStateRequestPtr message);
		void _OnPlay();
		bool m_Initialized;
		void SetActive(bool value);
		
		void Reset();
		typedef std::vector<ActivationObject> ActivationVector;
		ActivationVector m_AllActivators;
		bool m_Active;
		bool m_Update;
		
	};
	typedef SPTR<VehicleTriggerComponent> VehicleTriggerComponentPtr;
}
#endif