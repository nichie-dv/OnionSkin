#include <Geode/Geode.hpp>
#include <Geode/loader/SettingV3.hpp>
#include "main.hpp"

//These values get overwritten the first time theyre tweaked by the user
#define PLAYBACK_END 15
#define PLAYBACK_SPEED 10
#define MAX_FRAME_DEPTH 50

using namespace geode::prelude;



static OnionSkin* onionSkin = nullptr;



//Setting Entry
OSPSettingEntry* OSPSettingEntry::create(CCMenuItemSpriteExtra* button, std::string labelText) {
    auto ret = new OSPSettingEntry();
    if (ret->init()) {
        ret->autorelease();
        
        ret->mainButton = button;

        ret->setLayout(AxisLayout::create()
            ->setAutoGrowAxis(false)
            ->setAutoScale(false)
            ->setAxisAlignment(AxisAlignment::Start)
        );
        ret->setPosition({0, 0});

        ret->addChild(ret->mainButton);
        ret->label = CCLabelBMFont::create(labelText.c_str(), "bigFont.fnt");

        ret->addChild(ret->label);
        ret->updateLabelScale();



        ret->mainButton->setID("button");
        ret->label->setID("label");

        return ret;
    }
    delete ret;
    return nullptr;
}

#include <Geode/ui/SliderNode.hpp>
OSPSettingEntry* OSPSettingEntry::create(geode::SliderNode* slider) {
    auto ret = new OSPSettingEntry();
    if (ret->init()) {
        ret->autorelease();
        ret->xOffset = 10;
        
        slider->setScale(0.7);
        ret->mainButton = slider;

        ret->setLayout(AxisLayout::create()
            ->setAutoGrowAxis(false)
            ->setAutoScale(false)
            ->setAxisAlignment(AxisAlignment::Start)
        );
        ret->setPosition({0, 0});

        ret->addChild(ret->mainButton);
        ret->updateLabelScale();
        


        ret->mainButton->setID("button");

        return ret;
    }
    delete ret;
    return nullptr;
}

OSPSettingEntry* OSPSettingEntry::createTitle(std::string labelText) {
    auto ret = new OSPSettingEntry();
    if (ret->init()) {
        ret->autorelease();

        ret->setLayout(AxisLayout::create()
            ->setAutoGrowAxis(false)
            ->setAutoScale(false)
            ->setAxisAlignment(AxisAlignment::Start)
        );
        ret->setPosition({0, 0});
        ret->label = CCLabelBMFont::create(labelText.c_str(), "goldFont.fnt");
        ret->label->setScale(190 / (ret->label->getContentWidth()  / ret->label->getScale()));

        ret->addChild(ret->label);
        ret->updateLayout();
        //ret->updateLabelScale();

        ret->label->setID("label");

        return ret;
    }
    delete ret;
    return nullptr;
}

OSPSettingEntry* OSPSettingEntry::create(std::string labelText) {
    auto ret = new OSPSettingEntry();
    if (ret->init()) {
        ret->autorelease();

        ret->setLayout(AxisLayout::create()
            ->setAutoGrowAxis(false)
            ->setAutoScale(false)
            ->setAxisAlignment(AxisAlignment::Start)
        );
        ret->setPosition({0, 0});
        ret->label = CCLabelBMFont::create(labelText.c_str(), "bigFont.fnt");
        ret->label->setScale(0.75);
        ret->label->setScale(190 / (ret->label->getContentWidth()  / ret->label->getScale()));

        ret->addChild(ret->label);
        ret->updateLayout();
        //ret->updateLabelScale();

        ret->label->setID("label");

        return ret;
    }
    delete ret;
    return nullptr;
}



void OSPSettingEntry::updateLabelScale() {
    float totalWidth = 190;
    float gap = 5;

    float usedWidth = 0;

    if (mainButton) {
        usedWidth += mainButton->getContentWidth() / mainButton->getScale();
        usedWidth += gap;
    }
        


    if (infoIcon != nullptr) {
        usedWidth += infoIcon->getContentWidth() / infoIcon->getScale();
        usedWidth += gap;
    }

    float availableWidth = totalWidth - usedWidth - gap;
    if (label != nullptr) {
        float labelWidth = label->getContentWidth() / label->getScale();

        if (labelWidth > availableWidth) {
            float scale = std::max(0.3f, availableWidth / labelWidth);
            label->setScale(scale);
        } else {
            label->setScale(1);
        }
    }
    
    this->updateLayout();

    if (mainButton != nullptr) mainButton->setPositionX(mainButton->getPositionX() + xOffset);
    if (label != nullptr) label->setPositionX(label->getPositionX() + xOffset);
    if (infoIcon != nullptr) infoIcon->setPositionX(infoIcon->getPositionX() + xOffset);
}

void OSPSettingEntry::updateMainNode(CCNode* node) {
    mainButton = node;
}

void OSPSettingEntry::setInfo(std::string infoHeader, std::string infoLabel) {
    auto sprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    sprite->setScale(0.75);
    this->infoIcon = CCMenuItemExt::createSpriteExtra(
        sprite,
        [infoHeader, infoLabel](CCObject*) {
            FLAlertLayer::create(
                infoHeader.c_str(),
                infoLabel,
                "OK"
            )->show();
        }
    );
    
    this->addChild(this->infoIcon);
    this->updateLabelScale();
}





//Popup
void OSPopup::toggleOnion(CCObject* sender) {
    onionSkin->onionEnabled = !onionSkin->onionEnabled;
    onionSkin->toggler->toggle(onionSkin->onionEnabled);
}

void OSPopup::togglePlayback(CCObject* sender) {
    onionSkin->playbackEnabled = !onionSkin->playbackEnabled;
}

void OSPopup::onUseCurrent(CCObject* sender) {
    int val2 = frameRange[0];
    if (currentLayer <= val2) {
        currentLayer = val2 + 1;
    }
    this->frameRange[1] = currentLayer;
    if (!inputNodeE->getString().empty())
        inputNodeE->setString(numToString<int>(currentLayer));
    else
        inputNodeE->setPlaceholder(numToString<int>(currentLayer));
    Mod::get()->setSavedValue<std::vector<int>>("playback-range", frameRange);
}

