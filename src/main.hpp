#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/SliderNode.hpp>

#include <utility>

using namespace geode::prelude;



class OSPSettingEntry : public CCMenu {
    public:
    static OSPSettingEntry* create(CCMenuItemSpriteExtra* button, std::string label);
    static OSPSettingEntry* create(SliderNode* slider);

    static OSPSettingEntry* createTitle(std::string label);
    static OSPSettingEntry* create(std::string label);
    
    void setInfo(std::string infoHeader, std::string infoLabel);
    void updateLabelScale();
    void updateMainNode(CCNode* node);

    protected:
    float xOffset = 0;
    CCNode* mainButton = nullptr;
    CCLabelBMFont* label = nullptr;
    CCMenuItemSpriteExtra* infoIcon = nullptr;
    
};

class OSPopup : public cocos2d::CCLayer {

    public:
    static OSPopup* create();
    cocos2d::CCMenu* popupExitMenu;
    cocos2d::CCMenu* popupMainMenu;

    CCMenuItemToggler* OStoggle = nullptr;
    CCMenuItemToggler* PBtoggle = nullptr;
    TextInput* inputNodeF = nullptr;
    TextInput* inputNodeE = nullptr;
    TextInput* inputNodeFPS = nullptr;
    CCMenuItemSpriteExtra* useCurrentButton = nullptr;

    CCMenuItemToggler* rpSetting = nullptr;
    CCMenuItemToggler* rfSetting = nullptr;

    TextInput* frameDepthInput = nullptr;
    TextInput* opacityMaxInput = nullptr;
    TextInput* opacityMinInput = nullptr;

    std::vector<int> frameRange;
    int pbFPS;


    protected:
    bool init() override;
    void keyBackClicked() override;
    void onBack(CCObject*);

    void toggleOnion(CCObject* sender);
    void togglePlayback(CCObject* sender);
    void onUseCurrent(CCObject* sender);
   
    void registerWithTouchDispatcher() override;
    ~OSPopup();

    int currentLayer = 1;

};

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


    //Keybind stuffs
    bool shiftModifier = false;
    bool playtesting = false;

    bool kbRegistered = false;
    
};



//HORRIBLE things are happening here.