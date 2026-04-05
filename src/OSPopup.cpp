#include <Geode/Geode.hpp>

#include "OSPopup.hpp"

using namespace geode::prelude;

//Create an entry with a button. If using a toggler, wrap it in CCMenuItemExt first
OSPSettingEntry* OSPSettingEntry::create(CCMenuItemSpriteExtra* button, std::string labelText) {
    auto ret = new OSPSettingEntry();
    if (ret->init(button, labelText, "bigFont.fnt")) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

//Create an entry with a slider
#include <Geode/ui/SliderNode.hpp>
OSPSettingEntry* OSPSettingEntry::create(geode::SliderNode* slider) {
    
    auto ret = new OSPSettingEntry();
    slider->setScale(0.7f);
    ret->xOffset = 10;

    if (ret->init(slider, "", "")) {
        ret->autorelease();
        ret->mainButton->setID("slider");
        return ret;
    }
    delete ret;
    return nullptr;
}

//Create a title
OSPSettingEntry* OSPSettingEntry::createTitle(std::string labelText) {
    auto ret = new OSPSettingEntry();
    if (ret->init(nullptr, labelText, "goldFont.fnt")) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

//Create only a label
OSPSettingEntry* OSPSettingEntry::create(std::string labelText) {
    auto ret = new OSPSettingEntry();
    if (ret->init(nullptr, labelText, "bigFont.fnt")) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool OSPSettingEntry::init(CCNode* button, std::string labelText, std::string font) {
    if (!CCMenu::init()) return false;

    if (button != nullptr) {
        mainButton = button;
        mainButton->setID("button");
        addChild(mainButton);
        updateLabelScale();
    }

    setLayout(AxisLayout::create()
        ->setAutoGrowAxis(false)
        ->setAutoScale(false)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    setPosition({0, 0});
    if (!labelText.empty()) {
        label = CCLabelBMFont::create(labelText.c_str(), font.c_str());
        label->setScale(0.75);
        label->setScale(190 / (label->getContentWidth()  / label->getScale()));
        label->setID("label");
        addChild(label);
    }
    
    updateLabelScale();
    return true;
}


//Update label scale so it doesnt go out of scope
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

    if (label != nullptr) label->limitLabelWidth(availableWidth, 1, 0.25f);
    this->updateLayout();


    //This is because slider will get cut off if i dont add an offset
    if (mainButton != nullptr) mainButton->setPositionX(mainButton->getPositionX() + xOffset);
    if (label != nullptr) label->setPositionX(label->getPositionX() + xOffset);
    if (infoIcon != nullptr) infoIcon->setPositionX(infoIcon->getPositionX() + xOffset);
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


//Just because mainButton is protected
void OSPSettingEntry::updateMainNode(CCNode* node) {
    mainButton = node;
}


//Create an info button and add it to the entry
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


