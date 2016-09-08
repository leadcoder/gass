/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/										*
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
*                                                                           *
* GASS is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* GASS is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with GASS. If not, see <http://www.gnu.org/licenses/>.              *
*****************************************************************************/

#include <stdio.h>
#include <iostream>
#include <fstream>

#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/Utils/GASSEnumBinder.h"
#ifdef WIN32
#include <conio.h>
#endif
#ifndef WIN32 //implement getch for linux
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int getch( ) {
	struct termios oldt,newt;
	int ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
}
#endif


//user defined struct that can be registered as property
struct MyGearBox
{
	int Gears;
	bool Automatic;

	friend std::ostream& operator << (std::ostream& os, const MyGearBox& gb)
	{
		os << gb.Gears << " " << gb.Automatic;
		return os;
	}

	friend std::istream& operator >> (std::istream& is, MyGearBox& gb)
	{
		if(!(is >> gb.Gears  >> gb.Automatic))
		{
			std::cerr << "Failed to parse GearBox";
		}
		return is;
	}
};

//vector derived from std::vector to enable std::vector based attributes
template<typename T>
struct MyVector : std::vector<T>
{
	friend std::ostream& operator << (std::ostream& os, const std::vector<T>& v)
	{
		for(size_t i = 0; i < v.size(); i++)
		{
			if(i > 0)
				os << " ";
			os << v[i];
		}
		return os;
	}

	friend std::istream& operator >> (std::istream& is, std::vector<T>& v)
	{
		T value;
		v.clear();
		while(is.peek() != EOF && is >> value)
		{
			v.push_back(value);
		}
		return is;
	}
};


struct MyWheel
{
	MyWheel():Radius(1.0),Slip(0.1){}
	MyWheel(const std::string &name, double rad,double slip):Name(name),
		Radius(rad),
		Slip(slip){}
	std::string Name;
	double Radius;
	double Slip;

	friend std::ostream& operator << (std::ostream& os, const MyWheel& w)
	{
		os << w.Name << " " << w.Radius << " " << w.Slip;
		return os;
	}

	friend std::istream& operator >> (std::istream& is, MyWheel& w)
	{
		if(!(is >> w.Name >> w.Radius  >> w.Slip))
		{
			std::cerr << "Failed to parse Wheel";
		}
		return is;
	}
};


//Base class for all cars, dervied from the GASS::BaseReflectionObject to enable property reflection
class MyCar : public GASS::Reflection<MyCar, GASS::BaseReflectionObject>
{
public:
	MyCar() : m_Pos(0,0,0)
	{
		m_GearBox.Automatic = false;
		m_GearBox.Gears = 4;
		m_Wheels.push_back(MyWheel("FrontLeft",1.0,0.1));
		m_Wheels.push_back(MyWheel("FrontRight",1.0,0.1));
		m_Wheels.push_back(MyWheel("RearLeft",1.0,0.1));
		m_Wheels.push_back(MyWheel("RearRight",1.0,0.1));
	}

	~MyCar()
	{

	}

	//Called once on program startup
	static void RegisterReflection()
	{
		//register our attributes to the rtti system
		RegisterProperty<GASS::Vec3>("Position", &MyCar::GetPosition, &MyCar::SetPosition);
		RegisterProperty<GASS::Quaternion>("Rotation", &MyCar::GetRotation, &MyCar::SetRotation);
		RegisterProperty<std::string>("Description", &MyCar::GetDescription, &MyCar::SetDescription);
		RegisterProperty<MyGearBox>("GearBox", &MyCar::GetGearBox, &MyCar::SetGearBox);
		RegisterProperty<MyVector<MyWheel> >("Wheels", &MyCar::GetWheels, &MyCar::SetWheels);
		RegisterProperty<std::vector<GASS::Vec3> >("TagPoints", &MyCar::GetTagPoints, &MyCar::SetTagPoints);



	}

