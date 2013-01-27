#include "GASSPropertyWidget.h"
#include "GASSEd.h"
#include "VariantManager.h"
#include "VariantFactory.h"
#include "CustomTypes.h"
#include "Core/Utils/GASSColorRGB.h"
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
				gp.UpdateValue(str_name);
			}
			/*else if (varProp && varProp->propertyType() == QtVariantPropertyManager::ColorTypeId()) 
			{
				const std::string str_name = gp.m_Options[value.toInt()];
				gp.UpdateValue(str_name);
			}*/
			else
			{
				gp.UpdateValue(value_as_std_string);
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
	const GASS::ObjectSettings* os = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::EditorSystem>()->GetGUISettings()->GetObjectSettings(class_name);
	if(os && os->Visible) //we have settings
	{
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
			GASS::BaseComponentPtr comp = STATIC_CAST<GASS::BaseComponent>(comp_iter.getNext());
			std::string class_name = comp->GetRTTI()->GetClassName();
			const GASS::ObjectSettings* os = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::EditorSystem>()->GetGUISettings()->GetObjectSettings(class_name);
			if(os && os->Visible) //we have settings!
			{
				//os->GetProperty()->GUIControlType
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
	const GASS::ObjectSettings* os = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::EditorSystem>()->GetGUISettings()->GetObjectSettings(class_name);
	if(os && os->Visible) //we have settings
	{
		m_Root = m_VariantManager->addProperty(QtVariantPropertyManager::groupTypeId(),QLatin1String(scene->GetName().c_str()));
		addProperty(m_Root);
		GASS::PropertyVector props = scene->GetProperties();
		for(size_t i = 0;  i < props.size(); i++)
		{
			const GASS::PropertySettings* ps = os->GetProperty(props[i]->GetName());
			QtVariantProperty* grid_prop = CreateProp(scene,props[i],ps);
			if(grid_prop)
				m_Root->addSubProperty(grid_prop);
		}
		//list all scene managers
		GASS::SceneManagerIterator iter = scene->GetSceneManagers();
		while(iter.hasMoreElements())
		{
			GASS::SceneManagerPtr sm = iter.getNext();
			GASS::BaseReflectionObjectPtr obj = DYNAMIC_CAST<GASS::BaseReflectionObject>(sm);
			std::string class_name = obj->GetRTTI()->GetClassName();
			const GASS::ObjectSettings* os = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::EditorSystem>()->GetGUISettings()->GetObjectSettings(class_name);
			if(os && os->Visible) //we have settings!
			{
				QtVariantProperty* sm_root  = m_VariantManager->addProperty(QtVariantPropertyManager::groupTypeId(),QLatin1String(class_name.c_str()));
				m_Root->addSubProperty(sm_root);
				GASS::PropertyVector props = obj->GetProperties();
				for(size_t i = 0;  i < props.size(); i++)
				{
					const GASS::PropertySettings* ps = os->GetProperty(props[i]->GetName());
					QtVariantProperty* grid_prop = CreateProp(obj,props[i],ps);
					if(grid_prop)
						sm_root->addSubProperty(grid_prop);
				}
			}
		}
	}
	m_Polulating = false;
}



QtVariantProperty *GASSPropertyWidget::CreateProp(GASS::BaseReflectionObjectPtr obj, GASS::IProperty* prop,const GASS::PropertySettings *ps)
{
	if(ps && !ps->Visible)
		return NULL;
	const std::string prop_name = prop->GetName();
	const std::string prop_value = prop->GetValueAsString(obj.get());
	QtVariantProperty* item = NULL;
	GASSVariantProperty gp;
	GASS::IEnumProperty* enum_prop = dynamic_cast<GASS::IEnumProperty*>(prop);
	
	//if(prop->GetTypeID() == GASS::PROP_BOOL)
	
	if(enum_prop)
	{
		std::vector<std::string> options = enum_prop->GetEnumList("");
		bool multi_value = enum_prop->IsMultiValue();

		if(multi_value)
		{
			//CGASSMultiSelectionProperty *ms_prop = new CGASSMultiSelectionProperty(obj,prop, _T(ps->Documentation.c_str()));
			//ms_prop->AllowEdit(false);
			for(size_t i = 0 ; i < options.size() ; i++)
			{
				//ms_prop->AddItem(options[i]);
			}
			//grid_prop = ms_prop;
		}
		else
		{
			item = m_VariantManager->addProperty(QtVariantPropertyManager::enumTypeId(),prop_name.c_str());
			QStringList enumNames;
			int select = -1;
			for(size_t i = 0 ; i < options.size() ; i++)
			{
				gp.m_Options.push_back(options[i]);
				enumNames << options[i].c_str();
				if(prop_value == options[i])
					select = (int)i;
			}

			item->setAttribute(QLatin1String("enumNames"), enumNames);
			if(select > -1)
				item->setValue(select);
		}
	}
	else
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
			item->setValue(QColor(color.r,color.g,color.b));
		}
		else if(*prop->GetTypeID() == typeid(GASS::ResourceHandle))
		{
			if(ps)
			{
				GASS::ResourceManagerPtr rm = GASS::SimEngine::Get().GetResourceManager();
				GASS::ResourceGroupVector groups = rm->GetResourceGroups();
				std::vector<std::string> values;
				for(size_t i = 0; i < groups.size();i++)
				{
					GASS::ResourceGroupPtr group = groups[i];
					if(group->GetName() == ps->ResourceGroup)
					{
						GASS::ResourceVector res_vec = group->GetResourcesByType(ps->ResourceType);
						for(size_t j = 0; j < res_vec.size();j++)
						{
							values.push_back(res_vec[j].Name());
						}
					}
				}

				item = m_VariantManager->addProperty(QtVariantPropertyManager::enumTypeId(),prop_name.c_str());
				QStringList enumNames;
				int select = -1;
				for(size_t i = 0 ; i < values.size() ; i++)
				{
					gp.m_Options.push_back(values[i]);
					enumNames << values[i].c_str();
					if(prop_value == values[i])
						select = (int) i;
				}
				item->setAttribute(QLatin1String("enumNames"), enumNames);
				if(select > -1)
					item->setValue(select);
			}
		}
		if(*prop->GetTypeID() == typeid(GASS::FilePath))
		{
			//GASS::FilePath file_path = boost::any_cast<GASS::FilePath>(any_value);
			std::string filename = prop_value;
			filename = GASS::Misc::Replace(filename,"/","\\");
			item = m_VariantManager->addProperty(filePathTypeId(),prop_name.c_str());
			item->setValue(filename.c_str());
		}
	}

	if(item == NULL && ps)
	{
		//first check if specific control type is present
		switch(ps->GUIControlType)
		{
		case GASS::CT_FILE_DIALOG:
			{
				std::string filename = prop->GetValueAsString(obj.get());
				filename = GASS::Misc::Replace(filename,"/","\\");
				item = m_VariantManager->addProperty(filePathTypeId(),prop_name.c_str());
				item->setValue(filename.c_str());
				//item = new CGASSFileProperty(obj,prop, filename.c_str(), ps->FileControlSettings.c_str(),_T(ps->Documentation.c_str()));
			}
			break;
		}
	}
	if(item == NULL)
	{
		item = m_VariantManager->addProperty(QVariant::String, prop_name.c_str());
		item->setValue(prop_value.c_str());
	}
	
	gp.SetGASSData(obj,prop);
	m_PropMap[item] = gp;

	return item;
}

