#include <iomanip>
#include "MeasurementTool.h"
#include "MouseToolController.h"
#include "../EditorManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/Utils/GASSException.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Components/Graphics/GASSILocationComponent.h"
#include "Sim/Components/Graphics/GASSMeshData.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"


namespace GASS
{
	MeasurementTool::MeasurementTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller)
	{

	}

	MeasurementTool::~MeasurementTool()
	{

	}

	void MeasurementTool::MoveTo(const CursorInfo &info)
	{
		//SceneObjectPtr selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		if(m_MouseIsDown)
		{
			ScenePtr scene = EditorManager::Get().GetScene();
			Vec3 offset = Vec3(0,0.1f,0);
			UpdateLine(m_StartPos+offset, info.m_3DPos+offset);
		}
	}


	void MeasurementTool::MouseDown(const CursorInfo &info)
	{
		m_MouseIsDown = true;
		m_StartPos = info.m_3DPos;
	}

	void MeasurementTool::MouseUp(const CursorInfo &info)
	{
		m_MouseIsDown = false;
		SceneObjectPtr ruler = GetOrCreateRulerObject();
		ruler->PostMessage(MessagePtr(new ClearManualMeshMessage()));

		ComponentPtr text(m_TextComp);
		if(text)
		{
			BaseReflectionObjectPtr props = boost::shared_dynamic_cast<BaseReflectionObject>(text);
			std::string measurement_value = "";
			MessagePtr text_mess(new TextCaptionMessage(measurement_value));
			ruler->PostMessage(text_mess);
		}
	}


	void MeasurementTool::UpdateLine(const Vec3 &start, const Vec3 &end)
	{

		Vec3 text_pos = end;//(start + end)* 0.5; 
		SceneObjectPtr ruler = GetOrCreateRulerObject();

		ManualMeshDataPtr mesh_data(new ManualMeshData());
		MeshVertex vertex;
		mesh_data->Material = "WhiteTransparentNoLighting";
		mesh_data->Type = LINE_STRIP;
		vertex.TexCoord.Set(0,0);
		vertex.Color = Vec4(1,0,0,1);
		vertex.Pos = start - text_pos;
		mesh_data->VertexVector.push_back(vertex);
		vertex.Pos = end - text_pos;
		mesh_data->VertexVector.push_back(vertex);
		mesh_data->IndexVector.push_back(0);
		mesh_data->IndexVector.push_back(1);

		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		ruler->PostMessage(mesh_message);

		ComponentPtr text(m_TextComp);
		if(text)
		{
			float value = (start-end).Length();
			std::stringstream sstream;
			sstream << std::fixed << std::setprecision(2) << value << "m";
			std::string measurement_value = sstream.str();

			MessagePtr text_mess(new TextCaptionMessage(measurement_value));
			//text_mess->SetData("Parameter",SceneObject::CAPTION);
			//text_mess->SetData("Caption",measurement_value);
			ruler->PostMessage(text_mess);

			GASS::MessagePtr pos_msg(new PositionMessage(text_pos));
			ruler->PostMessage(pos_msg);
		}
	}

	SceneObjectPtr MeasurementTool::GetOrCreateRulerObject() 
	{
		GASS::SceneObjectPtr ruler(m_RulerObject,boost::detail::sp_nothrow_tag());

		if(!ruler)
		{
			ruler = EditorManager::Get().GetScene()->LoadObjectFromTemplate("RulerObject",EditorManager::Get().GetScene()->GetRootSceneObject());
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