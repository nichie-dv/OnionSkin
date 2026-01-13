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

        cocos2d::ccColor3B pastColor;  
        cocos2d::ccColor3B futureColor; 
        int maxLayerDistance;
        int maxOpacity;
        int minOpacity;

        bool renderPast;
        bool renderFuture;



        Fields():
            pastColor{255, 0, 0},
            futureColor{0, 0, 255},
            maxLayerDistance(5),
            maxOpacity(35),
            minOpacity(0),
            renderPast(true),
            renderFuture(true)
        {}
    };
};