bool OSPopup::init() {
    if (!CCLayer::init()) return false;


    frameRange = Mod::get()->getSavedValue<std::vector<int>>("playback-range", {0, PLAYBACK_END});
    pbFPS = Mod::get()->getSavedValue<int>("playback-speed", PLAYBACK_SPEED);

    Mod::get()->setSavedValue<std::vector<int>>("playback-range", {frameRange[0], frameRange[1]});
    Mod::get()->setSavedValue<int>("playback-speed", pbFPS);
    

    CCTouchDispatcher::get()->registerForcePrio(this, 2);
    auto winSize = CCDirector::get()->getWinSize();
    this->setID("onion-skin-menu"_spr);  
    
    this->setZOrder(100);
    auto fadeLayer = CCSprite::create("square.png");
    fadeLayer->setScaleX(winSize.width / 2);
    fadeLayer->setScaleY(winSize.height / 2);
    fadeLayer->setColor({0, 0, 0});
    fadeLayer->setOpacity(0);
    fadeLayer->setZOrder(1);
    fadeLayer->setID("fade-node");

    auto bgFadeIn = CCFadeTo::create(0.1f, 150);
    auto bgSeq = CCSequence::create(
        bgFadeIn,
        nullptr
    );

    fadeLayer->runAction(bgSeq);
    this->addChildAtPosition(fadeLayer, Anchor::Center);

    auto popupBase = NineSlice::create("GJ_square01.png");
    popupBase->setID("popup-base");
    popupBase->setContentSize({380, 260});
    popupBase->setScale(0);

    auto ppScaleIn = CCScaleTo::create(0.5f, 1);
    auto eOut = CCEaseElasticOut::create(ppScaleIn, 0.6f);

    auto ppSeq = CCSequence::create(
        eOut,
        nullptr
    );

    popupBase->runAction(ppSeq);
    popupBase->setZOrder(5);

    this->addChildAtPosition(popupBase, Anchor::Center);

    popupExitMenu = CCMenu::create();
    popupExitMenu->setZOrder(100);
    popupExitMenu->setID("exit-button-menu");
    
    

    auto exitButtonComp = CCNode::create();
    auto exitSpriteBase = CCSprite::createWithSpriteFrameName("geode.loader/baseCircle_BigAlt_Green.png");
    auto exitSpriteIcon = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
    
    auto center = ccp(
        exitSpriteBase->getContentSize().width / 2,
        exitSpriteBase->getContentSize().height / 2
    );

    popupBase->addChildAtPosition(popupExitMenu, Anchor::Center);


    exitSpriteBase->setPosition(center);
    exitSpriteIcon->setPosition(center);
    exitSpriteIcon->setScale(1.15f);

    exitButtonComp->addChild(exitSpriteBase);
    exitButtonComp->addChild(exitSpriteIcon);
    
    exitButtonComp->setScale(0.85f);

    auto exitBtn = CCMenuItemExt::createSpriteExtra(exitButtonComp, [this](auto btn) {
        this->onBack(btn);
    });
    exitBtn->setContentSize(ccp(
        exitSpriteBase->getContentWidth() * 0.85f,
        exitSpriteBase->getContentHeight() * 0.85f
    ));
    exitBtn->setAnchorPoint((ccp(0.5f, 0.5f)));
    popupExitMenu->setContentWidth(popupBase->getContentWidth() + (exitSpriteBase->getContentWidth() / 2));
    popupExitMenu->setContentHeight(popupBase->getContentHeight() + (exitSpriteBase->getContentHeight() / 2));
    
    popupExitMenu->addChildAtPosition(exitBtn, Anchor::TopLeft, ccp(15, -15));
    


    
    popupMainMenu = CCMenu::create();
    popupMainMenu->setID("main-toggle-menu");
    popupMainMenu->setContentSize({40, 240});
    popupMainMenu->ignoreAnchorPointForPosition(false);

    popupMainMenu->setLayout(AxisLayout::create(Axis::Column)->setGap(25)->setAxisReverse(true));

    popupBase->addChildAtPosition(popupMainMenu, Anchor::Left, {25, -15});

    //Enable Check
    {   
        auto itemNode = CCNode::create();
        itemNode->setContentSize({30, 30});
        itemNode->setAnchorPoint({0.5, 0.5});  

        auto toggleMenu = CCMenu::create();
        toggleMenu->setContentSize({30, 30});
        toggleMenu->setAnchorPoint({0, 0});  
        toggleMenu->ignoreAnchorPointForPosition(false);


        auto offSpr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        auto onSpr = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"); 


        OStoggle = CCMenuItemToggler::create(
            offSpr,
            onSpr,
            this,
            menu_selector(OSPopup::toggleOnion)
        );
        
        OStoggle->setContentSize({30, 30});
        OStoggle->toggle(onionSkin->onionEnabled);
        toggleMenu->addChild(OStoggle);
        itemNode->addChildAtPosition(toggleMenu, Anchor::Center);

        auto toggleLabel = CCLabelBMFont::create("enable", "bigFont.fnt");
        toggleLabel->setScale(0.5);
        itemNode->addChildAtPosition(toggleLabel, Anchor::Top, {toggleLabel->getContentWidth() / 2, 11});
        toggleLabel->setPositionX(toggleLabel->getContentWidth() / 4);
        popupMainMenu->addChild(itemNode);
    }

    //Playback Check
    {   
        auto itemNode = CCNode::create();
        itemNode->setContentSize({30, 30});
        itemNode->setAnchorPoint({0.5, 0.5});  

        auto toggleMenu = CCMenu::create();
        toggleMenu->setContentSize({30, 30});
        toggleMenu->setAnchorPoint({0, 0});  
        toggleMenu->ignoreAnchorPointForPosition(false);


        auto offSpr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        auto onSpr = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"); 


        PBtoggle = CCMenuItemToggler::create(
            offSpr,
            onSpr,
            this,
            menu_selector(OSPopup::togglePlayback)
        );
        
        PBtoggle->setContentSize({30, 30});
        PBtoggle->toggle(onionSkin->playbackEnabled);
        toggleMenu->addChild(PBtoggle);
        itemNode->addChildAtPosition(toggleMenu, Anchor::Center);

        auto toggleLabel = CCLabelBMFont::create("playback", "bigFont.fnt");
        toggleLabel->setScale(0.5);
        itemNode->addChildAtPosition(toggleLabel, Anchor::Top, {toggleLabel->getContentWidth() / 2, 11});
        toggleLabel->setPositionX(toggleLabel->getContentWidth() / 4);
        popupMainMenu->addChild(itemNode);
    }

    //Playback Range
    {   
        auto itemNode = CCNode::create();
        itemNode->setContentSize({30, 30});
        itemNode->setAnchorPoint({0.5, 0.5});  

        auto textMenu = CCMenu::create();
        textMenu->setContentSize({30, 30});
        textMenu->setAnchorPoint({0, 0.5});  
        textMenu->ignoreAnchorPointForPosition(false);
        textMenu->setLayout(AxisLayout::create()->setAutoGrowAxis(true)->setGap(10));


        auto offSpr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        auto onSpr = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"); 

        inputNodeF = TextInput::create(30, numToString<int>(frameRange[0]));
        inputNodeF->setLabel("start");
        inputNodeF->setFilter("0123456789");
        inputNodeF->setCallback([this](const std::string& value){ 
            int val = numFromString<int>(value).unwrapOr(0);
            frameRange[0] = val;
            Mod::get()->setSavedValue<std::vector<int>>("playback-range", frameRange);
        
        });

        inputNodeE = TextInput::create(30, numToString<int>(frameRange[1]));
        inputNodeE->setLabel("end");
        inputNodeE->setFilter("0123456789");
        inputNodeE->setCallback([this](const std::string& value){ 
            int val = numFromString<int>(value).unwrapOr(PLAYBACK_END);
            frameRange[1] = val;
            Mod::get()->setSavedValue<std::vector<int>>("playback-range", frameRange);
        
        });

        auto useCurrentNode = NineSlice::create("GJ_button_01.png");
        useCurrentNode->setContentSize({55, 30});

        auto useCurrentLabel = CCLabelBMFont::create("current", "bigFont.fnt");
        useCurrentLabel->setZOrder(10);
        useCurrentLabel->setScale(0.35);

        useCurrentNode->addChildAtPosition(useCurrentLabel, Anchor::Center);
        currentLayer = onionSkin->currentLayer;

        

        useCurrentButton = CCMenuItemSpriteExtra::create(
            useCurrentNode,
            this,
            menu_selector(OSPopup::onUseCurrent)
        );

        textMenu->addChild(inputNodeF);
        textMenu->addChild(inputNodeE);
        textMenu->addChild(useCurrentButton);
        textMenu->updateLayout();
        
        itemNode->addChildAtPosition(textMenu, Anchor::Center, {-15, 0});

        auto toggleLabel = CCLabelBMFont::create("playback range", "bigFont.fnt");
        toggleLabel->setScale(0.5);
        itemNode->addChildAtPosition(toggleLabel, Anchor::Top, {toggleLabel->getContentWidth() / 2, 17});
        toggleLabel->setPositionX(toggleLabel->getContentWidth() / 4);
        popupMainMenu->addChild(itemNode);
    }

    //Playback Speed
    {   
        auto itemNode = CCNode::create();
        itemNode->setContentSize({30, 30});
        itemNode->setAnchorPoint({0.5, 0.5});  

        auto textMenu = CCMenu::create();
        textMenu->setContentSize({30, 30});
        textMenu->setAnchorPoint({0.5, 0.5});  
        textMenu->ignoreAnchorPointForPosition(false);
        textMenu->setLayout(AxisLayout::create()->setAutoGrowAxis(true));


        auto offSpr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        auto onSpr = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"); 

        inputNodeFPS = TextInput::create(30, numToString<int>(pbFPS));
        inputNodeFPS->setFilter("0123456789");
        inputNodeFPS->setCallback([this](const std::string& value){ 
            int val = numFromString<int>(value).unwrapOr(PLAYBACK_SPEED); 
            val = std::clamp(val, 1, PLAYBACK_SPEED * 8);
            this->pbFPS = val;
            if (!inputNodeFPS->getString().empty())
                this->inputNodeFPS->setString(numToString<int>(val));
            else
                this->inputNodeFPS->setPlaceholder(numToString<int>(val));
            Mod::get()->setSavedValue<int>("playback-speed", val);
            
        });

        

        textMenu->addChild(inputNodeFPS);
        textMenu->updateLayout();
        
        
        itemNode->addChildAtPosition(textMenu, Anchor::Center);
        
        auto frameProg = CCLabelBMFont::create("0/0", "chatFont.fnt");

        auto toggleLabel = CCLabelBMFont::create("fps", "bigFont.fnt");
        toggleLabel->setScale(0.5);
        itemNode->addChildAtPosition(toggleLabel, Anchor::Top, {toggleLabel->getContentWidth() / 2, 11});
        toggleLabel->setPositionX(toggleLabel->getContentWidth() / 4);
        popupMainMenu->addChild(itemNode);
    }

    auto scrollBG = NineSlice::create("square02_001.png");
    scrollBG->setID("scroll-bg");
    scrollBG->setContentSize({200, 220});
    scrollBG->setOpacity(100);

    auto scrollLayer = ScrollLayer::create({190, 219});
    scrollLayer->setZOrder(10);
    scrollLayer->setAnchorPoint({0.5f, 0.5f});
    

    auto scrollBar = Scrollbar::create(scrollLayer);
    scrollBar->setID("scroll-bar");

    auto menu = CCMenu::create();
    menu->setLayout(AxisLayout::create(Axis::Column)
        ->setAutoGrowAxis(false)
        ->setAutoScale(true)
        ->setGap(15)
        ->setCrossAxisAlignment(AxisAlignment::Start)
        ->setCrossAxisLineAlignment(AxisAlignment::Start)
        ->setAxisReverse(true)
    );





    //Adding Settings to popup

    //Color Title
    {
        auto entry = OSPSettingEntry::createTitle("Color Settings");
        menu->addChild(entry);
        menu->updateLayout();
    }

    //Change Colors
    {
        auto toggler = CCMenuItemToggler::create(
            CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
            CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
            this,
            nullptr
        );

        auto btn = CCMenuItemExt::createSpriteExtra(toggler, [toggler](CCObject*) {
            bool state = !toggler->isToggled();
            toggler->toggle(state);
            Mod::get()->setSettingValue("change-colors", state);
            onionSkin->changeColors = state;
        });


        toggler->toggle(Mod::get()->getSettingValue<bool>("change-colors"));

        auto entry = OSPSettingEntry::create(btn, "Change Object Colors");
        //entry->setInfo("test", "this is a test node");
        menu->addChild(entry);
        menu->updateLayout();
    }

    //Past Color
    {   
        auto node = CCSprite::create("GJ_button_04.png");
        node->setScale(0.75);
        auto colorSprite = CCSprite::create("square02b_001.png");
        colorSprite->setScale(0.44);

        colorSprite->setZOrder(10);
        colorSprite->setColor(Mod::get()->getSettingValue<cocos2d::ccColor3B>("past-color"));

        node->addChildAtPosition(colorSprite, Anchor::Center);

        auto btn = CCMenuItemExt::createSpriteExtra(node, [colorSprite](CCObject*) {

            ccColor3B currentColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("past-color");
            ccColor4B startColor = { currentColor.r, currentColor.g, currentColor.b, 255 };

            colorSprite->setColor(currentColor);

            auto colorPicker = ColorPickPopup::create(currentColor);

            colorPicker->setCallback(geode::Function<void(const cocos2d::ccColor4B&)>(
                [colorSprite](const cocos2d::ccColor4B& newColor) {
                    colorSprite->setColor({ newColor.r, newColor.g, newColor.b });
                    Mod::get()->setSettingValue("past-color", cocos2d::ccColor3B{ newColor.r, newColor.g, newColor.b });
                }
            ));

            colorPicker->show();
        });

        btn->setContentSize({31, 31});

        auto entry = OSPSettingEntry::create(btn, "Past Frames");
        entry->setInfo("Past Frames", "Change what color past frames show up as.");
        menu->addChild(entry);
        menu->updateLayout();
    }

    //Future Color
    {   
        auto node = CCSprite::create("GJ_button_04.png");
        node->setScale(0.75);
        auto colorSprite = CCSprite::create("square02b_001.png");
        colorSprite->setScale(0.44);

        colorSprite->setZOrder(10);
        colorSprite->setColor(Mod::get()->getSettingValue<cocos2d::ccColor3B>("future-color"));

        node->addChildAtPosition(colorSprite, Anchor::Center);

        auto btn = CCMenuItemExt::createSpriteExtra(node, [colorSprite](CCObject*) {

            ccColor3B currentColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("future-color");
            ccColor4B startColor = { currentColor.r, currentColor.g, currentColor.b, 255 };

            colorSprite->setColor(currentColor);

            auto colorPicker = ColorPickPopup::create(currentColor);

            colorPicker->setCallback(geode::Function<void(const cocos2d::ccColor4B&)>(
                [colorSprite](const cocos2d::ccColor4B& newColor) {
                    colorSprite->setColor({ newColor.r, newColor.g, newColor.b });
                    Mod::get()->setSettingValue("future-color", cocos2d::ccColor3B{ newColor.r, newColor.g, newColor.b });
                }
            ));

            colorPicker->show();
        });

        btn->setContentSize({31, 31});

        auto entry = OSPSettingEntry::create(btn, "Future Frames");
        entry->setInfo("Future Frames", "Change what color future frames show up as.");
        menu->addChild(entry);
        menu->updateLayout();
    }




    //Rendering Title
    {
        auto entry = OSPSettingEntry::createTitle("Rendering Settings");
        menu->addChild(entry);
        menu->updateLayout();
    }

    //Show Timestamp In Editor
    {
        auto toggler = CCMenuItemToggler::create(
            CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
            CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
            this,
            nullptr
        );

        onionSkin->showTimestamp = Mod::get()->getSettingValue<bool>("show-timestamp");
        auto btn = CCMenuItemExt::createSpriteExtra(toggler, [toggler](CCObject*) {
            bool state = !toggler->isToggled();
            toggler->toggle(state);
            Mod::get()->setSettingValue("show-timestamp", state);
            onionSkin->showTimestamp = state;
        });


        toggler->toggle(Mod::get()->getSettingValue<bool>("show-timestamp"));

        auto entry = OSPSettingEntry::create(btn, "Playback Timestamp");
        entry->setInfo("Playback Timestamp", "Show playback timestamp in editor. Shows in top left corner.");
        menu->addChild(entry);
        menu->updateLayout();
    }

    //Show Past
    {
        auto toggler = CCMenuItemToggler::create(
            CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
            CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
            this,
            nullptr
        );

        auto btn = CCMenuItemExt::createSpriteExtra(toggler, [toggler](CCObject*) {
            bool state = !toggler->isToggled();
            toggler->toggle(state);
            Mod::get()->setSettingValue("show-past", state);
            onionSkin->renderPast = state;
        });


        toggler->toggle(Mod::get()->getSettingValue<bool>("show-past"));
        rpSetting = toggler;

        auto entry = OSPSettingEntry::create(btn, "Show Past Frames");
        //entry->setInfo("test", "this is a test node");
        menu->addChild(entry);
        menu->updateLayout();
    }

    //Show Future
    {
        auto toggler = CCMenuItemToggler::create(
            CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
            CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
            this,
            nullptr
        );

        auto btn = CCMenuItemExt::createSpriteExtra(toggler, [toggler](CCObject*) {
            bool state = !toggler->isToggled();
            toggler->toggle(state);
            Mod::get()->setSettingValue("show-future", state);
            onionSkin->renderFuture = state;
        });


        toggler->toggle(Mod::get()->getSettingValue<bool>("show-future"));
        rfSetting = toggler;

        auto entry = OSPSettingEntry::create(btn, "Show Future Frames");
        //entry->setInfo("test", "this is a test node");
        menu->addChild(entry);
        menu->updateLayout();
    }

    //Depth Slider
    {
        auto label = OSPSettingEntry::create("Frame Cutoff");

        frameDepthInput = TextInput::create(30, numToString<int>(Mod::get()->getSettingValue<int>("frame-depth")));
        frameDepthInput->setFilter("0123456789");
        frameDepthInput->setCallback([this](const std::string& value){ 
            int val = numFromString<int>(value).unwrapOr(Mod::get()->getSettingValue<int>("frame-depth")); 

            val = std::clamp(val, 1, MAX_FRAME_DEPTH);
            onionSkin->maxLayerDistance = val;
            Mod::get()->setSavedValue<int>("frame-depth", val);
            if (!frameDepthInput->getString().empty())
                this->frameDepthInput->setString(numToString<int>(val));
            else
                this->frameDepthInput->setPlaceholder(numToString<int>(val));
            
            
        });

        label->addChild(frameDepthInput);
        label->updateMainNode(frameDepthInput);
        label->setInfo("Frame Cutoff", "Change how many past and future frames will be displayed.");
        label->updateLabelScale();

        menu->addChild(label);
        menu->updateLayout();

        auto slider = SliderNode::create([](SliderNode* node, float value) {
            onionSkin->maxLayerDistance = static_cast<int>(value);
        });
        slider->setMin(1);
        slider->setMax(MAX_FRAME_DEPTH);
        slider->linkTextInput(frameDepthInput, 0);
        slider->setValue(onionSkin->maxLayerDistance); 

        auto entry = OSPSettingEntry::create(slider);
        
        menu->addChild(entry);
        menu->updateLayout();

        label->updateLabelScale();
    }

    //Max Opacity Slider
    {
        auto label = OSPSettingEntry::create("Maximum Frame Opacity");

        opacityMaxInput = TextInput::create(30, numToString<int>(Mod::get()->getSettingValue<int>("max-opacity")));
        opacityMaxInput->setFilter("0123456789");
        opacityMaxInput->setCallback([this](const std::string& value){ 
            int val = numFromString<int>(value).unwrapOr(Mod::get()->getSettingValue<int>("max-opacity")); 

            val = std::clamp(val, 0, 100);
            onionSkin->maxOpacity = val;
            Mod::get()->setSavedValue<int>("max-opacity", val);
            if (!opacityMaxInput->getString().empty())
                this->opacityMaxInput->setString(numToString<int>(val));
            else
                this->opacityMaxInput->setPlaceholder(numToString<int>(val));
            
            
        });

        label->addChild(opacityMaxInput);
        label->updateMainNode(opacityMaxInput);

        label->updateLabelScale();

        menu->addChild(label);
        menu->updateLayout();

        auto slider = SliderNode::create([](SliderNode* node, float value) {
            onionSkin->maxOpacity = static_cast<int>(value);
        });
        slider->setMin(0);
        slider->setMax(100);
        slider->linkTextInput(opacityMaxInput, 0);
        slider->setValue(onionSkin->maxOpacity);

        auto entry = OSPSettingEntry::create(slider);
        
        menu->addChild(entry);
        menu->updateLayout();

        label->updateLabelScale();
    }

    //Min Opacity Slider
    {
        auto label = OSPSettingEntry::create("Minimum Frame Opacity");

        opacityMinInput = TextInput::create(30, numToString<int>(Mod::get()->getSettingValue<int>("min-opacity")));
        opacityMinInput->setFilter("0123456789");
        opacityMinInput->setCallback([this](const std::string& value){ 
            int val = numFromString<int>(value).unwrapOr(Mod::get()->getSettingValue<int>("min-opacity")); 

            val = std::clamp(val, 0, 100);
            onionSkin->minOpacity = val;
            Mod::get()->setSavedValue<int>("min-opacity", val);
            if (!opacityMinInput->getString().empty())
                this->opacityMinInput->setString(numToString<int>(val));
            else
                this->opacityMinInput->setPlaceholder(numToString<int>(val));
            
            
        });

        label->addChild(opacityMinInput);
        label->updateMainNode(opacityMinInput);

        label->updateLabelScale();

        menu->addChild(label);
        menu->updateLayout();

        auto slider = SliderNode::create([](SliderNode* node, float value) {
            onionSkin->minOpacity = static_cast<int>(value);
        });
        slider->setMin(0);
        slider->setMax(100);
        slider->linkTextInput(opacityMinInput, 0);
        slider->setValue(onionSkin->minOpacity);

        auto entry = OSPSettingEntry::create(slider);
        
        menu->addChild(entry);
        menu->updateLayout();

        label->updateLabelScale();
    }







    
    scrollLayer->m_contentLayer->addChildAtPosition(menu, Anchor::Center);
    scrollLayer->m_contentLayer->setContentHeight(menu->getContentHeight() + 15);
    scrollLayer->m_contentLayer->updateLayout();
    
    scrollBG->addChildAtPosition(scrollLayer, Anchor::Center, {0, 0});
    scrollBG->addChildAtPosition(scrollBar, Anchor::Center, {107, 0});

    scrollLayer->scrollToTop();
    scrollLayer->ignoreAnchorPointForPosition(false);


    popupBase->addChildAtPosition(scrollBG, Anchor::Center, {68, 0});
    

    
    popupMainMenu->updateLayout();


    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);

    return true;
}




