#include "TerrainDeformTool.h"

#include <memory>
#include "MouseToolController.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Modules/Editor/Components/PaintGizmoComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObjectTemplateManager.h"
#include "Core/Utils/GASSException.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSimSystemManager.h"
#include "Plugins/PagedGeometry/PGMessages.h"

namespace GASS
{

	TerrainDeformTool::TerrainDeformTool(MouseToolController* controller): m_MouseIsDown(false),
		m_CursorPos(0,0,0),
		m_Controller(controller),
		m_BrushSize(116),
		m_BrushInnerSize(90),
		m_Intensity(1),
		m_Noise(0),
		m_TEM(TEM_DEFORM),
		m_InvertBrush(1),
		m_ActiveLayer(TL_1),
		m_Active(false)
	{
		controller->GetEditorSceneManager()->GetScene()->RegisterForMessage(REG_TMESS(TerrainDeformTool::OnSelectionChanged,EditorSelectionChangedEvent,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(TerrainDeformTool::OnInput,ControllSettingsMessage,0));
	}

	TerrainDeformTool::~TerrainDeformTool()
	{

	}

	void TerrainDeformTool::Update(double delta)
	{
		if(m_MouseIsDown)
		{
			float intensity = m_Intensity * m_InvertBrush* static_cast<float>(delta);

			//get the one and only terrain
			HeightmapTerrainComponentPtr terrain = m_Controller->GetEditorSceneManager()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<IHeightmapTerrainComponent>(true);
			if(terrain)
			{
				auto bsc = GASS_DYNAMIC_PTR_CAST<Component>(terrain);
				SceneObjectPtr terrain_group = bsc->GetSceneObject()->GetParentSceneObject();

				switch(m_TEM)
				{
				case TEM_DEFORM:
					//terrain_group->PostRequest(std::make_shared<TerrainHeightModifyRequest>(TerrainHeightModifyRequest::MT_DEFORM,m_CursorPos,m_BrushSize, m_BrushInnerSize,intensity,m_Noise));
					break;
				case TEM_FLATTEN:
					//terrain_group->PostRequest(std::make_shared<TerrainHeightModifyRequest>(TerrainHeightModifyRequest::MT_FLATTEN,m_CursorPos,m_BrushSize, m_BrushInnerSize,intensity,m_Noise));
					break;
				case TEM_SMOOTH:
					//terrain_group->PostRequest(std::make_shared<TerrainHeightModifyRequest>(TerrainHeightModifyRequest::MT_SMOOTH,m_CursorPos,m_BrushSize, m_BrushInnerSize,intensity,m_Noise));
					break;
				case TEM_LAYER_PAINT:
					//terrain_group->PostRequest(std::make_shared<TerrainPaintRequest>(m_CursorPos,m_BrushSize, m_BrushInnerSize,m_ActiveLayer,intensity,m_Noise));
					break;
				case TEM_VEGETATION_PAINT:
					break;
				}
			}

			if(m_TEM == TEM_VEGETATION_PAINT)
			{
				for(size_t i = 0; i< m_Selection.size(); i++)
				{
					SceneObjectPtr selected = m_Selection[i].lock();
					if(selected)
						selected->PostRequest(std::make_shared<GrassPaintMessage>(m_CursorPos, m_BrushSize, m_BrushInnerSize, intensity, m_Noise));
				}
			}
		}
	}

	void TerrainDeformTool::MouseMoved(const MouseData &/*data*/, const SceneCursorInfo &info)
	{
		m_CursorPos = info.m_3DPos;
		SceneObjectPtr gizmo = GetOrCreateGizmo();
		if(gizmo)
		{
			gizmo->GetFirstComponentByClass<ILocationComponent>()->SetWorldPosition(info.m_3DPos);
		}
	}

	void TerrainDeformTool::MouseDown(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
	{
		m_MouseIsDown = true;
	}

	void TerrainDeformTool::MouseUp(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
	{
		m_MouseIsDown = false;
	}

	void TerrainDeformTool::Stop()
	{
		SetGizmoVisiblity(false);
		m_Active = false;
	}

	void TerrainDeformTool::Start()
	{
		SetGizmoVisiblity(true);
		m_Active = true;
	}

	SceneObjectPtr TerrainDeformTool::GetOrCreateGizmo()
	{
		SceneObjectPtr gizmo = m_MasterGizmoObject.lock();
		if(!gizmo &&  m_Controller->GetEditorSceneManager()->GetScene())
		{
			ScenePtr scene = m_Controller->GetEditorSceneManager()->GetScene();
			std::string gizmo_name = "PaintGizmo";
			gizmo = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(gizmo_name, m_Controller->GetEditorSceneManager()->GetScene()->GetRootSceneObject());
			m_MasterGizmoObject = gizmo;
			PaintGizmoComponentPtr comp = gizmo->GetFirstComponentByClass<PaintGizmoComponent>();
			if(!comp)
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find  PaintGizmoComponent", "TerrainDeformTool::GetOrCreateGizmo");

			comp->SetSize(m_BrushSize*0.5f);
			comp->BuildMesh();
		}
		return gizmo;
	}

	void TerrainDeformTool::SetGizmoVisiblity(bool value)
	{
		SceneObjectPtr gizmo = GetOrCreateGizmo();
		if(gizmo)
		{
			auto iter = gizmo->GetChildren();
			while (iter.hasMoreElements())
			{
				auto child = iter.getNext();
				child->GetFirstComponentByClass<ILocationComponent>()->SetVisible(value);
			}
		}
	}

	void TerrainDeformTool::OnSelectionChanged(EditorSelectionChangedEventPtr message)
	{
		m_Selection = message->m_Selection;
	}

	void TerrainDeformTool::SendMessageRec(SceneObjectPtr obj,SceneObjectRequestMessagePtr msg)
	{
		obj->PostRequest(msg);
		auto iter = obj->GetChildren();
		while(iter.hasMoreElements())
		{
			auto child = iter.getNext();
			SendMessageRec(child,msg);
		}
	}

	void TerrainDeformTool::SetBrushSize(float value)
	{
		m_BrushSize = value;
		SceneObjectPtr gizmo = GetOrCreateGizmo();
		if(gizmo)
		{
			PaintGizmoComponentPtr comp = gizmo->GetFirstComponentByClass<PaintGizmoComponent>();
			comp->SetSize(m_BrushSize*0.5f);
			comp->BuildMesh();
		}
	}

	void TerrainDeformTool::SetBrushInnerSize(float value)
	{
		m_BrushInnerSize = value;
		SceneObjectPtr gizmo = GetOrCreateGizmo();
		if(gizmo)
		{
			PaintGizmoComponentPtr comp = gizmo->GetFirstComponentByClass<PaintGizmoComponent>();
			comp->SetInnerSize(m_BrushInnerSize*0.5f);
			comp->BuildMesh();
		}
	}

	void TerrainDeformTool::SetIntensity(float value)
	{
		m_Intensity = value;
	}

	void TerrainDeformTool::SetNoise(float value)
	{
		m_Noise= value;
	}

	/*void TerrainDeformTool::SetLayerTexture(const std::string &texture, float tiling)
	{
		SceneObjectPtr selected = m_SelectedObject.lock();
		if(selected)
		{
			TerrainComponentPtr terrain = selected->GetFirstComponentByClass<ITerrainComponent>();
			if(terrain)
			{
				selected->PostRequest(MessagePtr(new TerrainLayerRequest(m_ActiveLayer,texture,tiling)));
			}
		}

		HeightmapTerrainComponentPtr terrain = m_Controller->GetEditorSceneManager()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<IHeightmapTerrainComponent>(true);
		if(terrain)
		{
			ComponentPtr bsc = GASS_DYNAMIC_PTR_CAST <Component>(terrain);
			bsc->GetSceneObject()->PostRequest(MessagePtr(new TerrainLayerRequest(m_ActiveLayer,texture,tiling)));
		}
	}*/


	void TerrainDeformTool::OnInput(ControllSettingsMessagePtr message)
	{
		//Check settings
		if("EditorInputSettings" != message->GetSettings())
			return;

		if(m_Active)
		{
			std::string name = message->GetController();
			float value = message->GetValue();
			if(name == "ChangeBrushSize" && fabs(value)  > 0.5)
			{
				//std::cout << value << "\n";
				if(value > 0.5)
					m_BrushSize += m_BrushSize*0.3f;
				else if(value < -0.5)
					m_BrushSize -= m_BrushSize*0.3f;
				SetBrushSize(m_BrushSize);

			}
			if(name == "InvertBrush")
			{
				if(value > 0.5)
					m_InvertBrush = -1;
				else
					m_InvertBrush = 1;
			}
		}
	}
}
