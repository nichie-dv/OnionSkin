#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/SliderNode.hpp>

#include <utility>

#include "OSPopup.hpp"

using namespace geode::prelude;

struct OnionSkin {
    CCMenuItemSpriteExtra* LayerToggle = nullptr;
    CCMenuItemToggler* toggler = nullptr;
    LevelEditorLayer* LevelEditorLayer = nullptr;

    CCSprite* layerToggleOff = nullptr;
    CCSprite* layerToggleOn = nullptr;

    

    OSPopup* settingsPopup = nullptr;

    bool onionEnabled = false;
    bool playbackEnabled = false;
    bool showTimestamp = false;

    CCLabelBMFont* timestampLabel = nullptr;
    CCLabelBMFont* frameCountLabel = nullptr;

    bool renderPast = true;
    bool renderFuture = true;
    
    bool changeColors = true;
    cocos2d::ccColor3B pastColor = {255, 0, 0};  
    cocos2d::ccColor3B futureColor = {0, 0, 255}; 
    int maxLayerDistance = 5;
    int maxOpacity = 35;
    int minOpacity = 0;
    

    int currentLayer = -1;

    //Playback settings
    std::pair<int, int> animBounds;
    int fps;

    double pbTimer = 0;
    int pbFrameNum = 0; //same as current layer in the animation


    bool playtesting = false;

    bool kbRegistered = false;
    
};


//HORRIBLE things are happening here.