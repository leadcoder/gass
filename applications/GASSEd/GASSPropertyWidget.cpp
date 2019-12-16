#include "GASSPropertyWidget.h"
#include "GASSEd.h"
#include "VariantManager.h"
#include "VariantFactory.h"
#include "CustomTypes.h"
#include "Core/Utils/GASSColorRGB.h"

//#include "Core/Reflection/GASSObjectMetaData.h"
#include "Modules/Editor/EditorApplication.h"
#include "Modules/Editor/EditorSystem.h"

GASSPropertyWidget::GASSPropertyWidget(GASSEd *parent) : QtTreePropertyBrowser(parent), m_GASSEd(parent), m_Root(NULL), m_Polulating(false)
{
	registerCustomTypes();

	GASS::SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GASSPropertyWidget::OnLoadScene, GASS::PreSceneCreateEvent, 0));
	GASS::SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GASSPropertyWidget::OnUnloadScene, GASS::SceneUnloadedEvent, 0));


	m_VariantManager = new VariantManager(this);

	m_VariantFactory = new VariantFactory(this);
	setFactoryForManager(m_VariantManager, m_VariantFactory);
	setPropertiesWithoutValueMarked(true);
	setRootIsDecorated(false);
	setMinimumSize(200, 200);
	connect(m_VariantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(slotValueChanged(QtProperty *, const QVariant &)));

}

GASSPropertyWidget::~GASSPropertyWidget()
{

}

