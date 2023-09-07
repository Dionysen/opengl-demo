
#include "MyScene.h"
#include <iostream>

int main()
{
    MyScene app;
    if (app.initGLContext(2, true))
    {
        std::cout << "Initialize application ...\n";
        app.initialize();
        std::cout << "Starting render loop ...\n";
        app.startLoop();
    }
}