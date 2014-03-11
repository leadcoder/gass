#include <iomanip>
#include "MeasurementTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/Utils/GASSException.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"


namespace GASS
{
	MeasurementTool::MeasurementTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller)
	{

	}

	MeasurementTool::~MeasurementTool()
	{

	}

	void MeasurementTool::MouseMoved(const MouseData &data, const SceneCursorInfo &info)
	{
		//SceneObjectPtr selected(m_SelectedObject,NO_THROW);
		if(m_MouseIsDown)
		{
			ScenePtr scene = m_Controller->GetEditorSceneManager()->GetScene();
			Vec3 offset = Vec3(0,0.1f,0);
			UpdateLine(m_StartPos+offset, info.m_3DPos+offset);
		}
	}


	void MeasurementTool::MouseDown(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;
		m_StartPos = info.m_3DPos;
	}

	void MeasurementTool::MouseUp(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = false;
		SceneObjectPtr ruler = GetOrCreateRulerObject();
		ruler->PostRequest(ClearManualMeshMessagePtr(new ClearManualMeshMessage()));

		ComponentPtr text(m_TextComp);
		if(text)
		{
			BaseReflectionObjectPtr props = DYNAMIC_PTR_CAST<BaseReflectionObject>(text);
			std::string measurement_value = "";
			ruler->PostRequest(TextCaptionMessagePtr(new TextCaptionMessage(measurement_value)));
		}
	}


	void MeasurementTool::UpdateLine(const Vec3 &start, const Vec3 &end)
	{

		Vec3 text_pos = end;//(start + end)* 0.5; 
		SceneObjectPtr ruler = GetOrCreateRulerObject();

		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);
	
		sub_mesh_data->MaterialName = "WhiteTransparentNoLighting";
		sub_mesh_data->Type = LINE_STRIP;
		ColorRGBA color(1,0,0,1);
		
		Vec3 pos = start - text_pos;
		sub_mesh_data->PositionVector.push_back(pos);
		sub_mesh_data->ColorVector.push_back(color);
		pos = end - text_pos;
		sub_mesh_data->PositionVector.push_back(pos);
		sub_mesh_data->ColorVector.push_back(color);
		
		sub_mesh_data->IndexVector.push_back(0);
		sub_mesh_data->IndexVector.push_back(1);

		
		ruler->PostRequest(ManualMeshDataMessagePtr(new ManualMeshDataMessage(mesh_data)));

		ComponentPtr text(m_TextComp);
		if(text)
		{
			float value = (start-end).Length();
			std::stringstream sstream;
			sstream << std::fixed << std::setprecision(2) << value << "m";
			std::string measurement_value = sstream.str();
			
			ruler->PostRequest(TextCaptionMessagePtr(new TextCaptionMessage(measurement_value)));
			ruler->PostRequest(PositionMessagePtr(new PositionMessage(text_pos)));
		}
	}

	SceneObjectPtr MeasurementTool::GetOrCreateRulerObject() 
	{
		GASS::SceneObjectPtr ruler(m_RulerObject,NO_THROW);

		if(!ruler)
		{
			ruler = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate("RulerObject",m_Controller->GetEditorSceneManager()->GetScene()->GetRootSceneObject());
			if(!ruler)
			{
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to create in RulerObject","MeasurementTool::GetOrCreateRulerObject");
			}
			m_RulerObject = ruler;
			if(ruler)
			{
				m_TextComp = ruler->GetComponent("TextComponent");
			}
		}
		return ruler;
	}

}