void OSPopup::registerWithTouchDispatcher() {
    CCTouchDispatcher::get()->addTargetedDelegate(this, -500, true);
}

OSPopup::~OSPopup() {
    CCTouchDispatcher::get()->unregisterForcePrio(this);
}



void OSPopup::onBack(CCObject*) {   
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);

    onionSkin->animBounds = {frameRange[0], frameRange[1]};
    onionSkin->fps = pbFPS;
    
    onionSkin->settingsPopup = nullptr;


    this->removeFromParent();
}

void OSPopup::keyBackClicked() {
    this->onBack(nullptr);
}

OSPopup* OSPopup::create() {
    auto ret = new OSPopup();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
};





void onOSKeybind() {
    onionSkin->onionEnabled = !onionSkin->onionEnabled;
    onionSkin->toggler->toggle(onionSkin->onionEnabled);
    if (onionSkin->settingsPopup != nullptr && onionSkin->settingsPopup->OStoggle != nullptr) {
        onionSkin->settingsPopup->OStoggle->toggle(onionSkin->onionEnabled);
    }

    // Change menu setting accordingly
    auto alert = TextAlertPopup::create("Onion Skin: " + std::string(onionSkin->onionEnabled ? "On" : "Off"), 0.6f, 0.6f, 100, "chatFont-uhd.fnt");

    alert->setAlertPosition( 
        { 0.f, 1.f },
        { 20.f, -20.f }
    );
    alert->setLabelColor({0, 255, 0});
    alert->setZOrder(9999);
    EditorUI::get()->addChild(alert);
}

