#include "GASSPropertyWidget.h"
#include "GASSEd.h"
#include "VariantManager.h"
#include "VariantFactory.h"
#include "CustomTypes.h"
#include "Core/Utils/GASSColorRGB.h"
#include "Core/Reflection/GASSObjectMetaData.h"
#include "Modules/Editor/EditorApplication.h"
#include "Modules/Editor/EditorSystem.h"

GASSPropertyWidget::GASSPropertyWidget( GASSEd *parent): QtTreePropertyBrowser(parent), m_GASSEd(parent),m_Root(NULL),m_Polulating(false)
{
	registerCustomTypes();

	GASS::SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GASSPropertyWidget::OnLoadScene,GASS::PreSceneCreateEvent,0));
	GASS::SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GASSPropertyWidget::OnUnloadScene,GASS::SceneUnloadedEvent,0));
	

	m_VariantManager = new VariantManager(this);
	connect(m_VariantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(valueChanged(QtProperty *, const QVariant &)));
	m_VariantFactory = new VariantFactory(this);
	setFactoryForManager(m_VariantManager, m_VariantFactory);
	setPropertiesWithoutValueMarked(true);
	setRootIsDecorated(false);
	setMinimumSize(200,200);
}

GASSPropertyWidget::~GASSPropertyWidget()
{

}

void GASSPropertyWidget::valueChanged(QtProperty *property, const QVariant &value)
{
	//check if we just populate
	if(!m_Polulating)
	{
		QVariant v = value;
		
		const QString value_as_sstring = qVariantValue<QString>(v);
		const std::string value_as_std_string = value_as_sstring.toStdString();
		std::map<QtProperty*,GASSVariantProperty>::iterator iter = m_PropMap.find(property);
		if(iter != m_PropMap.end())
		{
			GASSVariantProperty gp = iter->second;
			QtVariantProperty *varProp = m_VariantManager->variantProperty(property);
			if (varProp && varProp->propertyType() == QtVariantPropertyManager::enumTypeId()) 
			{
				const std::string str_name = gp.m_Options[value.toInt()];
				gp.UpdateValueByString(str_name);
			}
			else if (varProp && varProp->propertyType() == QVariant::Color) 
			{
				QColor qcolor = v.value<QColor>();
				GASS::ColorRGB color(qcolor.redF(),qcolor.greenF(),qcolor.blueF());
				gp.UpdateValue(boost::any(color));
			}
			else
			{
				gp.UpdateValueByString(value_as_std_string);
			}
		}
	}
}

void GASSPropertyWidget::OnLoadScene(GASS::PreSceneCreateEventPtr message)
{
	GASS::ScenePtr scene = message->GetScene();
	scene->RegisterForMessage(REG_TMESS(GASSPropertyWidget::OnSceneObjectSelected,GASS::ObjectSelectionChangedEvent,0));
	scene->RegisterForMessage(REG_TMESS(GASSPropertyWidget::OnSceneSelected,GASS::SceneSelectionChangedEvent,0));
	m_Scene = scene;
}

void GASSPropertyWidget::OnUnloadScene(GASS::SceneUnloadedEventPtr message)
{

}

void GASSPropertyWidget::OnSceneObjectSelected(GASS::ObjectSelectionChangedEventPtr message)
{
	Show(message->GetSceneObject());
}

void GASSPropertyWidget::OnSceneSelected(GASS::SceneSelectionChangedEventPtr message)
{
	Show(message->GetScene());
}

void GASSPropertyWidget::Show(GASS::SceneObjectPtr object)
{
	m_Polulating = true;
	m_PropMap.clear();
	//iterate object attributes and components
	if(m_Root)
	{
		delete m_Root;
	}
	if(!object)
		return;
	std::string class_name  = object->GetRTTI()->GetClassName();
	
	if(object->HasMetaData() && object->GetMetaData()->GetFlags() & GASS::OF_VISIBLE)  
	{
		
		m_Root = m_VariantManager->addProperty(QtVariantPropertyManager::groupTypeId(),QLatin1String(object->GetName().c_str()));
		addProperty(m_Root);

		GASS::PropertyVector props = object->GetProperties();
		for(size_t i = 0;  i < props.size(); i++)
		{

			//const GASS::PropertySettings* ps = os->GetProperty(props[i]->GetName());
			QtVariantProperty* grid_prop = CreateProp(object,props[i]);
			if(grid_prop)
				m_Root->addSubProperty(grid_prop);
		}

		GASS::IComponentContainer::ComponentIterator comp_iter = object->GetComponents();
		while(comp_iter.hasMoreElements())
		{
			GASS::BaseComponentPtr comp = STATIC_PTR_CAST<GASS::BaseComponent>(comp_iter.getNext());
			std::string class_name = comp->GetRTTI()->GetClassName();
			if(comp->HasMetaData() &&  comp->GetMetaData()->GetFlags() & GASS::OF_VISIBLE) //we have settings!
			{
				//os->GetProperty()->GUIControlType
				QtVariantProperty* comp_root  = m_VariantManager->addProperty(QtVariantPropertyManager::groupTypeId(),QLatin1String(class_name.c_str()));

				//CMFCPropertyGridProperty* comp_root = new CMFCPropertyGridProperty(_T(class_name.c_str()));
				m_Root->addSubProperty(comp_root);

				GASS::PropertyVector props = comp->GetProperties();
				for(size_t i = 0;  i < props.size(); i++)
				{
					//const GASS::PropertySettings* ps = os->GetProperty(props[i]->GetName());
					QtVariantProperty* grid_prop = CreateProp(comp,props[i]);//,ps);
					if(grid_prop)
						comp_root->addSubProperty(grid_prop);
				}
			}
		}
	}
	m_Polulating = false;
}

