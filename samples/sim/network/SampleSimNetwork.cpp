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


#include "Sim/GASS.h"
#include "Core/PluginSystem/GASSPluginManager.h"
#include "Sim/Messages/GASSPlatformMessages.h"
#include "SimApplication.h"
#include "Client.h"
#include "Server.h"

#include <stdio.h>
#include <iostream>

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

typedef GASS_SHARED_PTR<SimApplication> SimApplicationPtr;

int main(int argc, char* argv[])
{
	SimApplicationPtr app;

	std::cout << "Server or client?, Press [S] or [C]";
	char key = _getch();
	if(key == 'c' || key == 'C')
		app = SimApplicationPtr(new SimClient());
	else if(key == 's' || key == 'S')
		app = SimApplicationPtr(new SimServer());
	else
	{
		std::cout << "Please select server or client....exiting"; return 0;
	}
	try
	{
		app->Init();
	}
	catch (std::exception& e) {
		std::cout << "init exception:" << e.what();
	}

	try
	{
		while (app->Update()) {}
	}
	catch (std::exception& e) {
		std::cout << "run exception:" << e.what();
	}

	return 0;
}