void onPBKeybind() {
    onionSkin->playbackEnabled = !onionSkin->playbackEnabled;
    if (onionSkin->settingsPopup != nullptr && onionSkin->settingsPopup->PBtoggle != nullptr) {
        onionSkin->settingsPopup->PBtoggle->toggle(onionSkin->playbackEnabled);
    }

    // Change menu setting accordingly
    auto alert = TextAlertPopup::create("Animation Playback: " + std::string(onionSkin->playbackEnabled ? "On" : "Off"), 0.6f, 0.6f, 100, "chatFont-uhd.fnt");

    alert->setAlertPosition( 
        { 0.f, 1.f },
        { 20.f, -20.f }
    );
    alert->setLabelColor({0, 255, 0});
    alert->setZOrder(9999);
    EditorUI::get()->addChild(alert);
}



void onRPKeybind() {
    onionSkin->renderPast = !onionSkin->renderPast;
    if (onionSkin->settingsPopup != nullptr && onionSkin->settingsPopup->rpSetting != nullptr) {
        onionSkin->settingsPopup->rpSetting->toggle(onionSkin->renderPast);
    }

    // Change menu setting accordingly
    Mod::get()->setSettingValue<bool>("show-past", onionSkin->renderPast);
    auto alert = TextAlertPopup::create("Past Frames: " + std::string(onionSkin->renderPast ? "On" : "Off"), 0.6f, 0.6f, 100, "chatFont-uhd.fnt");

    alert->setAlertPosition( // Great value
        { 0.f, 1.f },
        { 20.f, -20.f }
    );
    alert->setLabelColor({0, 255, 0});
    alert->setZOrder(9999);
    EditorUI::get()->addChild(alert);
}

