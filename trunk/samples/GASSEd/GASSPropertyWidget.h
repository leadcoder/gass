#ifndef __GASSPropertyWidget_H__
#define __GASSPropertyWidget_H__

#pragma warning (disable : 4100)
#include "Modules/Editor/EditorMessages.h"
#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"
#include <QtGui>

#include "Sim/GASS.h"
namespace GASS
{
	class PropertySettings;
}

class GASSVariantProperty
{
public:
	GASSVariantProperty() {};
	virtual ~GASSVariantProperty() {};
	void SetGASSData(GASS::BaseReflectionObjectPtr obj,GASS::IProperty* prop) {m_Object= obj; m_Prop = prop; }
	void UpdateValue(const std::string &value) 
	{
		GASS::BaseReflectionObjectPtr object(m_Object,boost::detail::sp_nothrow_tag());
		if(object)
		{
			m_Prop->SetValueByString(object.get(),value);
		}
	}

	GASS::BaseReflectionObjectWeakPtr m_Object;
	GASS::IProperty* m_Prop;
	std::vector<std::string > m_Options;
};

class GASSPropertyWidget : public QtTreePropertyBrowser, public GASS::StaticMessageListener
{
	Q_OBJECT
public:
	GASSPropertyWidget( QWidget *parent=0 );
	virtual ~GASSPropertyWidget();
	void OnLoadScene(GASS::SceneAboutToLoadNotifyMessagePtr message);
	void OnUnloadScene(GASS::SceneUnloadNotifyMessagePtr message);
	void OnSceneObjectSelected(GASS::ObjectSelectionChangedMessagePtr message);
protected:
	
	QtVariantProperty * CreateProp(GASS::BaseReflectionObjectPtr obj, GASS::IProperty* prop,const GASS::PropertySettings *ps);
	void Show(GASS::SceneObjectPtr object);
	GASS::SceneWeakPtr m_Scene;
	QtProperty *m_Root;
	QtVariantPropertyManager *m_VariantManager;
	QtVariantEditorFactory *m_VariantFactory;

	bool m_Polulating;
	std::map<QtProperty*,GASSVariantProperty> m_PropMap;

private slots:
	void valueChanged(QtProperty *property, const QVariant &value);
};

#endif