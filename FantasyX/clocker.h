#ifndef CLOCKER_H
#define CLOCKER_H

#include <iostream>
#include <ctime>
#include <glad\glad.h>

namespace fx {

    class Time {
    public:
        static GLfloat deltaTime;
        static void UpdateClocker();
    private:
        static clock_t lastTime;
        static clock_t currentTime;
    };
}

#endif // !CLOCKER_H

