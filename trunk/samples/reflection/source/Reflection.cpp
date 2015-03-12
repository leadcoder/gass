/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/										*
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
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
int _getch( ) {
	struct termios oldt,
		newt;
	int            ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
}
#endif

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

	}

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
private:
	GASS::Vec3 m_Pos;
	GASS::Quaternion m_Rot;
	std::string m_Description;
	MyGearBox m_GearBox;
	MyVector<MyWheel> m_Wheels;
};

enum EngineType
{
	ET_V12,
	ET_V8,
	ET_V6
};

using namespace GASS;
START_ENUM_BINDER(EngineType, EngineTypeBinder)
	BIND(ET_V12)
	BIND(ET_V8)
	BIND(ET_V6)
	END_ENUM_BINDER(EngineType, EngineTypeBinder)

class MyDerivedCar;
//Use custom meta data for color info



class MyDerivedCar : public GASS::Reflection<MyDerivedCar, MyCar>
{
public:
	//Custom meta data example
	class MyColorPropertyMetaData : public GASS::EnumerationPropertyMetaData
	{
	public:
		MyColorPropertyMetaData(const std::string &annotation, PropertyFlags flags): EnumerationPropertyMetaData(annotation,flags,false)
		{

		}
		virtual std::vector<std::string> GetEnumeration(BaseReflectionObjectPtr object) const 
		{
			std::vector<std::string> content;// = gfx_system->GetMaterialNames(m_ResourceGroup);
			boost::shared_ptr<MyDerivedCar> car = DYNAMIC_PTR_CAST<MyDerivedCar>(object);
			if(car) //get enumeration from unique instance 
				content = car->GetColorEnumeration();
			else //if some one else use this class we just add some static colors
			{
				content.push_back("Blue");
				content.push_back("Red");
				content.push_back("Green");
			}
			return content;
		}
	private:
	};
	typedef SPTR<MyColorPropertyMetaData> MyCustomPropertyMetaDataPtr;

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
			100, //min value
			200)); //min value

		//register our attributes to the RTTI system
		RegisterProperty<float>("EnginePower", &MyDerivedCar::GetEnginePower, &MyDerivedCar::SetEnginePower,ep_meta_data);

		//Create some meta data for the EngineType property , 
		GASS::BasePropertyMetaDataPtr et_meta_data(new GASS::EnumerationProxyPropertyMetaData(
			"Engine type",//annotation
			GASS::PF_VISIBLE, //editor flags
			&EngineTypeBinder::GetStringEnumeration)); //all enumeration values

		RegisterProperty<EngineTypeBinder>("EngineType", &MyDerivedCar::GetEngineType, &MyDerivedCar::SetEngineType,et_meta_data);

		//Create some meta data for the Color property that give us possiblity to have instance based enumeration, 
		GASS::BasePropertyMetaDataPtr color_meta_data(new MyColorPropertyMetaData(
			"Color name",//annotation
			GASS::PF_VISIBLE)); 
		RegisterProperty<std::string>("Color", &MyDerivedCar::GetColor, &MyDerivedCar::SetColor,color_meta_data);

	}
	float GetEnginePower()const {return m_EnginePower;}
	void SetEnginePower(const float value){m_EnginePower = value;}
	EngineTypeBinder GetEngineType()const {return m_EngineType;}
	void SetEngineType(const EngineTypeBinder value){m_EngineType = value;}
	std::string GetColor() const {return m_Color;}
	void SetColor(const std::string &value) {m_Color = value;}

	//used for instance based color enumeration 
	std::vector<std::string> GetColorEnumeration() const 
	{
		return m_ColorEnums;
	}
	void SetColorEnumeration(std::vector<std::string> value) {m_ColorEnums = value;}
private:
	float m_EnginePower;
	EngineTypeBinder m_EngineType;
	std::vector<std::string> m_ColorEnums;
	std::string m_Color;

};

