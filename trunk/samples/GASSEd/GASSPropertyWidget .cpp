#include "GASSPropertyWidget.h"
#include "GASSEd.h"
#include "Modules/Editor/EditorApplication.h"
#include "Modules/Editor/EditorSystem.h"


GASSPropertyWidget::GASSPropertyWidget( QWidget *parent): QtTreePropertyBrowser(parent), m_Root(NULL)
{
	GASS::SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GASSPropertyWidget::OnLoadScene,GASS::SceneAboutToLoadNotifyMessage,0));
	GASS::SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GASSPropertyWidget::OnUnloadScene,GASS::SceneUnloadNotifyMessage,0));
	GASS::SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GASSPropertyWidget::OnSceneObjectSelected,GASS::ObjectSelectionChangedMessage,0));
	
	m_VariantManager = new QtVariantPropertyManager();
	m_VariantFactory = new QtVariantEditorFactory();
    setFactoryForManager(m_VariantManager, m_VariantFactory);
    setPropertiesWithoutValueMarked(true);
    setRootIsDecorated(false);
	setMinimumSize(200,200);
}

GASSPropertyWidget::~GASSPropertyWidget()
{

}

void GASSPropertyWidget::OnLoadScene(GASS::SceneAboutToLoadNotifyMessagePtr message)
{
	GASS::ScenePtr scene = message->GetScene();
	m_Scene = scene;
	
}

void GASSPropertyWidget::OnUnloadScene(GASS::SceneUnloadNotifyMessagePtr message)
{

}


void GASSPropertyWidget::OnSceneObjectSelected(GASS::ObjectSelectionChangedMessagePtr message)
{
	Show(message->GetSceneObject());
}

void GASSPropertyWidget::Show(GASS::SceneObjectPtr object)
{
	//iterate object attributes and components
	if(m_Root)
	{
		delete m_Root;
		
    //QtVariantProperty *item = variantManager->addProperty(QVariant::Bool, QString::number(i++) + QLatin1String(" Bool Property"));
    //item->setValue(true);
    //topItem->addSubProperty(item);
	}

	if(!object)
		return;
	std::string class_name  = object->GetRTTI()->GetClassName();
	const GASS::ObjectSettings* os = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetGUISettings()->GetObjectSettings(class_name);
	if(os && os->Visible) //we have settings
	{
		//m_Root = new CMFCPropertyGridProperty(_T(object->GetName().c_str()));
		m_Root = m_VariantManager->addProperty(QtVariantPropertyManager::groupTypeId(),QLatin1String(object->GetName().c_str()));
		addProperty(m_Root);

		GASS::PropertyVector props = object->GetProperties();
		for(size_t i = 0;  i < props.size(); i++)
		{

			const GASS::PropertySettings* ps = os->GetProperty(props[i]->GetName());
			QtVariantProperty* grid_prop = CreateProp(object,props[i],ps);
			if(grid_prop)
				m_Root->addSubProperty(grid_prop);
		}

		GASS::IComponentContainer::ComponentIterator comp_iter = object->GetComponents();
		while(comp_iter.hasMoreElements())
		{
			GASS::BaseComponentPtr comp = boost::shared_static_cast<GASS::BaseComponent>(comp_iter.getNext());
			std::string class_name = comp->GetRTTI()->GetClassName();
			const GASS::ObjectSettings* os = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->GetGUISettings()->GetObjectSettings(class_name);
			if(os && os->Visible) //we have settings!
			{
				QtVariantProperty* comp_root  = m_VariantManager->addProperty(QtVariantPropertyManager::groupTypeId(),QLatin1String(class_name.c_str()));
		
				//CMFCPropertyGridProperty* comp_root = new CMFCPropertyGridProperty(_T(class_name.c_str()));
				m_Root->addSubProperty(comp_root);

				GASS::PropertyVector props = comp->GetProperties();
				for(size_t i = 0;  i < props.size(); i++)
				{
					const GASS::PropertySettings* ps = os->GetProperty(props[i]->GetName());
					QtVariantProperty* grid_prop = CreateProp(comp,props[i],ps);
					if(grid_prop)
						comp_root->addSubProperty(grid_prop);
				}
			}
		}
	}
}


QtVariantProperty *GASSPropertyWidget::CreateProp(GASS::BaseReflectionObjectPtr obj, GASS::IProperty* prop,const GASS::PropertySettings *ps)
{
	if(!ps)
		return NULL;
	if(!ps->Visible)
		return NULL;
	const std::string prop_name = prop->GetName();
	const std::string prop_value = prop->GetValueAsString(obj.get());
	QtVariantProperty* item = NULL;

	item = m_VariantManager->addProperty(QVariant::String, prop_name.c_str());
    item->setValue(prop_value.c_str());
    
	return item;
}


/*
CMFCPropertyGridProperty * CPropertiesWnd::CreateProp(GASS::BaseReflectionObjectPtr obj, GASS::IProperty* prop,const GASS::PropertySettings *ps)
{
	if(!ps)
		return NULL;
	if(!ps->Visible)
		return NULL;
	const std::string prop_name = prop->GetName();
	const std::string prop_value = prop->GetValueAsString(obj.get());
	CMFCPropertyGridProperty* grid_prop = NULL;

	//first check if specific control type is present
	switch(ps->GUIControlType)
	{
	case GASS::CT_FILE_DIALOG:
		grid_prop = new CGASSFileProperty(obj,prop, _T(ps->Documentation.c_str()));
		break;
	}

	if(grid_prop  == NULL)
	{
		switch(prop->GetTypeID())
		{
		case GASS::PROP_BOOL:
			{
				grid_prop = new CGASSBoolProperty(obj,prop, _T(ps->Documentation.c_str()));
			}
			break;
		default:
			{
				grid_prop = new CGASSBaseProperty(obj,prop, _T(ps->Documentation.c_str()));
				grid_prop->AllowEdit(ps->Editable);
				for(size_t i = 0 ; i < ps->Restrictions.size() ; i++)
				{
					grid_prop->AddOption(ps->Restrictions[i].c_str());
				}

				if(ps->RestrictionProxyProperty != "")
				{
					boost::any str_vec;
					if(obj->GetPropertyByType(ps->RestrictionProxyProperty,str_vec))
					{
						std::vector<std::string> options = boost::any_cast<std::vector<std::string> >(str_vec); //potential crash, if property not match std::vector<std::string>!
						for(size_t i = 0 ; i < options.size() ; i++)
						{
							grid_prop->AddOption(options[i].c_str());
						}
					}
				}
			}
		}
	}
	return grid_prop;
}
*/



