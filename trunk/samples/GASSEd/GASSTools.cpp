/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "GASSTools.h"
#include "Modules/Editor/EditorSystem.h"
#include "Sim/GASS.h"
#include "Modules/Editor/EditorMessages.h"
#include "Modules/Editor/ToolSystem/MouseToolController.h"

#include <QMainWindow>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QSpinBox>
#include <QLabel>
#include <QToolTip>
#include <stdlib.h>

StandardToolBar::StandardToolBar(const QString &title, QWidget *parent)
    : QToolBar(parent)
{

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
    connect(m_SelectAct, SIGNAL(triggered()), this, SLOT(OnSelect()));
	addAction(m_SelectAct);

	m_MoveAct = new QAction(QIcon(":/images/move.png"), tr("&Move Tool"), this);
    m_MoveAct->setStatusTip(tr("Move Tool"));
    connect(m_MoveAct, SIGNAL(triggered()), this, SLOT(OnMove()));
	addAction(m_MoveAct);

	m_RotateAct = new QAction(QIcon(":/images/rotate.png"), tr("&Rotate Tool"), this);
    m_RotateAct->setStatusTip(tr("Rotate Tool"));
    connect(m_RotateAct, SIGNAL(triggered()), this, SLOT(OnRotate()));
	addAction(m_RotateAct);

	m_TerrainDeformAct = new QAction(QIcon(":/images/rotate.png"), tr("&Terrain Deform Tool"), this);
    m_TerrainDeformAct->setStatusTip(tr("Rotate Tool"));
    connect(m_TerrainDeformAct, SIGNAL(triggered()), this, SLOT(OnTerrainDeform()));
	addAction(m_TerrainDeformAct);
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
	GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetMouseToolController()->SelectTool(TID_TERRAIN_DEFORM);
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