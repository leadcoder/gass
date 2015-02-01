#include "GASSRenderWidget.h"
#include "GASSEd.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"

#include "Modules/Editor/EditorApplication.h"
#include "Modules/Editor/ToolSystem/MouseToolController.h"

#include "Sim/GASS.h" 

GASSRenderWidget::GASSRenderWidget( GASSEd *parent): QWidget(parent),
	m_GASSEd(parent)
{
	setAttribute(Qt::WA_PaintOnScreen);
	setFocusPolicy(Qt::ClickFocus);
	setMouseTracking(true);
	setAcceptDrops(true);
}

GASSRenderWidget::~GASSRenderWidget()
{

}

void GASSRenderWidget::dragEnterEvent (QDragEnterEvent* event )
{
    //qDebug() << "dragEnterEvent";
    //if (event->mimeData()->hasFormat("foo/bar"))
        event->acceptProposedAction();
}

void GASSRenderWidget::dragMoveEvent(QDragMoveEvent *event)
{
    /*if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }*/
}

void GASSRenderWidget::dropEvent(QDropEvent *event)
{
	if(event->mimeData() && event->mimeData()->hasText())
	{
		const std::string template_name =  event->mimeData()->text().toStdString();
		QPoint pos = event->pos();
		GASS::Vec2 norm_pos((float) pos.x() / (float) m_Size.width(),
			(float) pos.y() / (float) m_Size.height());
		if(m_GASSEd->GetScene())
			m_GASSEd->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>()->GetMouseToolController()->CreateSceneObject(template_name,norm_pos);
	}
}

QPaintEngine* GASSRenderWidget::paintEngine() const
{
	// We don't want another paint engine to get in the way for our Ogre based paint engine.
	// So we return nothing.
	return NULL;
}

void GASSRenderWidget::paintEvent(QPaintEvent *e)
{
	if(m_GASSEd->IsInitialized())
	{
		m_GASSEd->m_GASSApp->Update(); 
	}
	update();
	e->accept();
}

void GASSRenderWidget::showEvent(QShowEvent *e)
{
	QWidget::showEvent(e);
	/*if(!m_Initialized)
	{
		//m_GASSEd->Initialize(this->winId());
		m_Initialized = true;
	}*/
}

void GASSRenderWidget::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);
	if(e->isAccepted())
	{
		const QSize &newSize = e->size();
		m_Size = newSize;
		onSizeUpdated();
	}
}

void GASSRenderWidget::onSizeUpdated()
{
	if(m_GASSEd->IsInitialized())
	{
		GASS::SystemMessagePtr resize_message(new GASS::ViewportMovedOrResizedEvent("RenderWindow",0,0,m_Size.width(), m_Size.height()));
		GASS::SimSystemManagerPtr ssm = GASS::SimEngine::Get().GetSimSystemManager();
		ssm->SendImmediate(resize_message);
	}
}

void GASSRenderWidget::sendDelayedResize()
{
	//updateGeometry();
	//this->resize(this->geometry().width(), this->geometry().height());
	/*if(m_GASSEd->IsInitialized())
	{
		GASS::SystemMessagePtr resize_message(new GASS::ViewportMovedOrResizedEvent("RenderWindow",0,0,m_Size.width(), m_Size.height(),-1,4));
		GASS::SimSystemManagerPtr ssm = GASS::SimEngine::Get().GetSimSystemManager();
		ssm->PostMessage(resize_message);
	}*/
}


void GASSRenderWidget::mouseReleaseEvent(QMouseEvent *e)
{
	if(m_GASSEd->IsInitialized())
	{
		GASS::ProxyInputSystemPtr is = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::IProxyInputSystem>();
		if(is)
			is->InjectMouseReleased(GetMouseData(e),GetMouseButton(e));
	}
	QWidget::mouseReleaseEvent(e);
}

void GASSRenderWidget::mousePressEvent(QMouseEvent *e)
{
	if(m_GASSEd->IsInitialized())
	{
		GASS::ProxyInputSystemPtr is = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::IProxyInputSystem>();
		if(is)
		{
			is->InjectMousePressed(GetMouseData(e),GetMouseButton(e));
		}
	}
	QWidget::mousePressEvent(e);
}

void GASSRenderWidget::mouseMoveEvent(QMouseEvent *e)
{
	if(m_GASSEd->IsInitialized())
	{
		GASS::ProxyInputSystemPtr is = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::IProxyInputSystem>();
		if(is)
		{
			GASS::MouseData  md = GetMouseData(e);
			is->InjectMouseMoved(md);
			m_LastData = md;
		}
	}
	QWidget::mouseMoveEvent(e);
}


static float MouseSpeed = 20;
float NormalizeMouseDelta(float value)
{
		float ret = 0;
	if (value > MouseSpeed)
			ret = 1;
		else if (value < -MouseSpeed)
			ret = -1;
		else
			ret = value / MouseSpeed;
		return ret;
}

GASS::MouseData GASSRenderWidget::GetMouseData(QMouseEvent *e) const 
{
	GASS::MouseData md;
	md.XAbs = e->x();
	md.YAbs = e->y();
	md.XRel = NormalizeMouseDelta(float (e->x()-m_LastData.XAbs));///10.0;
	md.YRel = NormalizeMouseDelta(float (e->y()-m_LastData.YAbs));///10.0;
	md.XAbsNorm = (float) e->x() / (float) m_Size.width();
	md.YAbsNorm = (float) e->y() / (float) m_Size.height();
	return md;	
}

GASS::MouseButtonId GASSRenderWidget::GetMouseButton(QMouseEvent *e) const
{
	GASS::MouseButtonId mb;
	
	switch(e->button())
	{
	case Qt::LeftButton:
		mb = GASS::MBID_LEFT;
		break;
	case Qt::RightButton:
		mb = GASS::MBID_RIGHT;
		break;
	}
	return mb;
}


