#pragma once
#include <Geode/Geode.hpp>

struct OnionSkin {
    CCMenuItemToggler* LayerToggle = nullptr;
    LevelEditorLayer* LevelEditorLayer = nullptr;

    

    bool onionEnabled = false;
    bool renderPast = true;
    bool renderFuture = true;
    

    cocos2d::ccColor3B pastColor = {255, 0, 0};  
    cocos2d::ccColor3B futureColor = {0, 0, 255}; 
    int maxLayerDistance = 5;
    int maxOpacity = 35;
    int minOpacity = 0;
    

    int currentLayer = -1;

    bool shiftModifier = false;
    bool playtesting = false;
    
};
//HORRIBLE things are happening here.