#include "GASSBrushSettingsWidget.h"
#include "GASSEd.h"
#include "Modules/Editor/EditorSystem.h"
#include "Sim/GASS.h"
#include "Modules/Editor/EditorMessages.h"
#include "Modules/Editor/EditorSceneManager.h"
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

BrushSettingsWidget::BrushSettingsWidget(const QString &title, GASSEd *parent)
    : QToolBar(parent),
	m_GASSEd(parent),
	m_BrushFade(100),
	m_BrushSize(10)
{
	setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

	QLabel*  main_label = new QLabel(tr("Brush Settings"));
	addWidget(main_label);
	addSeparator();

	QLabel*  size_label = new QLabel(tr("Size:"));
	addWidget(size_label);
	QSlider* m_BrushSizeSlider = new QSlider(Qt::Horizontal,this);
	m_BrushSizeSlider->setMinimum(0);
    m_BrushSizeSlider->setMaximum(1000);
    m_BrushSizeSlider->setValue(500);
	m_BrushSizeSlider->setMinimumWidth(30);
	addWidget(m_BrushSizeSlider);
	connect(m_BrushSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(OnBrushSizeChanged(int)));
	addSeparator();

	QLabel*  fade_label = new QLabel(tr("Fade:"));
	addWidget(fade_label);
	QSlider* m_BrushFadeSlider = new QSlider(Qt::Horizontal,this);
	m_BrushFadeSlider->setMinimum(0);
    m_BrushFadeSlider->setMaximum(100);
    m_BrushFadeSlider->setValue(m_BrushFade);
	m_BrushFadeSlider->setMinimumWidth(30);
	addWidget(m_BrushFadeSlider);
	connect(m_BrushFadeSlider, SIGNAL(valueChanged(int)), this, SLOT(OnBrushFadeChanged(int)));
	addSeparator();

	QLabel*  int_label = new QLabel(tr("Intensity:"));
	addWidget(int_label);
	
	QSlider* m_BrushIntSlider = new QSlider(Qt::Horizontal,this);
	m_BrushIntSlider->setMinimum(0);
    m_BrushIntSlider->setMaximum(1000);
    m_BrushIntSlider->setValue(500);
	m_BrushIntSlider->setMinimumWidth(30);
	addWidget(m_BrushIntSlider);
	connect(m_BrushIntSlider, SIGNAL(valueChanged(int)), this, SLOT(OnBrushIntChanged(int)));
	addSeparator();

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

void BrushSettingsWidget::OnBrushSizeChanged(int value)
{
	if(m_GASSEd->GetScene())
	{
		GASS::TerrainDeformTool* tool = static_cast<GASS::TerrainDeformTool*> (m_GASSEd->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>()->GetMouseToolController()->GetTool(TID_TERRAIN));
		m_BrushSize = value/10.0;
		tool->SetBrushSize(m_BrushSize);
		tool->SetBrushInnerSize(m_BrushSize*m_BrushFade/100.0);
	}
}

void BrushSettingsWidget::OnBrushFadeChanged(int value)
{
	if(m_GASSEd->GetScene())
	{
		GASS::TerrainDeformTool* tool = static_cast<GASS::TerrainDeformTool*> (m_GASSEd->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>()->GetMouseToolController()->GetTool(TID_TERRAIN));
		m_BrushFade = value;
		tool->SetBrushInnerSize(m_BrushSize*m_BrushFade/100.0);
	}
}

void BrushSettingsWidget::OnBrushIntChanged(int value)
{
	if(m_GASSEd->GetScene())
	{
		GASS::TerrainDeformTool* tool = static_cast<GASS::TerrainDeformTool*> (m_GASSEd->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>()->GetMouseToolController()->GetTool(TID_TERRAIN));
		float intensity = value/100.0;
		tool->SetIntensity(intensity);
	}
}

void BrushSettingsWidget::OnBrushNoiseChanged(int value)
{
	if(m_GASSEd->GetScene())
	{
		GASS::TerrainDeformTool* tool = static_cast<GASS::TerrainDeformTool*> (m_GASSEd->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>()->GetMouseToolController()->GetTool(TID_TERRAIN));
		float intensity = value/100.0;
		tool->SetNoise(intensity);
	}
}

