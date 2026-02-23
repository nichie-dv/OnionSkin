#include <Geode/Geode.hpp>
#include <Geode/loader/SettingV3.hpp>
#include "main.hpp"

using namespace geode::prelude;

OnionSkin* onionSkin = nullptr;


void onRPKeybind() {
    onionSkin->renderPast = !onionSkin->renderPast;

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



    static void onModify(auto& self) {
        (void)self.setHookPriority("EditorUI::init", -1600); // Make late to add button to far right
    }

    bool init(LevelEditorLayer* p0) {
        if (!EditorUI::init(p0))
            return false;


        listenForKeybindSettingPresses("renderpast-keybind", [](Keybind const& keybind, bool down, bool repeat, double timestamp) {
            if (!down || repeat) return;
            onRPKeybind();
        });
        listenForKeybindSettingPresses("renderfuture-keybind", [](Keybind const& keybind, bool down, bool repeat, double timestamp) {
            if (!down || repeat) return;
            onRFKeybind();
        });
        

        if (auto layerMenu = this->getChildByID("layer-menu")) {
            if (!onionSkin)
                onionSkin = new OnionSkin();
            
            // Get settings
            onionSkin->pastColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("past-color");
            onionSkin->futureColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("future-color");
            onionSkin->maxLayerDistance = Mod::get()->getSettingValue<int>("frame-depth");
            onionSkin->maxOpacity = Mod::get()->getSettingValue<int>("max-opacity");
            onionSkin->minOpacity = Mod::get()->getSettingValue<int>("min-opacity");
            onionSkin->renderPast = Mod::get()->getSettingValue<bool>("show-past");
            onionSkin->renderFuture = Mod::get()->getSettingValue<bool>("show-future");
            onionSkin->LevelEditorLayer = p0;

            // Create toggle button
            auto onionOnSpr = CCSprite::create("onionBtn_on.png"_spr);
            auto onionOffSpr = CCSprite::create("onionBtn_off.png"_spr);

            onionOnSpr->setScale(0.75f);
            onionOffSpr->setScale(0.75f);

            onionSkin->LayerToggle = CCMenuItemToggler::create(onionOffSpr, onionOnSpr, this, menu_selector(myEditorUI::onToggle));
            onionSkin->LayerToggle->setID("onion-skin-toggle"_spr);
            onionSkin->LayerToggle->m_notClickable = false;
            onionSkin->LayerToggle->toggle(onionSkin->onionEnabled);

            // Add toggle button to layer menu
            layerMenu->addChild(onionSkin->LayerToggle);
            layerMenu->updateLayout();

            onionSkin->currentLayer = p0->m_currentLayer;

            // Check for layer updates
            schedule(schedule_selector(myEditorUI::checkLayer), 0);

            

        }
		
        return true;
    }
    

    void keyDown(cocos2d::enumKeyCodes key, double timestamp) {
        auto pas = Mod::get()->getSettingValue<std::vector<geode::Keybind>>("renderpast-keybind");
        auto fut = Mod::get()->getSettingValue<std::vector<geode::Keybind>>("renderfuture-keybind");

        if (key == enumKeyCodes::KEY_Shift || key == enumKeyCodes::KEY_LeftShift || key == enumKeyCodes::KEY_RightShift) onionSkin->shiftModifier = true;
        
        // Override vanilla keybinds
        if (key == pas.back().key && onionSkin->shiftModifier) return; 
        if (key == fut.back().key && onionSkin->shiftModifier) return;
        
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

    void onToggle(CCObject*) {
        onionSkin->onionEnabled = !onionSkin->onionEnabled;
    }

    void checkLayer(float)
	{
        // I'm not really sure
		static int layer = -500;
		if (onionSkin->LevelEditorLayer->m_currentLayer == layer)
			return;

        // Update current layer
		layer = onionSkin->LevelEditorLayer->m_currentLayer;
        onionSkin->currentLayer = layer;
		
	}

};


#include <Geode/modify/GameObject.hpp>
class $modify(MyGameObject, GameObject) {
public:
    void updateMainColor(const ccColor3B& color) {
        auto ui = EditorUI::get();
        if (!ui) {
            GameObject::updateMainColor(color);
            return;
        }

        bool isSelected = ui->m_selectedObjects->containsObject(
            static_cast<cocos2d::CCObject*>(this)
        );
        
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
    }

};


