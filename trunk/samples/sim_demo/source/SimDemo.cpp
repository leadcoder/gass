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


#include "Sim/GASS.h"
#include "Plugins/Game/GameMessages.h"
#include "SimApplication.h"

//#include "Client.h"
//#include "Server.h"

#include <stdio.h>
#include <iostream>
#include <fstream>

#ifdef WIN32
#include <conio.h>
#endif


#ifndef WIN32
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

typedef boost::shared_ptr<SimApplication> SimApplicationPtr;

int main(int argc, char* argv[])
{
	/*std::string config = "../Configuration/app_config.xml";
	int index = 1;
	while(index < argc)
	{
		char* arg = argv[index];
		if(strcmp(arg, "--Config") == 0)
		{
			config = argv[index+1];
		}
		index += 2;
	}*/
	/*
	std::cout << "Server, client or standalone? Press [S] ,[C] or [A]:";
	char key = _getch();
	int app_mode = 2;
	if(key == 'c' || key == 'C')
		app_mode = 0;
	else if(key == 's' || key == 'S')
		app_mode = 1;
	else if(key == 'a' || key == 'A')
		app_mode = 2;

	if(app_mode == 0)
		app = SimApplicationPtr(new SimClient(config));
	else if(app_mode == 1)
		app = SimApplicationPtr(new SimServer(config));
	else if(app_mode == 2)
		app = SimApplicationPtr(new SimApplication(config));*/

	SimApplicationPtr app;
	app = SimApplicationPtr(new SimApplication());
	app->Init();
	while(app->Update())
	{
		
	}
	return 0;
}