void onRFKeybind() {
    onionSkin->renderFuture = !onionSkin->renderFuture;
    if (onionSkin->settingsPopup != nullptr && onionSkin->settingsPopup->rfSetting != nullptr) {
        onionSkin->settingsPopup->rfSetting->toggle(onionSkin->renderFuture);
    }

    // Change menu setting accordingly
    Mod::get()->setSettingValue<bool>("show-future", onionSkin->renderFuture);
    auto alert = TextAlertPopup::create("Future Frames: " + std::string(onionSkin->renderFuture ? "On" : "Off"), 0.6f, 0.6f, 100, "chatFont-uhd.fnt");

    alert->setAlertPosition( 
        { 0.f, 1.f },
        { 20.f, -20.f }
    );
    alert->setLabelColor({0, 255, 0});
    alert->setZOrder(9999);
    EditorUI::get()->addChild(alert);
}


#include <Geode/modify/EditorUI.hpp>
class $modify(myEditorUI, EditorUI) {
    struct Fields {
        geode::ListenerHandle* m_osListener = nullptr;
        geode::ListenerHandle* m_pbListener = nullptr;

        geode::ListenerHandle* m_rpListener = nullptr;
        geode::ListenerHandle* m_rfListener = nullptr;

        bool keybindsRegistered = false;
        CCLabelBMFont* m_timestampLabel = nullptr;
        CCLabelBMFont* m_frameCountLabel = nullptr;

    };
    


