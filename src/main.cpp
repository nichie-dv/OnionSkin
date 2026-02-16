#include <Geode/Geode.hpp>
#include <Geode/loader/SettingV3.hpp>
#include "main.hpp"



using namespace geode::prelude;

OnionSkin::Fields* g_onionFields = nullptr;


void onRPKeybind() {
    g_onionFields->renderPast = !g_onionFields->renderPast;

    // Change menu setting accordingly
    Mod::get()->setSettingValue<bool>("show-past", g_onionFields->renderPast);
    auto alert = TextAlertPopup::create("Past Frames: " + std::string(g_onionFields->renderPast ? "On" : "Off"), 0.6f, 0.6f, 100, "chatFont-uhd.fnt");

    alert->setAlertPosition( // Great value
        { 0.f, 1.f },
        { 20.f, -20.f }
    );
    alert->setLabelColor({0, 255, 0});
    alert->setZOrder(9999);
    EditorUI::get()->addChild(alert);
}

void onRFKeybind() {
    g_onionFields->renderFuture = !g_onionFields->renderFuture;

    // Change menu setting accordingly
    Mod::get()->setSettingValue<bool>("show-future", g_onionFields->renderFuture);
    auto alert = TextAlertPopup::create("Future Frames: " + std::string(g_onionFields->renderFuture ? "On" : "Off"), 0.6f, 0.6f, 100, "chatFont-uhd.fnt");

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

        if (auto layerMenu = this->getChildByID("layer-menu")) {
            if (!g_onionFields)
                g_onionFields = new OnionSkin::Fields();
            
            // Get settings
            g_onionFields->pastColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("past-color");
            g_onionFields->futureColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("future-color");
            g_onionFields->maxLayerDistance = Mod::get()->getSettingValue<int>("frame-depth");
            g_onionFields->maxOpacity = Mod::get()->getSettingValue<int>("max-opacity");
            g_onionFields->minOpacity = Mod::get()->getSettingValue<int>("min-opacity");
            g_onionFields->renderPast = Mod::get()->getSettingValue<bool>("show-past");
            g_onionFields->renderFuture = Mod::get()->getSettingValue<bool>("show-future");
            g_onionFields->levelEditorLayer = p0;

            // Create toggle button
            auto onionOnSpr = CCSprite::create("onionBtn_on.png"_spr);
            auto onionOffSpr = CCSprite::create("onionBtn_off.png"_spr);

            onionOnSpr->setScale(0.75f);
            onionOffSpr->setScale(0.75f);

            g_onionFields->layerToggle = CCMenuItemToggler::create(onionOffSpr, onionOnSpr, this, menu_selector(myEditorUI::onToggle));
            g_onionFields->layerToggle->setID("onion-skin-toggle"_spr);
            g_onionFields->layerToggle->m_notClickable = false;
            g_onionFields->layerToggle->toggle(g_onionFields->onionEnabled);

            // Add toggle button to layer menu
            layerMenu->addChild(g_onionFields->layerToggle);
            layerMenu->updateLayout();

            g_onionFields->currentLayer = p0->m_currentLayer;

            // Check for layer updates
            schedule(schedule_selector(myEditorUI::checkLayer), 0);

            

        }
		
        return true;
    }

    void keyDown(cocos2d::enumKeyCodes key, double dt) {
        auto pas = Mod::get()->getSettingValue<std::vector<geode::Keybind>>("renderpast-keybind");
        auto fut = Mod::get()->getSettingValue<std::vector<geode::Keybind>>("renderpast-keybind");

        
        if (key == enumKeyCodes::KEY_Shift) {
            g_onionFields->shiftModifier = true;
        } else {
            g_onionFields->shiftModifier = false;;
        }
        
        if (key == pas.back().key && g_onionFields->shiftModifier) {
            onRPKeybind();
            return; 
        }
        if (key == fut.back().key && g_onionFields->shiftModifier) {
            onRFKeybind();
            return; 
        }
		EditorUI::keyDown(key, dt);
		if (!g_onionFields->playtesting || getChildByID("position-slider")->isVisible()) {
			g_onionFields->layerToggle->setVisible(true);
		}
		else {
			g_onionFields->layerToggle->setVisible(false);
		}
        
	}

    void onPlaytest(cocos2d::CCObject* sender) {
        EditorUI::onPlaytest(sender);
        g_onionFields->playtesting = true;
    }

    void onStopPlaytest(cocos2d::CCObject* sender) {
        EditorUI::onStopPlaytest(sender);
        g_onionFields->playtesting = false;
    }

    void showUI(bool show) {
		EditorUI::showUI(show);
		g_onionFields->layerToggle->setVisible(show);
	}

    void onToggle(CCObject*) {
        g_onionFields->onionEnabled = !g_onionFields->onionEnabled;
    }

    void checkLayer(float)
	{
        // I'm not really sure
		static int layer = -500;
		if (g_onionFields->levelEditorLayer->m_currentLayer == layer)
			return;

        // Update current layer
		layer = g_onionFields->levelEditorLayer->m_currentLayer;
        g_onionFields->currentLayer = layer;
		
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
            reinterpret_cast<cocos2d::CCObject*>(this)
        );
        
        if (g_onionFields->onionEnabled && !isSelected) {       
            ccColor3B newColor;

            if (this->m_editorLayer == g_onionFields->currentLayer || g_onionFields->currentLayer == -1) {
                // Current layer
                newColor = color; 
            }
            else if (this->m_editorLayer < g_onionFields->currentLayer) {
                // Below current layer
                newColor = g_onionFields->pastColor; 
            }
            else {
                // Above current layer
                newColor = g_onionFields->futureColor; 
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
            reinterpret_cast<cocos2d::CCObject*>(this)
        );

        if (g_onionFields->onionEnabled && !isSelected) {
            ccColor3B newColor;
            if (this->m_editorLayer == g_onionFields->currentLayer || g_onionFields->currentLayer == -1) {
                // Current layer 
                newColor = color; 
            }
            else if (this->m_editorLayer < g_onionFields->currentLayer) {
                // Below current layer
                newColor = g_onionFields->pastColor; 
            }
            else {
                // Above current layer
                newColor = g_onionFields->futureColor; 
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
            reinterpret_cast<cocos2d::CCObject*>(this)
        );

        if (g_onionFields->onionEnabled && !isSelected) {
            GLubyte newAlpha;
            
            if (this->m_editorLayer == g_onionFields->currentLayer || g_onionFields->currentLayer == -1) {   
                // Current layer    
                newAlpha = opacity; 
            } else {
                // Not current layer
                int distance = std::abs(this->m_editorLayer - g_onionFields->currentLayer);
                int maxDistance = g_onionFields->maxLayerDistance;

                float scaleFactor = (maxDistance > 0) ? std::max(0.0f, 1.0f - (float)distance / maxDistance) : 1.0f;

                float minAlpha = (g_onionFields->minOpacity / 100.0f) * 255.0f;
                float maxAlpha = (g_onionFields->maxOpacity / 100.0f) * 255.0f;

                newAlpha = static_cast<GLubyte>(minAlpha + scaleFactor * (maxAlpha - minAlpha));
            }
            
            // Make invisible if it is supposed to not be visible (genius)
            if ((!g_onionFields->renderPast && this->m_editorLayer < g_onionFields->currentLayer) || (!g_onionFields->renderFuture && this->m_editorLayer > g_onionFields->currentLayer)) {
                newAlpha = 0;
            }

            GameObject::setOpacity(newAlpha);
        } else {
            GameObject::setOpacity(opacity);
        }
    }

};


