#include "GASSTools.h"
#include "Modules/Editor/EditorSystem.h"
#include "Sim/GASS.h"
#include "Modules/Editor/EditorMessages.h"
#include "Modules/Editor/ToolSystem/MouseToolController.h"
#include "Modules/Editor/ToolSystem/TerrainDeformTool.h"
#include "Modules/Editor/ToolSystem/MouseToolController.h"

#include <QMainWindow>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QSpinBox>
#include <QLabel>
#include <QToolTip>
#include <QSlider>
#include <QComboBox>
#include <stdlib.h>

StandardToolBar::StandardToolBar(const QString &title, QWidget *parent)
    : QToolBar(parent),
	m_BrushFade(100),
	m_BrushSize(10)
{

	setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	m_NewAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    m_NewAct->setStatusTip(tr("New"));
    connect(m_NewAct, SIGNAL(triggered()), this, SLOT(OnNew()));
	addAction(m_NewAct);

	m_OpenAct = new QAction(QIcon(":/images/open.png"), tr("&Open"), this);
    m_OpenAct->setStatusTip(tr("Open"));
    connect(m_OpenAct, SIGNAL(triggered()), this, SLOT(OnOpen()));
	addAction(m_OpenAct);

	m_SaveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    m_OpenAct->setStatusTip(tr("Save"));
    connect(m_SaveAct, SIGNAL(triggered()), this, SLOT(OnSave()));
	addAction(m_SaveAct);

	m_SelectAct = new QAction(QIcon(":/images/select.png"), tr("&Select Tool"), this);
    m_SelectAct->setStatusTip(tr("Select"));
	m_SelectAct->setCheckable(true);
    connect(m_SelectAct, SIGNAL(triggered()), this, SLOT(OnSelect()));
	addAction(m_SelectAct);

	m_MoveAct = new QAction(QIcon(":/images/move.png"), tr("&Move Tool"), this);
    m_MoveAct->setStatusTip(tr("Move Tool"));
	m_MoveAct->setCheckable(true);
    connect(m_MoveAct, SIGNAL(triggered()), this, SLOT(OnMove()));
	addAction(m_MoveAct);

	m_RotateAct = new QAction(QIcon(":/images/rotate.png"), tr("&Rotate Tool"), this);
    m_RotateAct->setStatusTip(tr("Rotate Tool"));
	m_RotateAct->setCheckable(true);
    connect(m_RotateAct, SIGNAL(triggered()), this, SLOT(OnRotate()));
	addAction(m_RotateAct);

	m_TerrainDeformAct = new QAction(QIcon(":/images/rotate.png"), tr("&Deform"), this);
    m_TerrainDeformAct->setStatusTip(tr("Terrain Deform Tool"));
	m_TerrainDeformAct->setCheckable(true);
    connect(m_TerrainDeformAct, SIGNAL(triggered()), this, SLOT(OnTerrainDeform()));
	addAction(m_TerrainDeformAct);

	m_TerrainSmoothAct = new QAction(QIcon(":/images/rotate.png"), tr("&Smooth"), this);
    m_TerrainSmoothAct->setStatusTip(tr("Terrain Smooth Tool"));
	m_TerrainSmoothAct->setCheckable(true);
    connect(m_TerrainSmoothAct, SIGNAL(triggered()), this, SLOT(OnTerrainSmooth()));
	addAction(m_TerrainSmoothAct);

	m_TerrainFlattenAct = new QAction(QIcon(":/images/rotate.png"), tr("&Flatten"), this);
    m_TerrainFlattenAct->setStatusTip(tr("Terrain Flatten Tool"));
	m_TerrainFlattenAct->setCheckable(true);
    connect(m_TerrainFlattenAct, SIGNAL(triggered()), this, SLOT(OnTerrainFlatten()));
	addAction(m_TerrainFlattenAct);
	
	m_TerrainPaintAct = new QAction(QIcon(":/images/rotate.png"), tr("&Paint"), this);
    m_TerrainPaintAct->setStatusTip(tr("Terrain Paint Tool"));
	m_TerrainPaintAct->setCheckable(true);
    connect(m_TerrainPaintAct, SIGNAL(triggered()), this, SLOT(OnTerrainPaint()));
	addAction(m_TerrainPaintAct);

	QComboBox* como_box = new QComboBox();
	como_box->addItem("Paint Layer1");
	como_box->addItem("Paint Layer2");
	como_box->addItem("Paint Layer3");
	como_box->addItem("Paint Layer4");
	connect(como_box, SIGNAL(currentIndexChanged(int)), this, SLOT(OnPaintLayerChanged(int)));
	addWidget(como_box);


	m_VegetationPaintAct = new QAction(QIcon(":/images/rotate.png"), tr("&Veg.Paint"), this);
    m_VegetationPaintAct->setStatusTip(tr("Vegetation Paint Tool"));
	m_VegetationPaintAct->setCheckable(true);
    connect(m_VegetationPaintAct, SIGNAL(triggered()), this, SLOT(OnVegetationPaint()));
	addAction(m_VegetationPaintAct);


	GASS::SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(StandardToolBar::OnToolChanged,GASS::ToolChangedMessage,0));	

	QLabel*  size_label = new QLabel(tr("Size:"));
	addWidget(size_label);
	QSlider* m_BrushSizeSlider = new QSlider(Qt::Horizontal,this);
	m_BrushSizeSlider->setMinimum(0);
    m_BrushSizeSlider->setMaximum(1000);
    m_BrushSizeSlider->setValue(500);
	m_BrushSizeSlider->setMinimumWidth(30);
	addWidget(m_BrushSizeSlider);
	connect(m_BrushSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(OnBrushSizeChanged(int)));

	QLabel*  fade_label = new QLabel(tr("Fade:"));
	addWidget(fade_label);
	QSlider* m_BrushFadeSlider = new QSlider(Qt::Horizontal,this);
	m_BrushFadeSlider->setMinimum(0);
    m_BrushFadeSlider->setMaximum(100);
    m_BrushFadeSlider->setValue(m_BrushFade);
	m_BrushFadeSlider->setMinimumWidth(30);
	addWidget(m_BrushFadeSlider);
	connect(m_BrushFadeSlider, SIGNAL(valueChanged(int)), this, SLOT(OnBrushFadeChanged(int)));

	QLabel*  int_label = new QLabel(tr("Intensity:"));
	addWidget(int_label);
	
	QSlider* m_BrushIntSlider = new QSlider(Qt::Horizontal,this);
	m_BrushIntSlider->setMinimum(0);
    m_BrushIntSlider->setMaximum(1000);
    m_BrushIntSlider->setValue(500);
	m_BrushIntSlider->setMinimumWidth(30);
	addWidget(m_BrushIntSlider);
	connect(m_BrushIntSlider, SIGNAL(valueChanged(int)), this, SLOT(OnBrushIntChanged(int)));

	
	QLabel*  noise_label = new QLabel(tr("Noise:"));
	addWidget(noise_label);
	

	QSlider* m_BrushNoiseSlider = new QSlider(Qt::Horizontal,this);
	m_BrushNoiseSlider->setMinimum(0);
    m_BrushNoiseSlider->setMaximum(1000);
    m_BrushNoiseSlider->setValue(0);
	m_BrushNoiseSlider->setMinimumWidth(30);
	addWidget(m_BrushNoiseSlider);
	connect(m_BrushNoiseSlider, SIGNAL(valueChanged(int)), this, SLOT(OnBrushNoiseChanged(int)));
}

