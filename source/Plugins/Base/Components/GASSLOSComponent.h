
#ifndef GASS_LOS_COMPONENT
#define GASS_LOS_COMPONENT

#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSFilePath.h"
#include "Sim/GASS.h"

namespace GASS
{
	class LOSComponent :  public Reflection<LOSComponent,BaseSceneComponent> 
	{
	public:
		LOSComponent(void);
		~LOSComponent(void) override;
		static void RegisterReflection();
		void OnInitialize() override;
		void SceneManagerTick(double delta_time) override;
	protected:
		void OnTransChanged(TransformationChangedEventPtr);
	private:
		void SetCalculate(bool value);
		bool GetCalculate() const {return false;}
		bool _CheckLOS(const Vec3 &start_pos, const Vec3 &end_pos, GASS::CollisionSceneManagerPtr col_sm) const;
		Float _GetHeight(const Vec3 &pos, GASS::CollisionSceneManagerPtr col_sm) const;
		
		void SetFOV(Float value) {m_FOV = value; SetCalculate(true);}
		Float GetFOV() const {return m_FOV;}
		void SetRadius(Float value) {m_Radius = value;SetCalculate(true);}
		Float GetRadius() const {return m_Radius;}
		void SetSampleDist(Float value) {m_SampleDist = value;SetCalculate(true);}
		Float GetSampleDist() const {return m_SampleDist;}

		void SetSourceOffset(Float value) {m_SourceOffset = value;SetCalculate(true);}
		Float GetSourceOffset() const {return m_SourceOffset;}

		void SetTargetOffset(Float value) {m_TargetOffset = value;SetCalculate(true);}
		Float GetTargetOffset() const {return m_TargetOffset;}

		void SetTransparency(Float value) {m_Transparency = value;SetCalculate(true);}
		Float GetTransparency() const {return m_Transparency;}
		
		
		bool m_Initialized;
		Vec3 m_Position;
		Vec3 m_ViewDir;
		Float m_FOV;
		Float m_Radius;
		Float m_SampleDist;
		Float m_SourceOffset;
		Float m_TargetOffset;
		Float m_Transparency;
		bool m_Debug;
		bool m_AutUpdateOnTransform;

	};
	typedef GASS_SHARED_PTR<LOSComponent> LOSComponentPtr;
}
#endif