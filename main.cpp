// Standard includes
#include <cstdlib>
#include <iostream>

// Library includes
#include <jsoncpp/json/json.h>

// Project includes
#include "DisplayWindow.h"

int main(int argc, char** argv)
{
  try
  {
    // Read settings from config file
    Json::Value json;
    std::ifstream configFile("config.json", std::ifstream::binary);
    configFile >> json;

    // Run OpenGL window
    DisplayWindow mainWindow(json);
    mainWindow.Init();
    mainWindow.MainLoop();
  }
  catch(std::exception &exc)
  {
    std::cout << "Program failed. Exception: " << exc.what() << std::endl;
  }
  catch(...)
  {
    std::cout << "Program failed. Exception: Unknown exception" << std::endl;
  }

  return (EXIT_SUCCESS);
}