void StandardToolBar::OnBrushSizeChanged(int value)
{
	GASS::TerrainDeformTool* tool = static_cast<GASS::TerrainDeformTool*> (GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->GetTool(TID_TERRAIN));
    m_BrushSize = value/10.0;
	tool->SetBrushSize(m_BrushSize);
	tool->SetBrushInnerSize(m_BrushSize*m_BrushFade/100.0);
}

void StandardToolBar::OnBrushFadeChanged(int value)
{
	GASS::TerrainDeformTool* tool = static_cast<GASS::TerrainDeformTool*> (GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->GetTool(TID_TERRAIN));
    m_BrushFade = value;
	tool->SetBrushInnerSize(m_BrushSize*m_BrushFade/100.0);
}

void StandardToolBar::OnBrushIntChanged(int value)
{
	GASS::TerrainDeformTool* tool = static_cast<GASS::TerrainDeformTool*> (GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->GetTool(TID_TERRAIN));
    float intensity = value/100.0;
	tool->SetIntensity(intensity);
}

void StandardToolBar::OnBrushNoiseChanged(int value)
{
	GASS::TerrainDeformTool* tool = static_cast<GASS::TerrainDeformTool*> (GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->GetTool(TID_TERRAIN));
    float intensity = value/100.0;
	tool->SetNoise(intensity);
}

