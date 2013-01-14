#include "GASSPropertyWidget.h"
#include "GASSEd.h"
#include "VariantManager.h"
#include "VariantFactory.h"
#include "CustomTypes.h"
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
	m_Scene = scene;
}

void GASSPropertyWidget::OnUnloadScene(GASS::SceneUnloadedEventPtr message)
{

}


void GASSPropertyWidget::OnSceneObjectSelected(GASS::ObjectSelectionChangedEventPtr message)
{
	Show(message->GetSceneObject());
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
			GASS::BaseComponentPtr comp = boost::shared_static_cast<GASS::BaseComponent>(comp_iter.getNext());
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

QtVariantProperty *GASSPropertyWidget::CreateProp(GASS::BaseReflectionObjectPtr obj, GASS::IProperty* prop,const GASS::PropertySettings *ps)
{
	if(!ps)
		return NULL;
	if(!ps->Visible)
		return NULL;
	const std::string prop_name = prop->GetName();
	const std::string prop_value = prop->GetValueAsString(obj.get());
	QtVariantProperty* item = NULL;
	GASSVariantProperty gp;
	GASS::IEnumProperty* enum_prop = dynamic_cast<GASS::IEnumProperty*>(prop);


	if(prop->GetTypeID() == GASS::PROP_BOOL)
	{
		item = m_VariantManager->addProperty(QVariant::Bool, prop_name.c_str());
		item->setValue(prop_value.c_str());
	}
	else if(enum_prop)
	{
		std::vector<std::string> options = enum_prop->GetEnumList();
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
					select = i;
			}

			item->setAttribute(QLatin1String("enumNames"), enumNames);
			if(select > -1)
				item->setValue(select);
		}
	}
	else
	{
		boost::any any_value;
		prop->GetValue(obj.get(), any_value);
		try
		{
			GASS::Resource resource = boost::any_cast<GASS::Resource>(any_value);
			GASS::ResourceSystemPtr rs = GASS::SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<GASS::IResourceSystem>();
			std::vector<std::string> values = rs->GetResourcesFromGroup(ps->ResourceType,ps->ResourceGroup);
				
			item = m_VariantManager->addProperty(QtVariantPropertyManager::enumTypeId(),prop_name.c_str());
			QStringList enumNames;
			int select = -1;
			for(size_t i = 0 ; i < values.size() ; i++)
			{
				gp.m_Options.push_back(values[i]);
				enumNames << values[i].c_str();
				if(prop_value == values[i])
						select = i;
			}
			item->setAttribute(QLatin1String("enumNames"), enumNames);
			if(select > -1)
				item->setValue(select);
				
		}
		catch(...)
		{

		}

		try
		{
			GASS::FilePath file_path = boost::any_cast<GASS::FilePath>(any_value);
			std::string filename = file_path.GetFullPath();
			filename = GASS::Misc::Replace(filename,"/","\\");
			item = m_VariantManager->addProperty(filePathTypeId(),prop_name.c_str());
			item->setValue(filename.c_str());
		}
		catch(...)
		{
			
				
		}
	}
	

	if(item == NULL)
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
		/*case GASS::CT_RESOURCE_COMBO:
			{
				GASS::ResourceSystemPtr rs = GASS::SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<GASS::IResourceSystem>();
				if(rs == NULL)
					GASS_EXCEPT(GASS::Exception::ERR_ITEM_NOT_FOUND,"No Resource Manager Found", "Scene::Load");
				std::vector<std::string> values = rs->GetResourceNames(ps->ResourceGroup);
				
				item = m_VariantManager->addProperty(QtVariantPropertyManager::enumTypeId(),prop_name.c_str());
				QStringList enumNames;
				int select = -1;
				for(size_t i = 0 ; i < values.size() ; i++)
				{
					gp.m_Options.push_back(values[i]);
					enumNames << values[i].c_str();
					if(prop_value == values[i])
						select = i;
				}

				item->setAttribute(QLatin1String("enumNames"), enumNames);
				if(select > -1)
					item->setValue(prop_value.c_str());
			}
			break;
		case GASS::CT_CONTENT_COMBO:
			{
				GASS::ResourceSystemPtr rs = GASS::SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<GASS::IResourceSystem>();
				if(rs == NULL)
					GASS_EXCEPT(GASS::Exception::ERR_ITEM_NOT_FOUND,"No Resource Manager Found", "Scene::Load");
				std::vector<std::string> values = rs->GetContentNamesFromGroup(ps->ComboContentType,ps->ResourceGroup);
				
				item = m_VariantManager->addProperty(QtVariantPropertyManager::enumTypeId(),prop_name.c_str());
				QStringList enumNames;
				int select = -1;
				for(size_t i = 0 ; i < values.size() ; i++)
				{
					gp.m_Options.push_back(values[i]);
					enumNames << values[i].c_str();
					if(prop_value == values[i])
						select = i;
				}
				item->setAttribute(QLatin1String("enumNames"), enumNames);
				if(select > -1)
					item->setValue(prop_value.c_str());
			}
			break;
		case GASS::CT_OBJECT_REFERENCE:
			{
				//grid_prop = new CGASSBaseProperty(obj,prop, _T(ps->Documentation.c_str()));
				//grid_prop->AllowEdit(false);
				//std::vector<std::string> options = GetObjectsFromFilter(obj,ps);
				//for(size_t i = 0 ; i < options.size() ; i++)
				//{
				//	grid_prop->AddOption(options[i].c_str());
				//}
			}
			break;*/
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


	

