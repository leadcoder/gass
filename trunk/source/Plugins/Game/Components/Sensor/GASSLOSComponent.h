
#ifndef SENSOR_COMPONENT
#define SENSOR_COMPONENT

#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSFilePath.h"
#include "Sim/GASS.h"

#include "Plugins/Game/GameMessages.h"
#include "Plugins/Game/PlatformType.h"

namespace GASS
{
	class SignatureComponent;
	typedef WPTR<SignatureComponent> SignatureComponentWeakPtr;

	class LOSComponent :  public Reflection<LOSComponent,BaseSceneComponent> 
	{
	public:
		LOSComponent(void);
		~LOSComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void SceneManagerTick(double delta_time);
	protected:
		void OnTransChanged(TransformationChangedEventPtr);
	private:
		void Calculate(bool value);
		bool _CheckLOS(const Vec3 &start_pos, const Vec3 &end_pos) const;
		void _BuildMesh();
		ADD_PROPERTY(Float,Radius)
		ADD_PROPERTY(Float,SampleDist)
		ADD_PROPERTY(bool,Debug)
		

		void Update();
		bool m_Initialized;
		Vec3 m_Position;
	};
	typedef SPTR<LOSComponent> LOSComponentPtr;
}
#endif