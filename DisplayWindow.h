#ifndef _DISPLAYWINDOW
#define	_DISPLAYWINDOW

// Standard includes
#include <stdint.h>
#include <fstream>

// Library includes
#include <jsoncpp/json/json.h>

// Project includes
#include "Interfaces/IDisplay.h"
#include "Trees/Quadtree.h"
#include "Models/NBody.h"
#include "Interfaces/IIntegrator.h"

class DisplayWindow : public IDisplay
{
public:

    DisplayWindow(Json::Value config);
    virtual void Render();
    virtual void OnProcessEvents(uint8_t type);
    void Init();

private:

    DisplayWindow(const DisplayWindow& orig);
    void DrawParticles();
    void ShowStatisticsConsole();
    void DrawTree();
    void DrawTreeNode(Quadtree *treeNode, int level);

    NBody *model;
    IIntegrator *integrator;
    Json::Value configuration;

    int cameraSettings;
    bool showAxis;
    bool showParticles;
    bool showStatistics;
    bool showForceTree;
    bool showCompleteTree;
    bool isSimulationPaused;
    
};

#endif