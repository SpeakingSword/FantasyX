//#define COMPONENTSYSTEM_TEST
#ifdef COMPONENTSYSTEM_TEST

#include <iostream>
#include "gameobject.h"
#include "component.h"

using namespace std;
using namespace fx;

int main()
{
    GameObject *man = new GameObject("Man", GT_OPAQUE);
    man->AddComponent(new Transform());
    man->AddComponent(new Render());
    man->ShowAllComponent();
    
    man->AddComponent(new Render());
    man->AddComponent(new Transform());

    man->RemoveComponent("Light");
    man->RemoveComponent("Render");

    Component *trans = man->GetComponent("Transform");
    trans->Print();
    system("pause");
}

#endif // !COMPONENTSYSTEM_TEST
