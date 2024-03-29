#include "GASSLOSComponent.h"

#include <memory>
#include "Core/Math/GASSQuaternion.h"
#include "Core/Math/GASSMath.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/Interface/GASSIManualMeshComponent.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"

namespace GASS
{
	LOSComponent::LOSComponent(void) : 
		m_Position(0,0,0),
		m_ViewDir(1,0,0)
		
	{

	}	

	LOSComponent::~LOSComponent(void)
	{

	}

	void LOSComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<LOSComponent>();
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("LOSComponent", OF_VISIBLE));
		RegisterGetSet("Radius", &GASS::LOSComponent::GetRadius, &GASS::LOSComponent::SetRadius,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("SampleDist", &GASS::LOSComponent::GetSampleDist, &GASS::LOSComponent::SetSampleDist,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("FOV", &GASS::LOSComponent::GetFOV, &GASS::LOSComponent::SetFOV,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("TargetOffset", &GASS::LOSComponent::GetTargetOffset, &GASS::LOSComponent::SetTargetOffset,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("SourceOffset", &GASS::LOSComponent::GetSourceOffset, &GASS::LOSComponent::SetSourceOffset,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("Calculate", &GASS::LOSComponent::GetCalculate, &GASS::LOSComponent::SetCalculate,PF_VISIBLE | PF_EDITABLE,"");
		RegisterMember("AutUpdateOnTransform", &GASS::LOSComponent::m_AutUpdateOnTransform,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("Transparency", &GASS::LOSComponent::GetTransparency, &GASS::LOSComponent::SetTransparency,PF_VISIBLE | PF_EDITABLE,"");
	}

	void LOSComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(LOSComponent::OnTransChanged,TransformationChangedEvent,0));
		SceneManagerListenerPtr listener = shared_from_this();
		m_Initialized = true;
	}

	void LOSComponent::SetCalculate(bool /*value*/)
	{
		if(!m_Initialized)
			return;

		ScenePtr scene = GetSceneObject()->GetScene();
		Vec3 east(1,0,0);
		Vec3 north(0,0,1);

		CollisionSceneManagerPtr col_sm = scene->GetFirstSceneManagerByClass<GASS::ICollisionSceneManager>();
		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);

		sub_mesh_data->MaterialName = "NodeMaterialNoTexture";
		//sub_mesh_data->MaterialName = "WhiteTransparentNoLighting";

		sub_mesh_data->Type = POINT_LIST;
		//sub_mesh_data->Type = LINE_LIST;

		int calc_samples = static_cast<int>(m_Radius / m_SampleDist);

		for(int i = -calc_samples; i < calc_samples; i++)
		{
			for(int j = -calc_samples; j < calc_samples; j++)
			{
				Vec3 dir = east*(i*m_SampleDist) + north*(j*-m_SampleDist);
				if(dir.Length() < m_Radius)
				{
					dir.Normalize();
					Float angle = Math::Rad2Deg(acos(Vec3::Dot(dir,m_ViewDir)));
					if(fabs(angle) < m_FOV  || m_FOV == 0)
					{
						Vec3 end_pos = m_Position + east*(i*m_SampleDist) + north*(j*-m_SampleDist);
						//only do radial
						if(i == 0 && j==0)
							end_pos = m_Position + east*(i*m_SampleDist) + north*-m_SampleDist;

						end_pos.y = GetHeight(end_pos,col_sm) + m_TargetOffset;

						bool los = CheckLos(m_Position, end_pos, col_sm);
						sub_mesh_data->PositionVector.push_back(end_pos);
						if(los)
						{
							sub_mesh_data->ColorVector.emplace_back(0,1,0,m_Transparency);
						}
						else
						{
							sub_mesh_data->ColorVector.emplace_back(1,0,0,m_Transparency);
						}
					}
				}
			}
		}
		
		auto los_obj = GetSceneObject()->GetChildByID("LOS_GEOM");
		los_obj->GetFirstComponentByClass<IManualMeshComponent>()->SetMeshData(*mesh_data);
	}

	bool LOSComponent::CheckLos(const Vec3 &start_pos, const Vec3 &end_pos, CollisionSceneManagerPtr col_sm) const
	{
		Vec3 ray_direction = end_pos - start_pos;
		CollisionResult result;
		//col_sm->Raycast(start_pos,ray_direction,(GASS::GeometryFlags)((int) GEOMETRY_FLAG_GROUND | (int) GEOMETRY_FLAG_STATIC_OBJECT ),result);
		col_sm->Raycast(start_pos,ray_direction,GEOMETRY_FLAG_SCENE_OBJECTS,result);

		return !result.Coll;
	}

	Float LOSComponent::GetHeight(const Vec3 &pos, CollisionSceneManagerPtr col_sm) const
	{
		if(col_sm)
		{
			Vec3 ray_start(pos.x  , 10000.0, pos.z);
			Vec3 ray_direction(0.0 ,-20000.0 , 0.0);
			CollisionResult result;
			//col_sm->Raycast(ray_start,ray_direction, (GASS::GeometryFlags)((int) GEOMETRY_FLAG_GROUND | (int) GEOMETRY_FLAG_STATIC_OBJECT ),result);
			col_sm->Raycast(ray_start,ray_direction, GEOMETRY_FLAG_SCENE_OBJECTS ,result);
			if(result.Coll)
			{
				return result.CollPosition.y;
			}
		}
		return 0;
	}

	void LOSComponent::OnTransChanged(TransformationChangedEventPtr message)
	{
		m_Position = message->GetPosition();
		m_ViewDir = message->GetRotation().GetRotationMatrix().GetZAxis();
		//Calculate(true);
		m_Position.y += m_SourceOffset;
		if(m_AutUpdateOnTransform)
			SetCalculate(true);
	}

	void LOSComponent::SceneManagerTick(double /*delta_time*/)
	{
		//debug?
		if(m_Debug)
		{
			//GetSceneObject()->PostRequest(DrawCircleRequestPtr(new DrawCircleRequest(m_Position,m_DefaultMaxDetectionDistance,Vec4(1,0,0,1),20,false)););
		}
	}
}
