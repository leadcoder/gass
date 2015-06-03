/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/										*
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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

#include "Core/RTC/GASSTBBManager.h"
#include "Core/RTC/GASSTaskNode.h"
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
enum 
{
	PHYSICS_SYSTEM = 1,
	PHYSICS_OBJECT = 2,
	GFX_SYSTEM = 3,
};
/*
class PhysicsSystem : public GASS::ITaskNode2Listener
{
public:
	class PhysicsObject : public GASS::ITaskNode2Listener, public boost::enable_shared_from_this<PhysicsObject>
	{
	public:
		PhysicsObject() : m_UpdateCount(0){}
		
		void Update(double delta_time, GASS::TaskNode2* caller)
		{
			std::cout << "PhysicsObject update, time:" << delta_time << "\n";

			//test unregister while updating
			if(m_UpdateCount == 4)
				caller->Unregister(shared_from_this());

			m_UpdateCount++;
		}
		int m_UpdateCount;
	};

	PhysicsSystem()
	{

	}

	void Init(GASS::TaskNode2Ptr physics_node)
	{
		//create child node to objects in parallel
		GASS::TaskNode2Ptr obj_node(new GASS::TaskNode2(physics_node->GetRuntimeController(),PHYSICS_OBJECT));
		obj_node->SetListenerUpdateMode(GASS::TaskNode2::PARALLEL);
		physics_node->AddChildNode(obj_node);
		for(size_t i = 0; i < 20; i++)
		{
			boost::shared_ptr<PhysicsObject> po(new PhysicsObject());
			obj_node->Register(po);
			m_Objects.push_back(po);
		}
	}

	void Update(double delta_time, GASS::TaskNode2* caller)
	{
		std::cout << "PhysicsSystem update, time:" << delta_time << "\n";
	}
	std::vector <boost::shared_ptr<PhysicsObject>> m_Objects;
};

class GFXSystem : public GASS::ITaskNode2Listener, public boost::enable_shared_from_this<GFXSystem>
{
public:
	GFXSystem()
	{

	}

	void Update(double delta_time, GASS::TaskNode2* caller)
	{
		std::cout << "GFXSystem update, time:" << delta_time << "\n";
	}
};*/

class GameObject : public GASS::ITaskNodeListener, public boost::enable_shared_from_this<GameObject>
{
public:
	GameObject() : m_UpdateCount(0){}

	void Update(double delta_time, GASS::TaskNode* caller)
	{
		if(caller->GetID() == PHYSICS_SYSTEM)
		{
			UpdatePhysics(delta_time);
		}
		else if(caller->GetID() == GFX_SYSTEM)
		{
			UpdateGFX(delta_time);
		}
		//std::cout << "GameObject update, time:" << delta_time << "\n";
		//m_UpdateCount++;
	}
	
	int _Factorial(int n)
	{
		int factorial;
		for (int i = 0; i <= n; i++)

		if (i == 0)
			factorial = 1;
		else
			factorial = factorial * i;
		return factorial;
	}

	void UpdatePhysics(double delta_time)
	{
		//fake some heavy work
		int pretendWeNeedTheResult = 0;
		const size_t inter  = 10000000;
		for(size_t i = 0; i < inter ; i++)
		{
			volatile int n = 30;
			pretendWeNeedTheResult += _Factorial(n);
			pretendWeNeedTheResult = pretendWeNeedTheResult/inter;
		}
		std::cout << "GameObject update physics, result:" << pretendWeNeedTheResult << "\n";
	}

	void UpdateGFX(double delta_time)
	{
		std::cout << "GameObject update gfx" << delta_time << "\n";
	}
	int m_UpdateCount;
};

int main(int argc, char* argv[])
{
	GASS::TBBManager *rtc = new GASS::TBBManager();
	
	//update in parallel, all children will be updated in parallel
	rtc->Init(-1);

	GASS::TaskNode2Ptr root_node(new GASS::TaskNode(0));
	root_node->SetUpdateFrequency(60.0);
	root_node->SetListenerUpdateMode(GASS::TaskNode::SEQUENCE);
	root_node->SetChildrenUpdateMode(GASS::TaskNode::SEQUENCE);
	
	GASS::TaskNode2Ptr physics_node(new GASS::TaskNode(PHYSICS_SYSTEM));
	physics_node->SetUpdateFrequency(60.0);
	physics_node->SetListenerUpdateMode(GASS::TaskNode::SEQUENCE);
	root_node->AddChildNode(physics_node);
	//boost::shared_ptr<PhysicsSystem> ps(new PhysicsSystem());
	//ps->Init(physics_node);
	//physics_node->Register(ps);

	GASS::TaskNode2Ptr gfx_node(new GASS::TaskNode(GFX_SYSTEM));
	gfx_node->SetUpdateFrequency(60.0);
	gfx_node->SetListenerUpdateMode(GASS::TaskNode::PARALLEL);
	root_node->AddChildNode(gfx_node);
	//boost::shared_ptr<GFXSystem> gs(new GFXSystem());
	//physics_node->Register(gs);
	std::vector<boost::shared_ptr<GameObject>> go_vec;
	for(size_t i = 0; i < 20; i++)
	{
		boost::shared_ptr<GameObject> go(new GameObject());
		gfx_node->Register(go);
		physics_node->Register(go);
		go_vec.push_back(go);
	}
	
	bool done = false;
	std::cout << "press space to step simulation, q to quit, p to pause physics simulation and t to toggle parallel/sequence updates...\n";
	while(!done)
	{
		//step simulation
		std::cout << "Press key to continue\n";
		char key = getch();

		if(key == 32) //check if space is pressed
		{
			root_node->Update(1.0/120.0,NULL);
			std::cout << "stepping...\n";
		}
		else if(key == 'q')
			done = true;
		else if(key == 'p')
		{
			physics_node->SetPaused(!physics_node->GetPaused());	
		}
		else if(key == 't')
		{
			//ps->m_Objects.clear();
			if(physics_node->GetListenerUpdateMode() == GASS::TaskNode::SEQUENCE)
			{
				std::cout << "PARALLEL\n";
				physics_node->SetListenerUpdateMode(GASS::TaskNode::PARALLEL);
			}
			else
			{
				std::cout << "SEQUENCE\n";
				physics_node->SetListenerUpdateMode(GASS::TaskNode::SEQUENCE);
			}
		}
		else if(key == 's') //run for 1 sec
		{
			root_node->Update(1.0,NULL);
		}
	}
	
	return 0;
}


