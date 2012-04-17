#include <iomanip>
#include "MeasurementTool.h"
#include "MouseToolController.h"
#include "../EditorManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Sim/Scenario/GASSScenario.h"
#include "Sim/Scenario/Scene/GASSSceneObject.h"
#include "Sim/Components/Graphics/GASSILocationComponent.h"
#include "Sim/Scenario/Scene/GASSSceneObjectManager.h"
#include "Sim/Components/Graphics/GASSMeshData.h"
#include "Sim/Scenario/Scene/Messages/GASSGraphicsSceneObjectMessages.h"


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
			ScenarioPtr scenario = m_Controller->GetScenario();
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

		SceneObjectPtr(m_RulerObject)->PostMessage(MessagePtr(new ClearManualMeshMessage()));


		ComponentPtr text(m_TextComp);
		if(text)
		{
			BaseReflectionObjectPtr props = boost::shared_dynamic_cast<BaseReflectionObject>(text);
			std::string measurement_value = "";
			//props->SetPropertyByType("Text",measurement_value);

			MessagePtr text_mess(new TextCaptionMessage(measurement_value));
			SceneObjectPtr(m_RulerObject)->PostMessage(text_mess);
		}
	}


	void MeasurementTool::UpdateLine(const Vec3 &start, const Vec3 &end)
	{

		Vec3 text_pos = start;//(start + end)* 0.5; 
		if(!SceneObjectPtr(m_RulerObject,boost::detail::sp_nothrow_tag()))
		{
			GASS::SceneObjectPtr scene_object = m_Controller->GetScenario()->GetObjectManager()->LoadFromTemplate("RulerObject");
			m_RulerObject = scene_object;
			if(scene_object)
			{
				m_TextComp = scene_object->GetComponent("TextComponent");
			}
		}

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
		SceneObjectPtr(m_RulerObject)->PostMessage(mesh_message);

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
			SceneObjectPtr(m_RulerObject)->PostMessage(text_mess);

			GASS::MessagePtr pos_msg(new PositionMessage(text_pos));
			SceneObjectPtr(m_RulerObject)->PostMessage(pos_msg);
		}
	}


}