void PrintProperties(GASS::BaseReflectionObjectPtr bro)
{
	//Use RTTI functionality to get properties for MyCar
	GASS::PropertyVector props = bro->GetProperties();
	std::cout << "List all properties for BaseReflectionObject derived object with classname: " << bro->GetRTTI()->GetClassName() << std::endl;
	for(size_t i = 0;  i < props.size(); i++)
	{
		const std::string prop_name = props[i]->GetName();
		const std::string prop_value = props[i]->GetValueAsString(bro.get());
		//You can also get attribute values like this
		//std::string prop_value; my_car->GetPropertyByString(prop_name,prop_value);
		std::cout << "PropertyName:" << prop_name << " Value:" << prop_value << std::endl;
	}
}

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
			GASS::BasePropertyMetaDataPtr meta_data = DYNAMIC_PTR_CAST<GASS::BasePropertyMetaData>(props[i]->GetMetaData());
			std::cout << "Property:" << prop_name  << "  Annotation:\"" << meta_data->GetAnnotation() << "\"";
			GASS::FloatMaxMinPropertyMetaDataPtr float_meta_data = DYNAMIC_PTR_CAST<GASS::FloatMaxMinPropertyMetaData>(meta_data);
			if(float_meta_data)
			{
				std::cout << "  Min:" << float_meta_data->GetMin() << " Max:" << float_meta_data->GetMax() << "\n";
			}
			GASS::EnumerationPropertyMetaDataPtr enum_meta_data = DYNAMIC_PTR_CAST<GASS::EnumerationPropertyMetaData>(meta_data);
			if(enum_meta_data)
			{
				std::cout << "  Possible enumeration values:"; 

				std::vector<std::string> values = enum_meta_data->GetEnumeration(bro);
				for(size_t i = 0; i < values.size();i++)
				{
					std::cout << " " << values[i];
				}
				std::cout << std::endl;
			}
		}
	}
}


void PrintPropertyTypes(GASS::BaseReflectionObjectPtr bro)
{
	GASS::PropertyVector props = bro->GetProperties();
	//Check property types
	for(size_t i = 0;  i < props.size(); i++)
	{
		const std::string prop_name = props[i]->GetName();
		boost::any any_value;
		//my_car.GetPropertyByType("Size",any_value);
		if(*props[i]->GetTypeID() == typeid(GASS::Vec3))
		{
			props[i]->GetValue(bro.get(), any_value);
			GASS::Vec3 vec3_value = boost::any_cast<GASS::Vec3>(any_value);
			std::cout << "Property " << prop_name << " is Vec3 and has value:" << vec3_value << "\n";
		}
		else if(*props[i]->GetTypeID() == typeid(GASS::Quaternion))
		{
			props[i]->GetValue(bro.get(), any_value);
			GASS::Quaternion quaternion_value = boost::any_cast<GASS::Quaternion>(any_value);
			std::cout << "Property " << prop_name << " is Quaternion and has value:" << quaternion_value << "\n";
		}
		else if(*props[i]->GetTypeID() == typeid(std::string))
		{
			std::cout << "Property " << prop_name << " is std::string and has value:" << props[i]->GetValueAsString(bro.get()) << "\n";
		}
		else if(*props[i]->GetTypeID() == typeid(MyGearBox))
		{
			props[i]->GetValue(bro.get(), any_value);
			MyGearBox gb_value = boost::any_cast<MyGearBox>(any_value);
			std::cout << "Property " << prop_name << " is GearBox, Numbers of gears:" << gb_value.Gears;
			if(gb_value.Automatic)
				std::cout << " Automatic mode\n";
			else
				std::cout << " Manual mode\n";
		}
		else if(*props[i]->GetTypeID() == typeid(MyVector<MyWheel>))
		{
			props[i]->GetValue(bro.get(), any_value);
			MyVector<MyWheel> wheels = boost::any_cast<MyVector<MyWheel> >(any_value);
			std::cout << "Property " << prop_name << " is Wheel vector holding:" << wheels.size() << " wheels\n";
			for(size_t i = 0; i< wheels.size();i++)
			{
				std::cout << "\t" << wheels[i].Name << " Radius:" << wheels[i].Radius << " Slip:" << wheels[i].Slip << "\n";
			}
		}
	}
}

int main(int argc, char* argv[])
{
	boost::shared_ptr<MyCar> my_car(new MyCar());
	my_car->SetDescription("My first car");
	my_car->SetPosition(GASS::Vec3(100.0, 0.0, 0.0));

	PrintProperties(my_car);

	std::cout << std::endl << "List property types" << std::endl;
	PrintPropertyTypes(my_car);

	//Change gear by value
	std::cout << std::endl << "Change gears by value..." << std::endl;

	boost::any any_value;
	my_car->GetPropertyByType("GearBox",any_value);
	MyGearBox gb_value = boost::any_cast<MyGearBox>(any_value);
	gb_value.Gears++;
	my_car->SetPropertyByType("GearBox",gb_value);
	PrintPropertyTypes(my_car);

	//Change gearbox settings by string, first argument is gears, second automatic bool (0 or 1)
	std::cout << std::endl << "Change gearbox by string..." << std::endl;
	my_car->SetPropertyByString("GearBox","10 1");
	PrintPropertyTypes(my_car);

	//test vector data write
	//my_car->SetPropertyByString("Wheels","LFront 1.0 0.5 RFront 1.0 0.7");
	//PrintProperties(my_car);

	boost::shared_ptr<MyDerivedCar> my_derived_car(new MyDerivedCar());

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


	getch();
	return 0;
}


