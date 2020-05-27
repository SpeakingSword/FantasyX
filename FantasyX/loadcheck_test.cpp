//#define LOADCHECK_TEST_H
#ifdef LOADCHECK_TEST_H

#define DEBUG

#include <iostream>
#include "context.h"
#include "resource_manager.h"
#include "texture.h"

using namespace fx;
using namespace std;

int main()
{
    fxWindow *window = new fxWindow();
    window->Init();
    ResourceManager *res = ResourceManager::GetInstance();

    // 测试加载纹理/贴图
    Texture awosomeface = res->LoadTexture2D("D:\\OpenGLAssets\\Images\\awesomeface.png", "_AlbedoMap", true);
    Texture awosomeface_2 = res->LoadTexture2D("D:\\OpenGLAssets\\Images\\awesomeface.png", "_AlbedoMap", true);
    Texture alyx_apt = res->LoadHdrTexture("D:\\OpenGLAssets\\Images\\HDR\\Alexs_Apartment\\Alexs_Apt_2k.hdr");
    Texture alyx_apt_2 = res->LoadHdrTexture("D:\\OpenGLAssets\\Images\\HDR\\Alexs_Apartment\\Alexs_Apt_2k.hdr");

    // 输出两个纹理
#ifdef DEBUG
    std::cout << "awosomeface: id = " << awosomeface.id << std::endl;
    std::cout << "awosomeface_2: id = " << awosomeface_2.id << std::endl;
    std::cout << "alyx_apt: id = " << alyx_apt.id << std::endl;
    std::cout << "alyx_apt_2: id = " << alyx_apt_2.id << std::endl;
#endif // DEBUG



    system("pause");

    return 0;
}

#endif // !LOADCHECK_TEST_H