	//get/set section
	GASS::Vec3 GetPosition()const {return m_Pos;}
	void SetPosition(const GASS::Vec3 &pos){m_Pos = pos;}
	GASS::Quaternion GetRotation()const {return m_Rot;}
	void SetRotation(const GASS::Quaternion &pos){m_Rot = pos;}
	void SetDescription(const std::string &desc) {m_Description = desc;}
	std::string GetDescription() const {return m_Description;}
	void SetGearBox(const MyGearBox &gb) {m_GearBox = gb;}
	MyGearBox GetGearBox() const {return m_GearBox;}
	MyVector<MyWheel> GetWheels() const {return m_Wheels;}
	void SetWheels(const MyVector<MyWheel> &value)  {m_Wheels = value;}
	std::vector<GASS::Vec3> GetTagPoints() const {return m_TagPoints;}
	void SetTagPoints(const std::vector<GASS::Vec3>  &value)  {m_TagPoints = value;}



	//If you just want regular get/set you can use the
	//convenience macro, ADD_PROPERTY, that will implement simple get/set.
	//ei. ADD_PROPERTY(std::string,Description)

private:
	GASS::Vec3 m_Pos;
	GASS::Quaternion m_Rot;
	std::string m_Description;
	MyGearBox m_GearBox;
	MyVector<MyWheel> m_Wheels;
	std::vector<GASS::Vec3>  m_TagPoints;
};


//user defined enum
enum EngineType
{
	ET_V12,
	ET_V8,
	ET_V6
};


START_ENUM_BINDER(EngineType, EngineTypeBinder)
	BIND(ET_V12)
	BIND(ET_V8)
	BIND(ET_V6)
END_ENUM_BINDER(EngineType, EngineTypeBinder)

class MyDerivedCar;

class MyDerivedCar : public GASS::Reflection<MyDerivedCar, MyCar>
{
public:
	//Use custom meta data for color info
	class MyColorPropertyMetaData : public GASS::EnumerationPropertyMetaData
	{
	public:
		MyColorPropertyMetaData(const std::string &annotation, GASS::PropertyFlags flags): GASS::EnumerationPropertyMetaData(annotation,flags,false)
		{

		}
		virtual std::vector<std::string> GetEnumeration(GASS::BaseReflectionObjectPtr object) const
		{
			std::vector<std::string> content;
			GASS_SHARED_PTR<MyDerivedCar> car = GASS_DYNAMIC_PTR_CAST<MyDerivedCar>(object);
			if(car) //get enumeration from unique instance
				content = car->GetColorEnumeration();
			else //if some other class use this metadata we just add some static colors
			{
				content.push_back("Blue");
				content.push_back("Red");
				content.push_back("Green");
			}
			return content;
		}
	};
	typedef GASS_SHARED_PTR<MyColorPropertyMetaData> MyCustomPropertyMetaDataPtr;

	MyDerivedCar() : m_EngineType(ET_V8),
		m_EnginePower(120)
	{
		m_ColorEnums.push_back("Yellow");
		m_ColorEnums.push_back("Pink");
	}

	~MyDerivedCar()
	{

	}

	//Called once on program startup
	static void RegisterReflection()
	{
		//Create some meta data for the EnginePower property ,
		GASS::BasePropertyMetaDataPtr ep_meta_data(new GASS::FloatMaxMinPropertyMetaData(
			"Engine power [HP]",//annotation
			GASS::PF_VISIBLE | GASS::PF_EDITABLE, //editor flags
			100, //min engine power value
			200)); //max engine power value

		//register our attributes to the RTTI system
		RegisterProperty<float>("EnginePower", &MyDerivedCar::GetEnginePower, &MyDerivedCar::SetEnginePower,ep_meta_data);

		//Create some meta data for the EngineType property
		GASS::BasePropertyMetaDataPtr et_meta_data(new GASS::EnumerationProxyPropertyMetaData(
			"Engine type",//annotation
			GASS::PF_VISIBLE, //editor flags
			&EngineTypeBinder::GetStringEnumeration)); //delegate enumeration function here

		RegisterProperty<EngineTypeBinder>("EngineType", &MyDerivedCar::GetEngineType, &MyDerivedCar::SetEngineType,et_meta_data);

		//Create some meta data for the Color property that give us possibility to have instance based enumeration,
		GASS::BasePropertyMetaDataPtr color_meta_data(new MyColorPropertyMetaData(
			"Color name",//annotation
			GASS::PF_VISIBLE));
		RegisterProperty<std::string>("Color", &MyDerivedCar::GetColor, &MyDerivedCar::SetColor,color_meta_data);
	}

