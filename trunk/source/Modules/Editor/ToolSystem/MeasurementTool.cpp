#include <iomanip>
#include "MeasurementTool.h"
#include "MouseToolController.h"
#include "../EditorManager.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/Components/Graphics/Geometry/ILineComponent.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"


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
			ScenarioScenePtr scene = m_Controller->GetScene();
			Vec3 offset = scene->GetSceneUp()*0.1f;
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

		LineComponentPtr line(m_RulerLine);
		if(line)
		{
			line->Clear();
		}

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

		if(!LineComponentPtr(m_RulerLine,boost::detail::sp_nothrow_tag()))
		{
			GASS::SceneObjectPtr scene_object = m_Controller->GetScene()->GetObjectManager()->LoadFromTemplate("RulerObject");
			m_RulerObject = scene_object;
			
			//m_RulerObject = scene_object->GetFirstChildByName("RulerObject",false);
			if(scene_object)
			{
				m_RulerLine = scene_object->GetFirstComponentByClass<ILineComponent>();
				m_TextComp = scene_object->GetComponent("TextComponent");
			}
		}

		LineComponentPtr line(m_RulerLine);
		if(line)
		{
			line->Clear();
			LineData start_data;
			LineData end_data;
			start_data.pos = start-text_pos;  
			start_data.color = Vec3(1,0,0);
			end_data.pos = end-text_pos; 
			end_data.color = Vec3(1,0,0);
			std::vector<LineData> line_data;
			line_data.push_back(start_data);
			line_data.push_back(end_data);
			line->Add(line_data);


		}

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