void GASSPropertyWidget::slotValueChanged(QtProperty *property, const QVariant &value)
{
	//check if we just populate
	if (!m_Polulating)
	{
		QVariant v = value;

		const QString value_as_sstring = v.value<QString>();
		const std::string value_as_std_string = value_as_sstring.toStdString();
		std::map<QtProperty*, GASSVariantProperty>::iterator iter = m_PropMap.find(property);
		if (iter != m_PropMap.end())
		{
			GASSVariantProperty gp = iter->second;
			QtVariantProperty *varProp = m_VariantManager->variantProperty(property);
			if (varProp && varProp->propertyType() == QtVariantPropertyManager::enumTypeId())
			{
				const std::string str_name = gp.m_Options[value.toInt()];
				gp.UpdateValueByString(str_name);
			}
			else if (varProp && varProp->propertyType() == QtVariantPropertyManager::flagTypeId())
			{
				int flags = value.toInt();
				std::string str_name;
				for (size_t i = 0; i < gp.m_Options.size(); i++)
				{
					int flag = 1 << i;
					if (flags & flag)
						str_name = str_name + " " + gp.m_Options[i];
				}
				gp.UpdateValueByString(str_name);
			}
			else if (varProp && varProp->propertyType() == QVariant::Color)
			{
				QColor qcolor = v.value<QColor>();
				GASS::ColorRGB color(qcolor.redF(), qcolor.greenF(), qcolor.blueF());
				gp.UpdateValue(color);
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
	scene->RegisterForMessage(REG_TMESS(GASSPropertyWidget::OnSceneObjectSelected, GASS::EditorSelectionChangedEvent, 0));
	scene->RegisterForMessage(REG_TMESS(GASSPropertyWidget::OnSceneSelected, GASS::SceneSelectionChangedEvent, 0));
	m_Scene = scene;
}

void GASSPropertyWidget::OnUnloadScene(GASS::SceneUnloadedEventPtr message)
{

}

void GASSPropertyWidget::OnSceneObjectSelected(GASS::EditorSelectionChangedEventPtr message)
{
	Show(message->GetFirstSelected().lock());
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
	if (m_Root)
	{
		delete m_Root;
		m_Root = NULL;
	}
	if (!object)
		return;
	std::string class_name = object->GetRTTI()->GetClassName();

	if (object->HasMetaData() && object->GetMetaData()->GetFlags() & GASS::OF_VISIBLE)
	{

		m_Root = m_VariantManager->addProperty(QtVariantPropertyManager::groupTypeId(), QLatin1String(object->GetName().c_str()));
		addProperty(m_Root);

		GASS::PropertyVector props = object->GetProperties();
		for (size_t i = 0; i < props.size(); i++)
		{

			//const GASS::PropertySettings* ps = os->GetProperty(props[i]->GetName());
			QtVariantProperty* grid_prop = CreateProp(object, props[i]);
			if (grid_prop)
				m_Root->addSubProperty(grid_prop);
		}

		GASS::ComponentContainer::ComponentIterator comp_iter = object->GetComponents();
		while (comp_iter.hasMoreElements())
		{
			GASS::ComponentPtr comp = GASS_STATIC_PTR_CAST<GASS::Component>(comp_iter.getNext());
			std::string class_name = comp->GetRTTI()->GetClassName();
			if (comp->HasMetaData() && comp->GetMetaData()->GetFlags() & GASS::OF_VISIBLE) //we have settings!
			{
				//os->GetProperty()->GUIControlType
				QtVariantProperty* comp_root = m_VariantManager->addProperty(QtVariantPropertyManager::groupTypeId(), QLatin1String(class_name.c_str()));

				//CMFCPropertyGridProperty* comp_root = new CMFCPropertyGridProperty(_T(class_name.c_str()));
				m_Root->addSubProperty(comp_root);

				GASS::PropertyVector props = comp->GetProperties();
				for (size_t i = 0; i < props.size(); i++)
				{
					//const GASS::PropertySettings* ps = os->GetProperty(props[i]->GetName());
					QtVariantProperty* grid_prop = CreateProp(comp, props[i]);//,ps);
					if (grid_prop)
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
	if (m_Root)
	{
		delete m_Root;
		m_Root = NULL;
	}

	if (!scene)
		return;
	std::string class_name = scene->GetRTTI()->GetClassName();
	if (scene->HasMetaData() && scene->GetMetaData()->GetFlags() & GASS::OF_VISIBLE) //we have settings
	{
		m_Root = m_VariantManager->addProperty(QtVariantPropertyManager::groupTypeId(), QLatin1String(scene->GetName().c_str()));
		addProperty(m_Root);
		GASS::PropertyVector props = scene->GetProperties();
		for (size_t i = 0; i < props.size(); i++)
		{
			QtVariantProperty* grid_prop = CreateProp(scene, props[i]);//,ps);
			if (grid_prop)
				m_Root->addSubProperty(grid_prop);
		}
		//list all scene managers
		GASS::SceneManagerIterator iter = scene->GetSceneManagers();
		while (iter.hasMoreElements())
		{
			GASS::SceneManagerPtr sm = iter.getNext();
			GASS::BaseReflectionObjectPtr obj = GASS_DYNAMIC_PTR_CAST<GASS::BaseReflectionObject>(sm);
			std::string class_name = obj->GetRTTI()->GetClassName();
			if (obj->HasMetaData() && obj->GetMetaData()->GetFlags() & GASS::OF_VISIBLE) //we have settings!
			{
				QtVariantProperty* sm_root = m_VariantManager->addProperty(QtVariantPropertyManager::groupTypeId(), QLatin1String(class_name.c_str()));
				m_Root->addSubProperty(sm_root);
				GASS::PropertyVector props = obj->GetProperties();
				for (size_t i = 0; i < props.size(); i++)
				{
					QtVariantProperty* grid_prop = CreateProp(obj, props[i]);//,ps);
					if (grid_prop)
						sm_root->addSubProperty(grid_prop);
				}
			}
		}
	}
	m_Polulating = false;
}



QtVariantProperty *GASSPropertyWidget::CreateProp(GASS::BaseReflectionObjectPtr obj, GASS::IProperty* prop)//const GASS::PropertySettings *ps)
{
	const std::string prop_name = prop->GetName();
	const std::string prop_value = prop->GetValueAsString(obj.get());
	QtVariantProperty* item = NULL;
	GASSVariantProperty gp;
	if (prop->GetFlags() & GASS::PF_VISIBLE)
	{
		if (prop->HasMetaData())
		{
			GASS::PropertyMetaDataPtr meta_data = prop->GetMetaData();
			const bool editable = (prop->GetFlags() & GASS::PF_EDITABLE);
			const std::string documentation = prop->GetDescription();

			if (GASS_DYNAMIC_PTR_CAST<GASS::ISceneObjectEnumerationPropertyMetaData>(meta_data))
			{
				item = m_VariantManager->addProperty(QtVariantPropertyManager::enumTypeId(), prop_name.c_str());
				GASS::SceneObjectEnumerationPropertyMetaDataPtr enumeration_data = GASS_DYNAMIC_PTR_CAST<GASS::ISceneObjectEnumerationPropertyMetaData>(meta_data);
				std::vector<GASS::SceneObjectPtr> enumeration = enumeration_data->GetEnumeration(obj);
				QStringList enumNames;
				int select = -1;
				for (size_t i = 0; i < enumeration.size(); i++)
				{
					std::string obj_name = enumeration[i]->GetName();
					gp.m_Options.push_back(obj_name);
					enumNames << obj_name.c_str();
					if (prop_value == obj_name)
						select = (int)i;
				}
				item->setAttribute(QLatin1String("enumNames"), enumNames);
				if (select > -1)
					item->setValue(select);
			}
		/*	else if (GASS_DYNAMIC_PTR_CAST<GASS::EnumerationPropertyMetaData>(meta_data))
			{
				item = m_VariantManager->addProperty(QtVariantPropertyManager::enumTypeId(), prop_name.c_str());
				GASS::EnumerationPropertyMetaDataPtr enumeration_data = GASS_DYNAMIC_PTR_CAST<GASS::EnumerationPropertyMetaData>(meta_data);
				std::vector<std::string> enumeration = enumeration_data->GetEnumeration(obj);

				QStringList enumNames;
				int select = -1;
				for (size_t i = 0; i < enumeration.size(); i++)
				{
					gp.m_Options.push_back(enumeration[i]);
					enumNames << enumeration[i].c_str();
					if (prop_value == enumeration[i])
						select = (int)i;
				}
				item->setAttribute(QLatin1String("enumNames"), enumNames);
				if (select > -1)
					item->setValue(select);
			}*/
			else if (GASS_DYNAMIC_PTR_CAST<GASS::FilePathPropertyMetaData>(meta_data))
			{
				item = m_VariantManager->addProperty(QtVariantPropertyManager::enumTypeId(), prop_name.c_str());
				GASS::FilePathPropertyMetaDataPtr file_path_data = GASS_DYNAMIC_PTR_CAST<GASS::FilePathPropertyMetaData>(meta_data);
				std::vector<std::string> exts = file_path_data->GetExtensions();
				GASS::FilePathPropertyMetaData::FilePathEditType type = file_path_data->GetType();

				std::string filter;
				for (size_t i = 0; i < exts.size(); i++)
				{
					if (i != 0)
						filter += " ";
					filter += "*.";
					filter += exts[i];
				}

				std::string filename = prop_value;
				filename = GASS::StringUtils::Replace(filename, "/", "\\");

				//m_VariantManager->setAttribute(item,QLatin1String("filter"),QVariant("*.png *.jpg"));
				switch (type)
				{
				case GASS::FilePathPropertyMetaData::IMPORT_FILE:
					item = m_VariantManager->addProperty(filePathTypeId(), prop_name.c_str());
					item->setValue(filename.c_str());
					item->setAttribute(QLatin1String("filter"), QVariant(filter.c_str()));
					break;
				case GASS::FilePathPropertyMetaData::EXPORT_FILE:
					item = m_VariantManager->addProperty(newFileTypeId(), prop_name.c_str());
					item->setValue(filename.c_str());
					item->setAttribute(QLatin1String("filter"), QVariant(filter.c_str()));
					break;
				case GASS::FilePathPropertyMetaData::PATH_SELECTION:
					break;
				}
			}
		}
		else if (!item)
		{
			const bool editable = (prop->GetFlags() & GASS::PF_EDITABLE);
			const bool multi = (prop->GetFlags() & GASS::PF_MULTI_OPTIONS);
			if (prop->HasOptions())
			{
				std::vector<std::string> options = prop->GetStringOptions();
				if (prop->HasObjectOptions())
				{
					std::vector<std::string> object_options = prop->GetStringOptionsByObject(obj.get());
					options.insert(options.end(), object_options.begin(), object_options.end());
				}

				if (multi)
				{
					item = m_VariantManager->addProperty(QtVariantPropertyManager::flagTypeId(), prop_name.c_str());
					QStringList enumNames;
					int flags = 0;
					for (size_t i = 0; i < options.size(); i++)
					{
						gp.m_Options.push_back(options[i]);
						enumNames << options[i].c_str();
						item->setAttribute(QLatin1String("flagNames"), enumNames);
						if (prop_value.find(options[i]) != std::string::npos)
							flags = flags | (1 << i);
					}
					item->setValue(flags);
				}
				else
				{
					item = m_VariantManager->addProperty(QtVariantPropertyManager::enumTypeId(), prop_name.c_str());

					QStringList enumNames;
					int select = -1;
					for (size_t i = 0; i < options.size(); i++)
					{
						gp.m_Options.push_back(options[i]);
						enumNames << options[i].c_str();
						if (prop_value == options[i])
							select = (int)i;
					}
					item->setAttribute(QLatin1String("enumNames"), enumNames);
					if (select > -1)
						item->setValue(select);
				}
				item->setEnabled(true);
			}
			else
			{
				if (*prop->GetTypeID() == typeid(bool))
				{
					item = m_VariantManager->addProperty(QVariant::Bool, prop_name.c_str());
					item->setValue(prop_value.c_str());
				}
				else if (*prop->GetTypeID() == typeid(GASS::ColorRGB))
				{
					GASS::ColorRGB color;
					obj->GetPropertyValue(prop, color);
					item = m_VariantManager->addProperty(QVariant::Color, prop_name.c_str());
					item->setValue(QColor(color.r * 255, color.g * 255, color.b * 255));
				}
				else if (*prop->GetTypeID() == typeid(GASS::FilePath))
				{
					std::string filename = prop_value;
					filename = GASS::StringUtils::Replace(filename, "/", "\\");
					item = m_VariantManager->addProperty(filePathTypeId(), prop_name.c_str());
					item->setValue(filename.c_str());
					//m_VariantManager->setAttribute(item,QLatin1String("filter"),QVariant("*.png *.jpg"));
					//item->setAttribute(QLatin1String("filter"),QVariant("*.png;*.jpg"));
				}
				else
				{
					item = m_VariantManager->addProperty(QVariant::String, prop_name.c_str());
					item->setValue(prop_value.c_str());
				}
				item->setEnabled(editable);
			}
		}
		gp.SetGASSData(obj, prop);
		m_PropMap[item] = gp;
	}
	/*	else if(item == NULL)
		{
			item = m_VariantManager->addProperty(QVariant::String, prop_name.c_str());
			item->setValue(prop_value.c_str());
		}*/
	return item;
}