	//get/set section
	float GetEnginePower()const {return m_EnginePower;}
	void SetEnginePower(const float value){m_EnginePower = value;}
	EngineTypeBinder GetEngineType()const {return m_EngineType;}
	void SetEngineType(const EngineTypeBinder value){m_EngineType = value;}
	std::string GetColor() const {return m_Color;}
	void SetColor(const std::string &value) {m_Color = value;}

	//used for instance based color enumeration,
	//note that this could be a property by it self enabling
	//instance based enumeration.
	std::vector<std::string> GetColorEnumeration() const {return m_ColorEnums;}
	void SetColorEnumeration(std::vector<std::string> value) {m_ColorEnums = value;}
private:
	float m_EnginePower;
	EngineTypeBinder m_EngineType;
	std::vector<std::string> m_ColorEnums;
	std::string m_Color;
};


//print property name and value for all RTTI-listed properties of object derived from GASS::BaseReflectionObject
void PrintProperties(GASS::BaseReflectionObjectPtr bro)
{
	//Use RTTI functionality to get properties for MyCar
	GASS::PropertyVector props = bro->GetProperties();
	std::cout << "List all properties for BaseReflectionObject derived object with class name: " << bro->GetRTTI()->GetClassName() << std::endl;
	for(size_t i = 0;  i < props.size(); i++)
	{
		const std::string prop_name = props[i]->GetName();
		const std::string prop_value = props[i]->GetValueAsString(bro.get());
		//You can also get attribute values like this
		//std::string prop_value; my_car->GetPropertyByString(prop_name,prop_value);
		std::cout << "PropertyName:" << prop_name << " Value:" << prop_value << std::endl;
	}
}

//print property type and access value by type
void PrintPropertyTypes(GASS::BaseReflectionObjectPtr bro)
{
	GASS::PropertyVector props = bro->GetProperties();
	//Check property types
	for(size_t i = 0;  i < props.size(); i++)
	{
		const std::string prop_name = props[i]->GetName();
		GASS_ANY any_value;

		//Do type checking by typeid
		if(*props[i]->GetTypeID() == typeid(GASS::Vec3))
		{
			//get property value by using boost any
			props[i]->GetValue(bro.get(), any_value);
			GASS::Vec3 vec3_value = GASS_ANY_CAST<GASS::Vec3>(any_value);
			std::cout << "Property " << prop_name << " is Vec3 and has value:" << vec3_value << "\n";
		}
		else if(*props[i]->GetTypeID() == typeid(GASS::Quaternion))
		{
			props[i]->GetValue(bro.get(), any_value);
			GASS::Quaternion quaternion_value = GASS_ANY_CAST<GASS::Quaternion>(any_value);
			std::cout << "Property " << prop_name << " is Quaternion and has value:" << quaternion_value << "\n";
		}
		else if(*props[i]->GetTypeID() == typeid(std::string))
		{
			std::cout << "Property " << prop_name << " is std::string and has value:" << props[i]->GetValueAsString(bro.get()) << "\n";
		}
		else if(*props[i]->GetTypeID() == typeid(MyGearBox))
		{
			props[i]->GetValue(bro.get(), any_value);
			MyGearBox gb_value = GASS_ANY_CAST<MyGearBox>(any_value);
			std::cout << "Property " << prop_name << " is GearBox, Numbers of gears:" << gb_value.Gears;
			if(gb_value.Automatic)
				std::cout << " Automatic mode\n";
			else
				std::cout << " Manual mode\n";
		}
		else if(*props[i]->GetTypeID() == typeid(MyVector<MyWheel>))
		{
			props[i]->GetValue(bro.get(), any_value);
			MyVector<MyWheel> wheels = GASS_ANY_CAST<MyVector<MyWheel> >(any_value);
			std::cout << "Property " << prop_name << " is Wheel vector holding:" << wheels.size() << " wheels\n";
			for(size_t j = 0; j< wheels.size(); j++)
			{
				std::cout << "\t" << wheels[j].Name << " Radius:" << wheels[j].Radius << " Slip:" << wheels[j].Slip << "\n";
			}
		}
		else
		{
			const std::string prop_value = props[i]->GetValueAsString(bro.get());
			std::cout << "Property " << prop_name << " type is unknown and value is:" << prop_value << "\n";
		}
	}
}


