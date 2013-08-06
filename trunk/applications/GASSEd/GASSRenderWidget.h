#ifndef __GASSRenderWidget_H__
#define __GASSRenderWidget_H__
#pragma warning (disable : 4100)
#include <Sim/GASS.h>
#include <QtGui>
class GASSEd;
class GASSRenderWidget : public QWidget
{
	Q_OBJECT;
public:
	GASSRenderWidget( GASSEd *parent=0 );
	virtual ~GASSRenderWidget();
	virtual QPaintEngine* paintEngine() const;
	virtual void paintEvent(QPaintEvent *e);
	virtual void resizeEvent(QResizeEvent *e);
	virtual void mouseReleaseEvent ( QMouseEvent * e );
	virtual void mousePressEvent ( QMouseEvent * e );
	virtual void mouseMoveEvent ( QMouseEvent * e );
	virtual void dropEvent(QDropEvent *event);
	virtual void dragEnterEvent (QDragEnterEvent* event );
	virtual void dragMoveEvent (QDragMoveEvent* event );
	void showEvent(QShowEvent *e);
	void initialize();

	GASSEd* m_GASSEd;
protected:
	GASS::MouseData GetMouseData(QMouseEvent *e) const;
	GASS::MouseButtonId GetMouseButton(QMouseEvent *e) const;
	QSize m_Size;
	GASS::MouseData m_LastData;
	bool m_Initialized;


};

#endif