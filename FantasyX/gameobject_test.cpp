//#define GAMEOBJECT_TEST
#ifdef GAMEOBJECT_TEST

#include <iostream>
#include "gameobject.h"

using namespace std;
using namespace fx;

void TestObjectTree();
void TestSingleton();

int main()
{
    TestObjectTree();
    system("pause");
}

void TestObjectTree()
{
    GameObject *man = new GameObject();
    man->name = "Man";
    man->tag = GT_OPAQUE;
    GameObject *man_head = new GameObject();
    man_head->name = "Head";
    man_head->tag = GT_OPAQUE;
    GameObject *man_hand = new GameObject();
    man_hand->name = "Hand";
    man_hand->tag = GT_OPAQUE;
    GameObject *man_hand_left = new GameObject();
    man_hand_left->name = "Hand_Left";
    man_hand_left->tag = GT_OPAQUE;
    GameObject *man_hand_right = new GameObject();
    man_hand_right->name = "Hand_Right";
    man_hand_right->tag = GT_OPAQUE;
    GameObject *left_eye = new GameObject();
    left_eye->name = "Left_Eye";
    left_eye->tag = GT_OPAQUE;
    GameObject *right_eye = new GameObject();
    right_eye->name = "Right_Eye";
    right_eye->tag = GT_OPAQUE;

    man->Add(man_hand);
    man->Add(man_head);
    man_hand->Add(man_hand_left);
    man_hand->Add(man_hand_right);
    man_head->Add(left_eye);
    man_head->Add(right_eye);

    std::string prefix = "-";
    GameObject::PrintAllName(man, prefix);
}

void TestSingleton()
{
    
}
#endif // SCENE_TEST
