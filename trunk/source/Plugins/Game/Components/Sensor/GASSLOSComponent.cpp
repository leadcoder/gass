#include "GASSLOSComponent.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"

#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSimSystemManager.h"
#include "Plugins/Game/Components/Sensor/SignatureComponent.h"
#include "Plugins/Game/GameSceneManager.h"

namespace GASS
{
	LOSComponent::LOSComponent(void) :
		m_Initialized(false),
		m_Radius(100),
		m_SampleDist(1),
		m_Debug(false),
		m_Position(0,0,0)
	{
		
	}	

	LOSComponent::~LOSComponent(void)
	{

	}

	void LOSComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("LOSComponent",new Creator<LOSComponent, IComponent>);
		REG_PROPERTY(Float,Radius,LOSComponent)
		REG_PROPERTY(bool,Debug,LOSComponent)
		REG_PROPERTY(Float,SampleDist,LOSComponent)
		
	}

	void LOSComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(LOSComponent::OnTransChanged,TransformationChangedEvent,0));
		//GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS( LOSComponent::OnSceneObjectCreated,PostSceneObjectInitializedEvent,0));
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<GameSceneManager>()->Register(listener);
		m_Initialized = true;
	}

	void LOSComponent::Calculate(bool value)
	{
		if(!m_Initialized)
			return;

		ScenePtr scenario = GetSceneObject()->GetScene();
		Vec3 east(1,0,0);
		Vec3 north(0,0,1);

		//if(m_Type =="grid")
		{
			
		//m_Result.clear();
			int calc_samples = m_Radius/m_SampleDist;
		//	m_Result.resize(calc_samples*2);

			/*std::stringstream sstream;
			sstream << std::fixed << std::setprecision(2) <<  "Calculate grid with " << calc_samples*calc_samples*4 << " sample points\r\n";
			std::string std_string = sstream.str();
			MessagePtr calc_message(new TextFeedbackMessage(std_string));
			EditorManager::GetPtr()->GetMessageManager()->SendImmediate(calc_message);
			*/
			for(int i = -calc_samples; i < calc_samples; i++)
			{
				//progress message
				/*if(i%10 == 0)
				{
					std::stringstream sstream;
					sstream << std::fixed << std::setprecision(2) <<  (i+calc_samples)*calc_samples*2 << " samples done\r\n";
					std::string std_string = sstream.str();
					MessagePtr calc_message(new TextFeedbackMessage(std_string));
					EditorManager::GetPtr()->GetMessageManager()->SendImmediate(calc_message);
				}*/
				//std::cout << (i+calc_samples)*calc_samples*2 << " samples done" << std::endl;
				for(int j = -calc_samples; j < calc_samples; j++)
				{
					Vec3 end_pos = m_Position + east*(i*m_SampleDist) + north*(j*-m_SampleDist);
					if(i == 0 && j==0)
						end_pos = m_Position + east*(i*m_SampleDist) + north*-m_SampleDist;
					bool los = _CheckLOS(m_Position,end_pos);
				}
			}
		}
	}

	bool LOSComponent::_CheckLOS(const Vec3 &start_pos, const Vec3 &end_pos) const
	{
		return true;
	}

	void LOSComponent::_BuildMesh()
	{
		if(!m_Initialized)
			return;

		int calc_index = 0;

		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);

		//ScenePtr scenario = GetSceneObject()->GetScene();
		Vec3 offset(0,20,0);
		//if(m_ShowPoints)
		{
			sub_mesh_data->MaterialName = "NodeMaterialNoTexture";
			sub_mesh_data->Type = POINT_LIST;
			/*for(int i = 0; i < m_Result.size(); i++)
			{
				for(int j = 0; j < m_Result[i].size(); j++)
				{
					sub_mesh_data->VertexVector.push_back(GetResult(i,j));
					sub_mesh_data->IndexVector.push_back(calc_index++);
				}
			}*/
		}
		/*else if(m_Type =="grid")
		{
			m_CoverageMesh->Material = "WhiteTransparentNoLightingNoDepthCheck";
			m_CoverageMesh->Type = TRIANGLE_LIST;
			m_CoverageMesh->VertexVector.clear();
			m_CoverageMesh->IndexVector.clear();

			for(int i = 0; i < ((int) m_Result.size()) - 1; i++)
			{
				for(int j = 0; j < ((int) m_Result[i].size()) - 1; j++)
				{
					m_CoverageMesh->VertexVector.push_back(GetResult(i,j,offset,out_effect));
					m_CoverageMesh->VertexVector.push_back(GetResult(i,j+1,offset,out_effect));
					m_CoverageMesh->VertexVector.push_back(GetResult(i+1,j,offset,out_effect));

					m_CoverageMesh->VertexVector.push_back(GetResult(i+1,j,offset,out_effect));
					m_CoverageMesh->VertexVector.push_back(GetResult(i,j+1,offset,out_effect));
					m_CoverageMesh->VertexVector.push_back(GetResult(i+1,j+1,offset,out_effect));


					m_CoverageMesh->IndexVector.push_back(calc_index++);
					m_CoverageMesh->IndexVector.push_back(calc_index++);
					m_CoverageMesh->IndexVector.push_back(calc_index++);
					m_CoverageMesh->IndexVector.push_back(calc_index++);
					m_CoverageMesh->IndexVector.push_back(calc_index++);
					m_CoverageMesh->IndexVector.push_back(calc_index++);
				}
			}
		}
		else
		{
			m_CoverageMesh->Material = "WhiteTransparentNoLightingNoDepthCheck";
			m_CoverageMesh->Type = TRIANGLE_LIST;
			m_CoverageMesh->VertexVector.clear();
			m_CoverageMesh->IndexVector.clear();

			int size_i  = m_Result.size();
			int loop_i  = m_Result.size();
			if(!(m_DirectionInterval.x <= 0 && m_DirectionInterval.y >= 360))
				loop_i--;
			for(int i = 0; i < loop_i; i++)
			{
				int size_j = m_Result[i].size();
				int loop_j = m_Result[i].size();
				//if(!(m_DirectionInterval.x <= 0 && m_DirectionInterval.y >= 360))
				loop_j--;
				for(int j = 0; j < loop_j; j++)
				{
					m_CoverageMesh->VertexVector.push_back(GetResult((i+1)%size_i,j,offset,out_effect));
					m_CoverageMesh->VertexVector.push_back(GetResult(i,(j+1)%size_j,offset,out_effect));
					m_CoverageMesh->VertexVector.push_back(GetResult(i,j,offset,out_effect));
					m_CoverageMesh->VertexVector.push_back(GetResult((i+1)%size_i,(j+1)%size_j,offset,out_effect));
					m_CoverageMesh->VertexVector.push_back(GetResult(i,(j+1)%size_j,offset,out_effect));
					m_CoverageMesh->VertexVector.push_back(GetResult((i+1)%size_i,j,offset,out_effect));


					m_CoverageMesh->IndexVector.push_back(calc_index++);
					m_CoverageMesh->IndexVector.push_back(calc_index++);
					m_CoverageMesh->IndexVector.push_back(calc_index++);
					m_CoverageMesh->IndexVector.push_back(calc_index++);
					m_CoverageMesh->IndexVector.push_back(calc_index++);
					m_CoverageMesh->IndexVector.push_back(calc_index++);
				}
			}
		}*/
		ManualMeshDataRequestPtr message(new ManualMeshDataRequest(mesh_data));
		GetSceneObject()->PostRequest(message);
	}


		
	void LOSComponent::OnTransChanged(TransformationChangedEventPtr message)
	{
		m_Position = message->GetPosition();
	}

	void LOSComponent::SceneManagerTick(double delta_time)
	{
		//debug?
		if(m_Debug)
		{
			//GetSceneObject()->PostRequest(DrawCircleRequestPtr(new DrawCircleRequest(m_Position,m_DefaultMaxDetectionDistance,Vec4(1,0,0,1),20,false)););
		}
	}
}
