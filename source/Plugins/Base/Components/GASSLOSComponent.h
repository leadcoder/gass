
#ifndef GASS_LOS_COMPONENT
#define GASS_LOS_COMPONENT

#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSFilePath.h"
#include "Sim/GASS.h"

namespace GASS
{
	class LOSComponent :  public Reflection<LOSComponent,Component> 
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
		bool CheckLos(const Vec3 &start_pos, const Vec3 &end_pos, GASS::CollisionSceneManagerPtr col_sm) const;
		Float GetHeight(const Vec3 &pos, GASS::CollisionSceneManagerPtr col_sm) const;
		
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
		
		
		bool m_Initialized{false};
		Vec3 m_Position;
		Vec3 m_ViewDir;
		Float m_FOV{40};
		Float m_Radius{100};
		Float m_SampleDist{1};
		Float m_SourceOffset{1.7};
		Float m_TargetOffset{1.7};
		Float m_Transparency{0.2};
		bool m_Debug{false};
		bool m_AutUpdateOnTransform{true};

	};
	using LOSComponentPtr = std::shared_ptr<LOSComponent>;
}
#endif