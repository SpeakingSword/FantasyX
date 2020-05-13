#include "clocker.h"

using namespace fx;


GLfloat Time::deltaTime = 0.0f;
clock_t Time::lastTime = 0;
clock_t Time::currentTime = 0;


void Time::UpdateClocker()
{
    currentTime = clock();
    deltaTime = (currentTime - lastTime) * 0.001f;
    lastTime = currentTime;
}
