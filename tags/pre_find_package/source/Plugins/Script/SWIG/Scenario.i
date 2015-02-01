

#ifndef SCENARIO_I
#define SCENARIO_I

%{
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Core/Utils/Iterators.h"
#include <vector>

%}
%template(ScenarioSceneVector) std::vector<ScenarioScene>;

    template <class T>
    class VectorIterator
    {
    public:
        VectorIterator();
    public:
        VectorIterator(typename T::iterator start, typename T::iterator end);
        explicit VectorIterator(T& c);
        bool hasMoreElements(void) const;
        typename T::value_type getNext(void);
        typename T::value_type peekNext(void);
        typename T::pointer peekNextPtr(void);
        void moveNext(void);
    };


%template(ScenarioSceneIterator) VectorIterator<ScenarioSceneVector>;

class Scenario
{
public:
	Scenario();
	~Scenario();
	ScenarioSceneIterator GetScenarioScenes();
};

#endif