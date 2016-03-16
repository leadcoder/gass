

#ifndef SIM_ENGINE_I
#define SIM_ENGINE_I

%{
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/SimEngine.h"
%}

class SimEngine
{
public:
	SimEngine();
	~SimEngine();
	static SimEngine* GetPtr();
	static SimEngine& Get();
	SimSystemManagerPtr GetSimSystemManager();
};

#endif