#include "../include/sblend/Application.h"
#include <iostream>
#include <filesystem>

int main()
{
  


    sx::Application* application;
    try
    {
       application = sx::Application::getApplication();
       application->run();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    
   sx::Application::terminateApplication(application);
}