void GASSPropertyWidget::Show(GASS::ScenePtr scene)
{
	m_Polulating = true;
	m_PropMap.clear();
	//iterate object attributes and components
	if(m_Root)
	{
		delete m_Root;
	}

	if(!scene)
		return;
	std::string class_name  = scene->GetRTTI()->GetClassName();
	if(scene->HasMetaData() && scene->GetMetaData()->GetFlags() & GASS::OF_VISIBLE) //we have settings
	{
		m_Root = m_VariantManager->addProperty(QtVariantPropertyManager::groupTypeId(),QLatin1String(scene->GetName().c_str()));
		addProperty(m_Root);
		GASS::PropertyVector props = scene->GetProperties();
		for(size_t i = 0;  i < props.size(); i++)
		{
			QtVariantProperty* grid_prop = CreateProp(scene,props[i]);//,ps);
			if(grid_prop)
				m_Root->addSubProperty(grid_prop);
		}
		//list all scene managers
		GASS::SceneManagerIterator iter = scene->GetSceneManagers();
		while(iter.hasMoreElements())
		{
			GASS::SceneManagerPtr sm = iter.getNext();
			GASS::BaseReflectionObjectPtr obj = DYNAMIC_PTR_CAST<GASS::BaseReflectionObject>(sm);
			std::string class_name = obj->GetRTTI()->GetClassName();
			if(obj->HasMetaData() && obj->GetMetaData()->GetFlags() & GASS::OF_VISIBLE) //we have settings!
			{
				QtVariantProperty* sm_root  = m_VariantManager->addProperty(QtVariantPropertyManager::groupTypeId(),QLatin1String(class_name.c_str()));
				m_Root->addSubProperty(sm_root);
				GASS::PropertyVector props = obj->GetProperties();
				for(size_t i = 0;  i < props.size(); i++)
				{
					QtVariantProperty* grid_prop = CreateProp(obj,props[i]);//,ps);
					if(grid_prop)
						sm_root->addSubProperty(grid_prop);
				}
			}
		}
	}
	m_Polulating = false;
}



QtVariantProperty *GASSPropertyWidget::CreateProp(GASS::BaseReflectionObjectPtr obj, GASS::IProperty* prop)//const GASS::PropertySettings *ps)
{
	//if(ps && !ps->Visible)
	//	return NULL;
	const std::string prop_name = prop->GetName();
	const std::string prop_value = prop->GetValueAsString(obj.get());
	QtVariantProperty* item = NULL;
	GASSVariantProperty gp;
	if(prop->HasMetaData())
	{
		GASS::BasePropertyMetaDataPtr meta_data = DYNAMIC_PTR_CAST<GASS::BasePropertyMetaData>(prop->GetMetaData());
		if(meta_data->GetFlags() & GASS::PF_VISIBLE)
		{

			bool editable = (meta_data->GetFlags() & GASS::PF_EDITABLE);
			std::string documentation = meta_data->GetAnnotation();

			if(DYNAMIC_PTR_CAST<GASS::EnumerationPropertyMetaData>(meta_data))
			{
				item = m_VariantManager->addProperty(QtVariantPropertyManager::enumTypeId(),prop_name.c_str());
				GASS::EnumerationPropertyMetaDataPtr enumeration_data = DYNAMIC_PTR_CAST<GASS::EnumerationPropertyMetaData>(meta_data);
				std::vector<std::string> enumeration = enumeration_data->GetEnumeration(obj);

				QStringList enumNames;
				int select = -1;
				for(size_t i = 0 ; i < enumeration.size() ; i++)
				{
					gp.m_Options.push_back(enumeration[i]);
					enumNames << enumeration[i].c_str();
					if(prop_value == enumeration[i])
						select = (int)i;
				}
				item->setAttribute(QLatin1String("enumNames"), enumNames);
				if(select > -1)
					item->setValue(select);
			}
			else if(!item)
			{
				if(*prop->GetTypeID() == typeid(bool))
				{
					item = m_VariantManager->addProperty(QVariant::Bool, prop_name.c_str());
					item->setValue(prop_value.c_str());
				}
				else if(*prop->GetTypeID() == typeid(GASS::ColorRGB))
				{
					boost::any any_value;
					prop->GetValue(obj.get(),any_value );
					GASS::ColorRGB color = boost::any_cast<GASS::ColorRGB>(any_value);
					item = m_VariantManager->addProperty(QVariant::Color, prop_name.c_str());
					item->setValue(QColor(color.r*255,color.g*255,color.b*255));
				}
				else if(*prop->GetTypeID() == typeid(GASS::FilePath))
				{
					//GASS::FilePath file_path = boost::any_cast<GASS::FilePath>(any_value);
					std::string filename = prop_value;
					filename = GASS::Misc::Replace(filename,"/","\\");
					item = m_VariantManager->addProperty(filePathTypeId(),prop_name.c_str());
					item->setValue(filename.c_str());
				}
				else
				{
					item = m_VariantManager->addProperty(QVariant::String, prop_name.c_str());
					item->setValue(prop_value.c_str());
				}
				item->setEnabled(editable);
			}
			gp.SetGASSData(obj,prop);
			m_PropMap[item] = gp;
		}
	}
/*	else if(item == NULL)
	{
		item = m_VariantManager->addProperty(QVariant::String, prop_name.c_str());
		item->setValue(prop_value.c_str());
	}*/
	return item;
}

