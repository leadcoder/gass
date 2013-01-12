#include "TerrainDeformTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Modules/Editor/Components/PaintGizmoComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"
#include "Core/Utils/GASSException.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/GASSSimSystemManager.h"
#include "Plugins/PagedGeometry/PGMessages.h"

namespace GASS
{

	TerrainDeformTool::TerrainDeformTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller),m_BrushSize(116),m_BrushInnerSize(90), m_Intensity(1),m_Noise(0), m_TEM(TEM_DEFORM),m_InvertBrush(1),m_ActiveLayer(TL_1)
	{
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(TerrainDeformTool::OnSceneObjectSelected,ObjectSelectionChangedEvent,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(TerrainDeformTool::OnInput,ControllSettingsMessage,0));
	}

	TerrainDeformTool::~TerrainDeformTool()
	{

	}

	void TerrainDeformTool::MouseMoved(const CursorInfo &info)
	{
		int from_id = (int) this;
		//SceneObjectPtr selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		if(m_MouseIsDown)// && selected)
		{
			//if(selected)
			{
				float intensity = m_Intensity*m_InvertBrush*m_Controller->GetDeltaTime();
				//TerrainComponentPtr terrain = selected->GetFirstComponentByClass<ITerrainComponent>();
				TerrainComponentPtr terrain = m_Controller->GetEditorSceneManager()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<ITerrainComponent>(true);
				if(terrain)
				{
					BaseSceneComponentPtr bsc = boost::dynamic_pointer_cast<BaseSceneComponent>(terrain);
					
					switch(m_TEM)
					{
					case TEM_DEFORM:
						bsc->GetSceneObject()->PostMessage(MessagePtr(new TerrainHeightModifyMessage(TerrainHeightModifyMessage::MT_DEFORM,info.m_3DPos,m_BrushSize, m_BrushInnerSize,intensity,m_Noise)));
						break;
					case TEM_FLATTEN:
						bsc->GetSceneObject()->PostMessage(MessagePtr(new TerrainHeightModifyMessage(TerrainHeightModifyMessage::MT_FLATTEN,info.m_3DPos,m_BrushSize, m_BrushInnerSize,intensity,m_Noise)));
						break;
					case TEM_SMOOTH:
						bsc->GetSceneObject()->PostMessage(MessagePtr(new TerrainHeightModifyMessage(TerrainHeightModifyMessage::MT_SMOOTH,info.m_3DPos,m_BrushSize, m_BrushInnerSize,intensity,m_Noise)));
						break;
					case TEM_LAYER_PAINT:
						bsc->GetSceneObject()->PostMessage(MessagePtr(new TerrainPaintMessage(info.m_3DPos,m_BrushSize, m_BrushInnerSize,m_ActiveLayer,intensity,m_Noise)));
						break;
					}
				}
				if(m_TEM == TEM_VEGETATION_PAINT)
				{
					SceneObjectPtr selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
					if(selected)
						selected->PostMessage(MessagePtr(new GrassPaintMessage(info.m_3DPos,m_BrushSize, m_BrushInnerSize,intensity,m_Noise)));
				}
			}
			//GASS::MessagePtr paint_msg(new PaintMessage(info.m_3DPos,selected,from_id));
			//SimEngine::Get().GetSimSystemManager()->SendImmediate(paint_msg);
		}
		SceneObjectPtr gizmo = GetOrCreateGizmo();
		if(gizmo)
		{
			GASS::MessagePtr pos_msg(new GASS::WorldPositionMessage(info.m_3DPos,from_id));
			gizmo->PostMessage(pos_msg);
		}
	}

	void TerrainDeformTool::MouseDown(const CursorInfo &info)
	{
		m_MouseIsDown = true;
	}

	void TerrainDeformTool::MouseUp(const CursorInfo &info)
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
		SceneObjectPtr gizmo(m_MasterGizmoObject,boost::detail::sp_nothrow_tag());
		if(!gizmo &&  m_Controller->GetEditorSceneManager()->GetScene())
		{
			ScenePtr scene = m_Controller->GetEditorSceneManager()->GetScene();
			std::string gizmo_name = "PaintGizmo";
			gizmo = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(gizmo_name,m_Controller->GetEditorSceneManager()->GetScene()->GetRootSceneObject());
			m_MasterGizmoObject = gizmo;
			PaintGizmoComponentPtr comp = gizmo->GetFirstComponentByClass<PaintGizmoComponent>();
			if(!comp)
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find  PaintGizmoComponent", "TerrainDeformTool::GetOrCreateGizmo");

			comp->SetSize(m_BrushSize*0.5);
			comp->BuildMesh();
		}
		return gizmo;
	}

	void TerrainDeformTool::SetGizmoVisiblity(bool value)
	{
		SceneObjectPtr gizmo = GetOrCreateGizmo();
		if(gizmo)
		{
			int from_id = (int) this;
			MessagePtr vis_msg(new GASS::VisibilityMessage(value,from_id));
			SendMessageRec(gizmo,vis_msg);
		}
	}

	void TerrainDeformTool::OnSceneObjectSelected(ObjectSelectionChangedEventPtr message)
	{
		m_SelectedObject = message->GetSceneObject();
	}

	void TerrainDeformTool::SendMessageRec(SceneObjectPtr obj,MessagePtr msg)
	{
		obj->PostMessage(msg);
		GASS::IComponentContainer::ComponentContainerIterator iter = obj->GetChildren();
		while(iter.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(iter.getNext());
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
			comp->SetSize(m_BrushSize*0.5);
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
			comp->SetInnerSize(m_BrushInnerSize*0.5);
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

	void TerrainDeformTool::SetLayerTexture(const std::string &texture, float tiling)
	{
		SceneObjectPtr selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		if(selected)
		{
			TerrainComponentPtr terrain = selected->GetFirstComponentByClass<ITerrainComponent>();
			if(terrain)
			{
				selected->PostMessage(MessagePtr(new TerrainLayerMessage(m_ActiveLayer,texture,tiling)));
			}
		}
	}


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
					m_BrushSize += m_BrushSize*0.3;
				else if(value < -0.5) 
					m_BrushSize -= m_BrushSize*0.3;
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