void StandardToolBar::OnPaintLayerChanged(int value)
{
	GASS::TerrainDeformTool* tool = static_cast<GASS::TerrainDeformTool*> (GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->GetTool(TID_TERRAIN));
    
	switch(value)
	{
	case 0:
		tool->SetActiveLayer(GASS::TL_1);
		break;
	case 1:
		tool->SetActiveLayer(GASS::TL_2);
		break;
	case 2:
		tool->SetActiveLayer(GASS::TL_3);
		break;
	case 3:
		tool->SetActiveLayer(GASS::TL_4);
		break;
	}
}


void StandardToolBar::OnToolChanged(GASS::ToolChangedMessagePtr message)
{
	//reset all
	m_SelectAct->setChecked(false);
    m_MoveAct->setChecked(false);
    m_RotateAct->setChecked(false);
    m_TerrainDeformAct->setChecked(false);
	m_TerrainSmoothAct->setChecked(false);
	m_TerrainFlattenAct->setChecked(false);
	m_TerrainPaintAct->setChecked(false);
	m_VegetationPaintAct->setChecked(false);

	if(message->GetTool() == TID_SELECT)
		m_SelectAct->setChecked(true);
	if(message->GetTool() == TID_MOVE)
		m_MoveAct->setChecked(true);
	if(message->GetTool() == TID_ROTATE)
		m_RotateAct->setChecked(true);
	if(message->GetTool() == TID_TERRAIN)
	{
		GASS::TerrainDeformTool* tool = static_cast<GASS::TerrainDeformTool*> (GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->GetTool(TID_TERRAIN));
		switch(tool->GetModMode())
		{
			case GASS::TerrainDeformTool::TEM_DEFORM:
				m_TerrainDeformAct->setChecked(true);
				break;
			case GASS::TerrainDeformTool::TEM_SMOOTH:
				m_TerrainSmoothAct->setChecked(true);
				break;
			case GASS::TerrainDeformTool::TEM_FLATTEN:
				m_TerrainFlattenAct->setChecked(true);
				break;
			case GASS::TerrainDeformTool::TEM_LAYER_PAINT:
				m_TerrainPaintAct->setChecked(true);
				break;
			case GASS::TerrainDeformTool::TEM_VEGETATION_PAINT:
				m_VegetationPaintAct->setChecked(true);
				break;
		}
	}
}


void StandardToolBar::OnSelect()
{
	GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->SelectTool(TID_SELECT);
}

void StandardToolBar::OnMove()
{
	GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->SelectTool(TID_MOVE);
}

void StandardToolBar::OnRotate()
{
	GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->SelectTool(TID_ROTATE);
}

void StandardToolBar::OnTerrainDeform()
{
	GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->SelectTool(TID_TERRAIN);
	GASS::TerrainDeformTool* tool = static_cast<GASS::TerrainDeformTool*> (GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->GetTool(TID_TERRAIN));
	tool->SetModMode(GASS::TerrainDeformTool::TEM_DEFORM);
}

void StandardToolBar::OnTerrainSmooth()
{
	GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->SelectTool(TID_TERRAIN);
	GASS::TerrainDeformTool* tool = static_cast<GASS::TerrainDeformTool*> (GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->GetTool(TID_TERRAIN));
	tool->SetModMode(GASS::TerrainDeformTool::TEM_SMOOTH);
}

void StandardToolBar::OnTerrainFlatten()
{
	GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->SelectTool(TID_TERRAIN);
	GASS::TerrainDeformTool* tool = static_cast<GASS::TerrainDeformTool*> (GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->GetTool(TID_TERRAIN));
	tool->SetModMode(GASS::TerrainDeformTool::TEM_FLATTEN);
}

void StandardToolBar::OnTerrainPaint()
{
	GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->SelectTool(TID_TERRAIN);
	GASS::TerrainDeformTool* tool = static_cast<GASS::TerrainDeformTool*> (GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->GetTool(TID_TERRAIN));
	tool->SetModMode(GASS::TerrainDeformTool::TEM_LAYER_PAINT);
}

void StandardToolBar::OnVegetationPaint()
{
	GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->SelectTool(TID_TERRAIN);
	GASS::TerrainDeformTool* tool = static_cast<GASS::TerrainDeformTool*> (GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->GetTool(TID_TERRAIN));
	tool->SetModMode(GASS::TerrainDeformTool::TEM_VEGETATION_PAINT);
}

void StandardToolBar::OnNew()
{

}

void StandardToolBar::OnSave()
{

}

void StandardToolBar::OnOpen()
{

}