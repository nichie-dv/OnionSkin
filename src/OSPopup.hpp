#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;


#include <Geode/ui/SliderNode.hpp>
class OSPSettingEntry : public CCMenu {
    public:
    static OSPSettingEntry* create(CCMenuItemSpriteExtra* button, std::string label);
    static OSPSettingEntry* create(geode::SliderNode* slider);

    static OSPSettingEntry* createTitle(std::string label);
    static OSPSettingEntry* create(std::string label);
    
    void setInfo(std::string infoHeader, std::string infoLabel);
    void updateLabelScale(); //Replace with limit
    void updateMainNode(CCNode* node);

    protected:
    bool init(CCNode* button, std::string labelText, std::string font);
    float xOffset = 0;
    CCNode* mainButton = nullptr;
    CCLabelBMFont* label = nullptr;
    CCMenuItemSpriteExtra* infoIcon = nullptr;
    
};





class OSPopup : public geode::Popup {

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


    int currentLayer = 1;

};