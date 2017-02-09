// CoreTest.cpp : Defines the entry point for the console application.
//
#include <boost/bind.hpp>

#include "Core/ComponentSystem/IComponent.h"
#include "Core/Math/Vector.h"
#include "Core/Utils/Factory.h"
#include "Core/Utils/Log.h"
#include "Core/Serialize/Serialize.h"
#include "Core/Utils/Timer.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"

#include "Core/PluginSystem/PluginManager.h"

#include "Sim/SimEngine.h"
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/SceneObject.h"

#include <stdio.h>
#include <iostream>
#include <fstream>



GASS::MessageManager mm;

int main(int argc, char* argv[])
{
	GASS::SimEngine* engine = new GASS::SimEngine();
	engine->Init();
	GASS::Scenario* scenario = new GASS::Scenario();
	std::cout << "Load scenario" << std::endl;
	scenario->Load("../../../../data/scenarios/osg_scenario");
	GASS::Timer timer;

	timer.Reset();

	double prev = 0;
	double temp_t = 0;
	bool check_reset = true;
	while(timer.GetTime() < 2000000)
	{
		double time = timer.GetTime();
		engine->Update(time - temp_t);
		scenario->OnUpdate(time - temp_t);
		temp_t = time;
		if(time - prev > 1.0)
		{
			std::cout << "Time is:" << time << std::endl;
			prev = time;
		}
		/*if(check_reset && time > 5.0)
		{
			check_reset = false;
			timer.Reset();
			time = timer.GetTime();
			std::cout << "Reset timer after 5 sec , current time is:" << time << std::endl;
			prev = time;
		}*/
	}

	/////////////////TEST plugin manager/////////////

	
	return 0;
}




