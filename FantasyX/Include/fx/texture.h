#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <glad\glad.h>

using namespace std;

namespace fx {
    
    struct Texture
    {
        GLuint id;
        string type;
        string path;

        Texture()
        {
            id = 0;
            type = "null";
            path = "null";
        }
    };
    
}

#endif // !TEXTURE_H