    static void onModify(auto& self) {
        (void)self.setHookPriority("EditorUI::init", -1600); // Make late to add button to far right
    }

    void updateTimerLabel() {
        std::string prefix = numToString<int>(onionSkin->pbFrameNum);
        std::string suffix = numToString<int>(onionSkin->animBounds.second);

        if (onionSkin->playbackEnabled) {
            onionSkin->frameCountLabel->setString(fmt::format("{}/{}", prefix, suffix).c_str());
        } else {

            onionSkin->frameCountLabel->setString(fmt::format("0/{}", suffix).c_str());
        }
            
    }

    void updateTimestamp() {
        if (onionSkin->playbackEnabled) {
            onionSkin->timestampLabel->setString(fmt::format("{:.2f}s", onionSkin->pbTimer).c_str());
        } else {
            onionSkin->timestampLabel->setString("0.00s");
        }
    }

    //Update Playback Animation
    void updatePlayback(double dt) {

        bool show = onionSkin->showTimestamp;
        onionSkin->timestampLabel->setVisible(show);
        onionSkin->frameCountLabel->setVisible(show);

        updateTimerLabel();
        updateTimestamp();

        if (!onionSkin->playbackEnabled) return;

        

        onionSkin->pbTimer += dt;

        int start = onionSkin->animBounds.first;
        int end = onionSkin->animBounds.second;

        //direction
        bool reverse = start > end;

        int range = std::abs(end - start) + 1;
        if (range <= 0) return;

        int frame = static_cast<int>(onionSkin->pbTimer * onionSkin->fps);

        if (frame >= range) {
            onionSkin->pbTimer = 0.0;
            frame = 0;
        }

        if (reverse) {
            //back
            onionSkin->pbFrameNum = start - frame;
        } else {
            //up
            onionSkin->pbFrameNum = start + frame;
        }

    }

