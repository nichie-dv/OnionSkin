#pragma once
#include <Geode/Geode.hpp>
#include <cocos2d.h>

class OnionSkin {
public:

    struct Fields {
        CCMenuItemToggler* layerToggle = nullptr;
        LevelEditorLayer* levelEditorLayer = nullptr;
        bool onionEnabled = false;
        int currentLayer = -1;

        cocos2d::ccColor3B pastColor = {255, 0, 0};  
        cocos2d::ccColor3B futureColor = {0, 0, 255}; 
        int maxLayerDistance = 5;
        int maxOpacity = 35;
        int minOpacity = 0;

        bool renderPast = true;
        bool renderFuture = true;

    };
};
// This is probably very wrong but it works