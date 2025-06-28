#include "../include/sblend/Application.h"
#include <iostream>
#include <filesystem>

int main()
{
    sx::Application *application;
    bool appGoneWrong = false;
    try
    {
        application = sx::Application::getApplication();
        application->run();
    } catch (std::exception &e)
    {
        std::cerr << e.what() << '\n';
        appGoneWrong = true;
    }

    if (!appGoneWrong)
        sx::Application::terminateApplication(application);
}