    bool init(LevelEditorLayer* p0) {
        if (!EditorUI::init(p0))
            return false;

        if (!onionSkin)
            onionSkin = new OnionSkin();

        auto winSize = CCDirector::get()->getWinSize();

        onionSkin->showTimestamp = Mod::get()->getSettingValue<bool>("show-timestamp");
        
        onionSkin->timestampLabel = CCLabelBMFont::create("0.00s", "chatFont.fnt");
        onionSkin->timestampLabel->setID("playback-timestamp"_spr);
        onionSkin->timestampLabel->setAnchorPoint({0, 0.5});
        onionSkin->timestampLabel->setScale(0.5);
        onionSkin->timestampLabel->setZOrder(1000);
        onionSkin->timestampLabel->setPosition({70, winSize.height - (onionSkin->timestampLabel->getScaledContentHeight() / 2)});
        this->addChild(onionSkin->timestampLabel);
        onionSkin->timestampLabel->setVisible(onionSkin->showTimestamp);


        onionSkin->frameCountLabel = CCLabelBMFont::create("0/0", "chatFont.fnt");
        onionSkin->frameCountLabel->setID("playback-frame-count"_spr);
        onionSkin->frameCountLabel->setAnchorPoint({0, 0.5});
        onionSkin->frameCountLabel->setScale(0.5);
        onionSkin->frameCountLabel->setZOrder(1000);
        onionSkin->frameCountLabel->setPosition({45, winSize.height - (onionSkin->frameCountLabel->getScaledContentHeight() / 2)});
        this->addChild(onionSkin->frameCountLabel);
        onionSkin->frameCountLabel->setVisible(onionSkin->showTimestamp);
    
        
        
           
        if (!onionSkin->kbRegistered) {

            //Onion Skin Toggle
            m_fields->m_osListener = listenForKeybindSettingPresses("onion-enable-keybind", [](Keybind const& keybind, bool down, bool repeat, double timestamp) {
                if (!down || repeat) return;
                onOSKeybind();
                
            });


            //Animation Playback Toggle
            m_fields->m_pbListener = listenForKeybindSettingPresses("playback-enable-keybind", [](Keybind const& keybind, bool down, bool repeat, double timestamp) {
                if (!down || repeat) return;
                onPBKeybind();
            });
        

            //Past Frames Toggle
            m_fields->m_rpListener = listenForKeybindSettingPresses("render-past-keybind", [](Keybind const& keybind, bool down, bool repeat, double timestamp) {
                if (!down || repeat) return;
                onRPKeybind();
                
            });
        
        
            //Future Frames Toggle
            m_fields->m_rfListener = listenForKeybindSettingPresses("render-future-keybind", [](Keybind const& keybind, bool down, bool repeat, double timestamp) {
                if (!down || repeat) return;
                onRFKeybind();
                
            });

            onionSkin->kbRegistered = true;
        }

        if (auto layerMenu = this->getChildByID("layer-menu")) {
            
            
            // Get settings
            onionSkin->changeColors = Mod::get()->getSettingValue<bool>("change-colors");
            onionSkin->pastColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("past-color");
            onionSkin->futureColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("future-color");
            onionSkin->maxLayerDistance = Mod::get()->getSettingValue<int>("frame-depth");
            onionSkin->maxOpacity = Mod::get()->getSettingValue<int>("max-opacity");
            onionSkin->minOpacity = Mod::get()->getSettingValue<int>("min-opacity");
            onionSkin->renderPast = Mod::get()->getSettingValue<bool>("show-past");
            onionSkin->renderFuture = Mod::get()->getSettingValue<bool>("show-future");
            onionSkin->LevelEditorLayer = p0;

            onionSkin->fps = Mod::get()->getSavedValue<int>("playback-speed", PLAYBACK_SPEED);
            auto vec = Mod::get()->getSavedValue<std::vector<int>>("playback-range", {0, PLAYBACK_END});
            onionSkin->animBounds = {vec[0], vec[1]};
            if (onionSkin->currentLayer <= onionSkin->animBounds.second && onionSkin->currentLayer >= onionSkin->animBounds.first) {
                onionSkin->pbFrameNum = onionSkin->currentLayer;
            } else {
                onionSkin->pbFrameNum = onionSkin->animBounds.first;
            }

            // Create toggle button
            auto onionOnSpr = CCSprite::create("onionBtn_on.png"_spr);
            auto onionOffSpr = CCSprite::create("onionBtn_off.png"_spr);

            onionOnSpr->setScale(0.75f);
            onionOffSpr->setScale(0.75f);

            onionSkin->layerToggleOff = onionOffSpr;
            onionSkin->layerToggleOn = onionOnSpr;
            
            onionSkin->toggler = CCMenuItemToggler::create(onionOffSpr, onionOnSpr, this, nullptr);
            onionSkin->toggler->setID("onion-skin-toggle"_spr);
            onionSkin->toggler->m_notClickable = true;
            onionSkin->toggler->toggle(onionSkin->onionEnabled);

            onionSkin->LayerToggle = CCMenuItemSpriteExtra::create(onionSkin->toggler, this, menu_selector(myEditorUI::onOnionButton));

            // Add toggle button to layer menu
            layerMenu->addChild(onionSkin->LayerToggle);
            layerMenu->updateLayout();

            onionSkin->currentLayer = p0->m_currentLayer;

            // Check for layer updates
            schedule(schedule_selector(myEditorUI::checkLayer), 0);
            schedule(schedule_selector(myEditorUI::updatePlaybackVal), 0);
        }

        
		
        return true;
    }
    

    void keyDown(cocos2d::enumKeyCodes key, double timestamp) {

        auto os = Mod::get()->getSettingValue<std::vector<geode::Keybind>>("onion-enable-keybind");
        auto pb = Mod::get()->getSettingValue<std::vector<geode::Keybind>>("playback-enable-keybind");


        auto pas = Mod::get()->getSettingValue<std::vector<geode::Keybind>>("render-past-keybind");
        auto fut = Mod::get()->getSettingValue<std::vector<geode::Keybind>>("render-future-keybind");

        //Shift Modifier
        if (key == enumKeyCodes::KEY_Shift || key == enumKeyCodes::KEY_LeftShift || key == enumKeyCodes::KEY_RightShift) onionSkin->shiftModifier = true;


        // Override vanilla keybinds
        if (onionSkin->shiftModifier) if (key == os.back().key || key == pb.back().key || key == pas.back().key || key == fut.back().key) return;
        //Isn't it wonderful?
        
        

		EditorUI::keyDown(key, timestamp);

		if (!onionSkin->playtesting || getChildByID("position-slider")->isVisible()) {
			onionSkin->LayerToggle->setVisible(true);
		}
		else {
			onionSkin->LayerToggle->setVisible(false);
		}   
	}

    void keyUp(enumKeyCodes key, double timestamp) {
        if (key == enumKeyCodes::KEY_Shift || key == enumKeyCodes::KEY_LeftShift || key == enumKeyCodes::KEY_RightShift) onionSkin->shiftModifier = false;
        EditorUI::keyUp(key, timestamp);
    }