//print property meta-data for object derived from GASS::BaseReflectionObject
void PrintPropertyMetaData(GASS::BaseReflectionObjectPtr bro)
{
	//Use RTTI functionality to get properties for MyCar
	GASS::PropertyVector props = bro->GetProperties();
	std::cout << "List all properties that has metadata" << std::endl;
	for(size_t i = 0;  i < props.size(); i++)
	{
		const std::string prop_name = props[i]->GetName();
		const std::string prop_value = props[i]->GetValueAsString(bro.get());

		if(props[i]->HasMetaData())
		{
			GASS::BasePropertyMetaDataPtr meta_data = GASS_DYNAMIC_PTR_CAST<GASS::BasePropertyMetaData>(props[i]->GetMetaData());
			std::cout << "Property:" << prop_name  << "  Annotation:\"" << meta_data->GetAnnotation() << "\"";
			GASS::FloatMaxMinPropertyMetaDataPtr float_meta_data = GASS_DYNAMIC_PTR_CAST<GASS::FloatMaxMinPropertyMetaData>(meta_data);
			if(float_meta_data)
			{
				std::cout << "  Min:" << float_meta_data->GetMin() << " Max:" << float_meta_data->GetMax() << "\n";
			}
			GASS::EnumerationPropertyMetaDataPtr enum_meta_data = GASS_DYNAMIC_PTR_CAST<GASS::EnumerationPropertyMetaData>(meta_data);
			if(enum_meta_data)
			{
				std::cout << "  Possible enumeration values:";
				std::vector<std::string> values = enum_meta_data->GetEnumeration(bro);
				for(size_t j = 0; j< values.size(); j++)
				{
					std::cout << "," << values[j];
				}
				std::cout << std::endl;
			}
		}
	}
}



int main(int /*argc*/, char** /* argv[] */)
{
	GASS_SHARED_PTR<MyCar> my_car(new MyCar());
	my_car->SetDescription("My first car");
	my_car->SetPosition(GASS::Vec3(100.0, 0.0, 0.0));

	PrintProperties(my_car);

	std::cout << std::endl << "List property types" << std::endl;
	PrintPropertyTypes(my_car);

	//Change gear by value
	std::cout << std::endl << "Change gears by value..." << std::endl;

	GASS_ANY any_value;
	my_car->GetPropertyByType("GearBox",any_value);
	MyGearBox gb_value = GASS_ANY_CAST<MyGearBox>(any_value);
	gb_value.Gears++;
	my_car->SetPropertyByType("GearBox",gb_value);



	PrintPropertyTypes(my_car);

	//Change gearbox settings by string, first argument is gears, second automatic bool (0 or 1)
	std::cout << std::endl << "Change gearbox by string..." << std::endl;
	my_car->SetPropertyByString("GearBox","10 1");
	PrintPropertyTypes(my_car);


	//my_car->SetPropertyByString("TagPoints","1 1 1 2 2 2 3 3 3");
	//std::string test;
	///my_car->GetPropertyByString("TagPoints",test);


	//test vector data write (by string)
	//my_car->SetPropertyByString("Wheels","LFront 1.0 0.5 RFront 1.0 0.7");
	//PrintProperties(my_car);

	GASS_SHARED_PTR<MyDerivedCar> my_derived_car(new MyDerivedCar());

	std::cout << "\nShow some metadata...\n";
	PrintPropertyMetaData(my_derived_car);

	std::cout << "\nChange color enumeration for this instance...\n";
	std::vector<std::string> colors;
	colors.push_back("white");
	colors.push_back("black");
	my_derived_car->SetColorEnumeration(colors);

	PrintPropertyMetaData(my_derived_car);

	std::cout << "\nShow that we include base class properties...\n";
	PrintProperties(my_derived_car);
	_getch();
	return 0;
}


