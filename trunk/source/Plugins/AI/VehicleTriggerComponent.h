
#ifndef VEHICLE_TRIGGER_COMPONENT
#define VEHICLE_TRIGGER_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include <set>

namespace GASS
{

	enum TriggerAreaType
	{
		TAT_ELLIPSOID,
		TAT_ELLIPSE,
		TAT_BOX,
		TAT_RECTANGLE,
	};

	START_ENUM_BINDER(TriggerAreaType,TriggerAreaTypeBinder)
		BIND(TAT_ELLIPSOID)
		BIND(TAT_ELLIPSE)
		BIND(TAT_BOX)
		BIND(TAT_RECTANGLE)
	END_ENUM_BINDER(TriggerAreaType,TriggerAreaTypeBinder)


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

		TriggerAreaTypeBinder GetAreaType() const {return m_AreaType;}
		Vec3 GetAreaSize() const {return m_AreaSize;}
		void SetAreaType(TriggerAreaTypeBinder at) {m_AreaType = at; _UpdateArea();}
		void SetAreaSize(const Vec3 &as) {m_AreaSize = as; _UpdateArea();}
		void _UpdateArea();
		void OnScenarioEvent(ScenarioStateRequestPtr message);
		void OnTransformation(TransformationNotifyMessagePtr message);
		bool _IsPointInside(const Vec3 &point);
		void _OnPlay();
		bool m_Initialized;
		void SetActive(bool value);
		
		void Reset();
		typedef std::vector<ActivationObject> ActivationVector;
		ActivationVector m_AllActivators;
		bool m_Active;
		bool m_Update;
		TriggerAreaTypeBinder m_AreaType;
		Vec3 m_AreaSize;
		Mat4 m_InverseTransform;
	};
	typedef SPTR<VehicleTriggerComponent> VehicleTriggerComponentPtr;
}
#endif