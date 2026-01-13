#include <Geode/Geode.hpp>
#include "main.hpp"
#include <geode.custom-keybinds/include/OptionalAPI.hpp>
#include <Geode/ui/Notification.hpp>




using namespace keybinds;
$execute {
    BindManager::get()->registerBindable({
        // ID, should be prefixed with mod ID
        "renderPast"_spr,
        // Name
        "Render Past Frames",
        // Description, leave empty for none
        "",
        // Default binds
        { Keybind::create(cocos2d::KEY_F9, Modifier::None) },
        // Category; use slashes for specifying subcategories. See the
        // Category class for default categories
        "Onion Skin/Rendering"
    });

    BindManager::get()->registerBindable({
        // ID, should be prefixed with mod ID
        "renderFuture"_spr,
        // Name
        "Render Future Frames",
        // Description, leave empty for none
        "",
        // Default binds
        { Keybind::create(cocos2d::KEY_F10, Modifier::None) },
        // Category; use slashes for specifying subcategories. See the
        // Category class for default categories
        "Onion Skin/Rendering"
    });

}




using namespace geode::prelude;

OnionSkin::Fields* g_onionFields;

#include <Geode/modify/EditorUI.hpp>
class $modify(myEditorUI, EditorUI) {
	

    static void onModify(auto& self) {
        (void)self.setHookPriority("EditorUI::init", -1500); //add button to right hand side
    }

    bool init(LevelEditorLayer* p0) {
        if (!EditorUI::init(p0))
            return false;

        if (auto layerMenu = this->getChildByID("layer-menu")) {
            if (!g_onionFields)
                g_onionFields = new OnionSkin::Fields();
            
            // get settings
            g_onionFields->pastColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("past-color");
            g_onionFields->futureColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("future-color");
            g_onionFields->maxLayerDistance = Mod::get()->getSettingValue<int>("frame-depth");
            g_onionFields->maxOpacity = Mod::get()->getSettingValue<int>("max-opacity");
            g_onionFields->minOpacity = Mod::get()->getSettingValue<int>("min-opacity");
            g_onionFields->renderPast = Mod::get()->getSettingValue<bool>("show-past");
            g_onionFields->renderFuture = Mod::get()->getSettingValue<bool>("show-future");


            // add past callback
            this->addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
                if (event->isDown()) {
                    g_onionFields->renderPast = !g_onionFields->renderPast;
                    Mod::get()->setSettingValue<bool>("show-past", g_onionFields->renderPast);
                    auto alert = TextAlertPopup::create("Past Frames: " + std::string(g_onionFields->renderPast ? "On" : "Off"), 0.6f, 0.6f, 100, "chatFont-uhd.fnt");

                    alert->setAlertPosition( // thanks
                        { 0.f, 1.f },
                        { 20.f, -20.f }
                    );
                    alert->setLabelColor({0, 255, 0});
                    alert->setZOrder(9999);
                    EditorUI::get()->addChild(alert);
                    
                }
                return ListenerResult::Propagate;
            }, "renderPast"_spr);
            

            // add future callback
            this->addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
                if (event->isDown()) {
                    g_onionFields->renderFuture = !g_onionFields->renderFuture;
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
                return ListenerResult::Propagate;
            }, "renderFuture"_spr);


            g_onionFields->levelEditorLayer = p0;

            auto onionOnSpr = CCSprite::create("onionBtn_on.png"_spr);
            auto onionOffSpr = CCSprite::create("onionBtn_off.png"_spr);
            g_onionFields->layerToggle = CCMenuItemToggler::create(onionOffSpr, onionOnSpr, this, menu_selector(myEditorUI::onToggle));
            g_onionFields->layerToggle->setID("onion-skin-toggle"_spr);
            g_onionFields->layerToggle->m_notClickable = false;
            g_onionFields->layerToggle->toggle(g_onionFields->onionEnabled);

            layerMenu->addChild(g_onionFields->layerToggle);
            layerMenu->updateLayout();

            g_onionFields->currentLayer = p0->m_currentLayer;
            schedule(schedule_selector(myEditorUI::checkLayer), 0);

        }
		
        return true;
    }

    void onToggle(CCObject*) {
        g_onionFields->onionEnabled = !g_onionFields->onionEnabled;
    }

    void checkLayer(float)
	{
		static int layer = -500;
		if (g_onionFields->levelEditorLayer->m_currentLayer == layer)
			return;
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
                newColor = color; // current layer
            }
            else if (this->m_editorLayer < g_onionFields->currentLayer) {
                newColor = g_onionFields->pastColor; // below current layer
            }
            else {
                newColor = g_onionFields->futureColor; // above current layer
            }
        
            GameObject::updateMainColor(newColor); 
            if (this->hasSecondaryColor()) 
                MyGameObject::updateSecondaryColor(newColor);
        } else {
            GameObject::updateMainColor(color);
            if (this->hasSecondaryColor()) 
                GameObject::updateSecondaryColor(color);
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
                newColor = color; // current layer 
            }
            else if (this->m_editorLayer < g_onionFields->currentLayer) {
                newColor = g_onionFields->pastColor; // below current layer
            }
            else {
                newColor = g_onionFields->futureColor; // above current layer
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
                newAlpha = opacity; // current layer
            } else {
                int distance = std::abs(this->m_editorLayer - g_onionFields->currentLayer);
                int maxDistance = g_onionFields->maxLayerDistance;

                float scaleFactor = (maxDistance > 0) ? std::max(0.0f, 1.0f - (float)distance / maxDistance) : 1.0f;

                float minAlpha = (g_onionFields->minOpacity / 100.0f) * 255.0f;
                float maxAlpha = (g_onionFields->maxOpacity / 100.0f) * 255.0f;

                newAlpha = static_cast<GLubyte>(minAlpha + scaleFactor * (maxAlpha - minAlpha));
            }
            
            if ((!g_onionFields->renderPast && this->m_editorLayer < g_onionFields->currentLayer) || (!g_onionFields->renderFuture && this->m_editorLayer > g_onionFields->currentLayer)) {
                newAlpha = 0;
            }

            GameObject::setOpacity(newAlpha);
        } else {
            GameObject::setOpacity(opacity);
        }
    }

};