/*
QtVariantProperty * GASSPropertyWidget::CreateProp(GASS::BaseReflectionObjectPtr obj, GASS::IProperty* prop,const GASS::PropertySettings *ps)
{
if(!ps)
return NULL;
if(!ps->Visible)
return NULL;
const std::string prop_name = prop->GetName();
const std::string prop_value = prop->GetValueAsString(obj.get());
GASS::IVectorProperty* vector_prop = dynamic_cast<GASS::IVectorProperty*>(prop);

//Check if this is a vector propery
if(vector_prop)
{
switch(ps->GUIControlType)
{
case GASS::CT_OBJECT_REFERENCE:
{
//CGASSMultiSelectionProperty *ms_prop = new CGASSMultiSelectionProperty(obj,prop, _T(ps->Documentation.c_str()));
//grid_prop = ms_prop;
//grid_prop->AllowEdit(false);
//std::vector<std::string> options = GetObjectsFromFilter(obj,ps);
//for(size_t i = 0 ; i < options.size() ; i++)
//{
//	ms_prop->AddItem(options[i].c_str());
//}
}
break;
}
if(!grid_prop)
{
//show multi selection dialog
if(ps->Restrictions.size() > 0)
{
//	CGASSMultiSelectionProperty* ms_prop= new CGASSMultiSelectionProperty(obj,prop, _T(ps->Documentation.c_str()));
//	grid_prop = ms_prop;
//	grid_prop->AllowEdit(ps->Editable);
//	for(size_t i = 0 ; i < ps->Restrictions.size() ; i++)
//	{
//		ms_prop->AddItem(ps->Restrictions[i]);
//	}
}
else if(ps->RestrictionProxyProperty != "")
{
//CGASSMultiSelectionProperty* ms_prop = new CGASSMultiSelectionProperty(obj,prop, _T(ps->Documentation.c_str()));
//grid_prop = ms_prop;

boost::any str_vec;
if(obj->GetPropertyByType(ps->RestrictionProxyProperty,str_vec))
{
try
{
std::vector<std::string> options = boost::any_cast<std::vector<std::string> >(str_vec); //potential crash, if property not match std::vector<std::string>!
for(size_t i = 0 ; i < options.size() ; i++)
{
//ms_prop->AddItem(options[i]);
}
}
catch(...)
{

}
}
}
}
}

GASS::IEnumProperty* enum_prop = dynamic_cast<GASS::IEnumProperty*>(prop);
if(enum_prop)
{
std::vector<std::string> options = enum_prop->GetEnumList();
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
//grid_prop = new CGASSBaseProperty(obj,prop, _T(ps->Documentation.c_str()));
//grid_prop->AllowEdit(false);
//for(size_t i = 0 ; i < options.size() ; i++)
//{
//	grid_prop->AddOption(options[i].c_str());
//}
}
}

if(grid_prop  == NULL)
{
//first check if specific control type is present
switch(ps->GUIControlType)
{
case GASS::CT_FILE_DIALOG:
{
std::string filename = prop->GetValueAsString(obj.get());
filename = GASS::Misc::Replace(filename,"/","\\");
//grid_prop = new CGASSFileProperty(obj,prop, filename.c_str(), ps->FileControlSettings.c_str(),_T(ps->Documentation.c_str()));
}
break;
case GASS::CT_OBJECT_REFERENCE:
{
//grid_prop = new CGASSBaseProperty(obj,prop, _T(ps->Documentation.c_str()));
//grid_prop->AllowEdit(false);
//std::vector<std::string> options = GetObjectsFromFilter(obj,ps);
//for(size_t i = 0 ; i < options.size() ; i++)
//{
//	grid_prop->AddOption(options[i].c_str());
//}
}
break;
}
}

if(grid_prop  == NULL)
{
boost::any any_value;
prop->GetValue(obj.get(), any_value);


switch(prop->GetTypeID())
{
case GASS::PROP_BOOL:
{
bool value = boost::any_cast<bool>(any_value);
//grid_prop = new CGASSBoolProperty(obj,prop, value, _T(ps->Documentation.c_str()));
}
break;
default:
{
//grid_prop = new CGASSBaseProperty(obj,prop, _T(ps->Documentation.c_str()));
//grid_prop->AllowEdit(ps->Editable);
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
}*/