    void onPlaytest(cocos2d::CCObject* sender) {
        EditorUI::onPlaytest(sender);
        onionSkin->playtesting = true;
    }

    void onStopPlaytest(cocos2d::CCObject* sender) {
        EditorUI::onStopPlaytest(sender);
        onionSkin->playtesting = false;
    }

    void showUI(bool show) {
		EditorUI::showUI(show);
		onionSkin->LayerToggle->setVisible(show);
	}

    void updatePlaybackVal(float dt) {
        updatePlayback(static_cast<double>(dt));
        if (!onionSkin->playbackEnabled) {
            onionSkin->pbTimer = 0;
            if (onionSkin->currentLayer <= onionSkin->animBounds.second && onionSkin->currentLayer >= onionSkin->animBounds.first) {
                onionSkin->pbFrameNum = onionSkin->currentLayer;
            } else {
                onionSkin->pbFrameNum = onionSkin->animBounds.first;
            }
        }
    }

    void checkLayer(float dt)
	{
        // I'm not really sure
		static int layer = -500;
		if (onionSkin->LevelEditorLayer->m_currentLayer == layer)
			return;

        // Update current layer
		layer = onionSkin->LevelEditorLayer->m_currentLayer;
        onionSkin->currentLayer = layer;

	
	}

    void onOnionButton(CCObject* sender) {
        auto popup = OSPopup::create();
        onionSkin->settingsPopup = popup;
        auto winSize = CCDirector::get()->getWinSize();
        popup->setPosition({winSize.width / 2, winSize.height / 2});
        this->addChild(popup);
    }

    ~myEditorUI() {
        m_fields->m_osListener->destroy();
        delete m_fields->m_osListener;
        m_fields->m_pbListener->destroy();
        delete m_fields->m_pbListener;
        m_fields->m_rfListener->destroy();
        delete m_fields->m_rfListener;
        m_fields->m_rpListener->destroy();
        delete m_fields->m_rpListener;

        if (onionSkin) {
            
            delete onionSkin;
            onionSkin = nullptr;
        }
    }

};






//GameObject
#include <Geode/modify/GameObject.hpp>
class $modify(MyGameObject, GameObject) {
    void updateMainColor(const ccColor3B& color) {
        auto ui = EditorUI::get();
        if (!ui) {
            GameObject::updateMainColor(color);
            return;
        }

        bool isSelected = ui->m_selectedObjects->containsObject(
            static_cast<cocos2d::CCObject*>(this)
        );

        if (!onionSkin->playbackEnabled && onionSkin->changeColors) {
            if (onionSkin->onionEnabled && !isSelected) {       
                ccColor3B newColor;

                if (this->m_editorLayer == onionSkin->currentLayer || onionSkin->currentLayer == -1) {
                    // Current layer
                    newColor = color; 
                }
                else if (this->m_editorLayer < onionSkin->currentLayer) {
                    // Below current layer
                    newColor = onionSkin->pastColor; 
                }
                else {
                    // Above current layer
                    newColor = onionSkin->futureColor; 
                }
            
                GameObject::updateMainColor(newColor); 
            } else {
                GameObject::updateMainColor(color);        
            }
        } else {
            GameObject::updateMainColor(color);
        }
    }

    void updateSecondaryColor(const ccColor3B& color) {
        auto ui = EditorUI::get();
        if (!ui) {
            GameObject::updateSecondaryColor(color);
            return;
        }

        bool isSelected = ui->m_selectedObjects->containsObject(
            static_cast<cocos2d::CCObject*>(this)
        );
        if (!onionSkin->playbackEnabled && onionSkin->changeColors) {
            if (onionSkin->onionEnabled && !isSelected) {
                ccColor3B newColor;
                if (this->m_editorLayer == onionSkin->currentLayer || onionSkin->currentLayer == -1) {
                    // Current layer 
                    newColor = color; 
                }
                else if (this->m_editorLayer < onionSkin->currentLayer) {
                    // Below current layer
                    newColor = onionSkin->pastColor; 
                }
                else {
                    // Above current layer
                    newColor = onionSkin->futureColor; 
                }
                GameObject::updateSecondaryColor(newColor);
            } else {
                GameObject::updateSecondaryColor(color);
            }
        } else {
            GameObject::updateSecondaryColor(color);
        }
    }

    void setOpacity(GLubyte opacity) {
        auto ui = EditorUI::get();
        if (!ui) {
            GameObject::setOpacity(opacity);
            return;
        }

        bool isSelected = ui->m_selectedObjects->containsObject(
            static_cast<cocos2d::CCObject*>(this)
        );
        if (!onionSkin->playbackEnabled) {
            if (onionSkin->onionEnabled && !isSelected) {
                GLubyte newAlpha;
                
                if (this->m_editorLayer == onionSkin->currentLayer || onionSkin->currentLayer == -1) {   
                    // Current layer    
                    newAlpha = opacity; 
                } else {
                    // Not current layer
                    int distance = std::abs(this->m_editorLayer - onionSkin->currentLayer);
                    int maxDistance = onionSkin->maxLayerDistance;

                    float scaleFactor = (maxDistance > 0) ? std::max(0.0f, 1.0f - (float)distance / maxDistance) : 1.0f;

                    float minAlpha = (onionSkin->minOpacity / 100.0f) * 255.0f;
                    float maxAlpha = (onionSkin->maxOpacity / 100.0f) * 255.0f;

                    newAlpha = static_cast<GLubyte>(minAlpha + scaleFactor * (maxAlpha - minAlpha));
                }
                
                // Make invisible if it is supposed to not be visible (genius)
                if ((!onionSkin->renderPast && this->m_editorLayer < onionSkin->currentLayer) || (!onionSkin->renderFuture && this->m_editorLayer > onionSkin->currentLayer)) {
                    newAlpha = 0;
                }

                GameObject::setOpacity(newAlpha);
            } else {
                GameObject::setOpacity(opacity);
            }
        } else {
            if (this->m_editorLayer == onionSkin->pbFrameNum) {
                GameObject::setOpacity(255);
            } else {
                GameObject::setOpacity(0);
            }
        }
        
    }

};


