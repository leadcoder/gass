#include "MeasurementTool.h"
#include <iomanip>
#include <memory>
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSException.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Interface/GASSILocationComponent.h"

namespace GASS
{
	MeasurementTool::MeasurementTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller)
	{

	}

	MeasurementTool::~MeasurementTool()
	{

	}

	void MeasurementTool::MouseMoved(const MouseData &/*data*/, const SceneCursorInfo &info)
	{
		if(m_MouseIsDown)
		{
			ScenePtr scene = m_Controller->GetEditorSceneManager()->GetScene();
			Vec3 offset = Vec3(0,0.1f,0);
			UpdateLine(m_StartPos+offset, info.m_3DPos+offset);
		}
	}


	void MeasurementTool::MouseDown(const MouseData &/*data*/, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;
		m_StartPos = info.m_3DPos;
	}

	void MeasurementTool::MouseUp(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
	{
		m_MouseIsDown = false;
		SceneObjectPtr ruler = GetOrCreateRulerObject();
		ruler->PostRequest(std::make_shared<ClearManualMeshRequest>());

		ComponentPtr text(m_TextComp);
		if(text)
		{
			std::string measurement_value = "";
			ruler->PostRequest(std::make_shared<TextCaptionRequest>(measurement_value));
		}
	}


	void MeasurementTool::UpdateLine(const Vec3 &start, const Vec3 &end)
	{
		const std::string mat_name = "MeasurementMaterial";
		GraphicsSystemPtr gfx_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IGraphicsSystem>();
		if (!gfx_sys->HasMaterial(mat_name))
		{
			GraphicsMaterial mat;
			mat.Name = mat_name;
			mat.Diffuse.Set(0, 0, 0, 1);
			mat.Ambient.Set(0, 0, 0);
			mat.SelfIllumination.Set(1.0, 0, 0);
			mat.DepthTest = false;
			mat.DepthWrite = true;
			mat.TrackVertexColor = false;
			gfx_sys->AddMaterial(mat, mat_name);
		}


		Vec3 text_pos = end;//(start + end)* 0.5; 
		SceneObjectPtr ruler = GetOrCreateRulerObject();

		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);
	
		sub_mesh_data->MaterialName = mat_name;
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

		
		ruler->PostRequest(std::make_shared<ManualMeshDataRequest>(mesh_data));

		ComponentPtr text(m_TextComp);
		if(text)
		{
			Float value = (start - end).Length();
			std::stringstream sstream;
			sstream << std::fixed << std::setprecision(2) << value << "m";
			std::string measurement_value = sstream.str();
			ruler->PostRequest(std::make_shared<TextCaptionRequest>(measurement_value));
			ruler->GetFirstComponentByClass<ILocationComponent>()->SetPosition(text_pos);
		}
	}

	SceneObjectPtr MeasurementTool::GetOrCreateRulerObject() 
	{
		GASS::SceneObjectPtr ruler = m_RulerObject.lock();

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