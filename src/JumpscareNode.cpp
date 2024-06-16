#include <Geode/Geode.hpp>
#include <Geode/loader/SettingNode.hpp>

#include "utils.hpp"
#include "JumpscareValue.hpp"

using namespace geode::prelude;
namespace fs = std::filesystem;

fs::path configDir = Mod::get()->getConfigDir();
std::string defaultJumpscare = (configDir / "jumpscare").string();

class JumpscareNode : public SettingNode {
protected:
    std::string m_currentJumpscare;
    std::vector<fs::path> m_jumpscareDirs;

    bool init(JumpscareValue* value, float width) {
        if (!SettingNode::init(value))
            return false;
        
        m_currentJumpscare = value->getJumpscare();
        m_jumpscareDirs = getJumpscareSubDir(configDir);

        this->setContentSize({ width, 55.f });

        auto menu = CCMenu::create();
        menu->setPosition(width - 20.f, 27.5f);
        menu->setID("button-menu");
        this->addChild(menu);

        auto label = CCLabelBMFont::create("Jumpscare in use", "bigFont.fnt");
        label->setAnchorPoint({0.f, 0.5f});
        label->setPosition(20.f, 27.5f);
        label->setScale(0.5f);
        this->addChild(label);

        auto inputNode = InputNode::create(103.f, "...", "chatFont.fnt");
        inputNode->setScale(0.65f);
        inputNode->setPosition(-51.5f, 0);
        inputNode->setString(fs::path(as<JumpscareValue*>(m_value)->getJumpscare()).filename().string());
        inputNode->getInput()->setAllowedChars("");
        menu->addChild(inputNode);

        auto arrowRight = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
        arrowRight->setScale(0.3f);
        auto buttonRight = CCMenuItemSpriteExtra::create(arrowRight, this, menu_selector(JumpscareNode::onNext));
        buttonRight->setPosition(-10.f, 0);
        menu->addChild(buttonRight);

        auto arrowLeft = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
        arrowLeft->setScale(0.3f);
        arrowLeft->setFlipX(true);
        auto buttonLeft = CCMenuItemSpriteExtra::create(arrowLeft, this, menu_selector(JumpscareNode::onPrev));
        buttonLeft->setPosition(-93.f, 0);
        menu->addChild(buttonLeft);
        
        return true;
    }

    void onNext(CCObject* sender) {
        m_jumpscareDirs = getJumpscareSubDir(configDir);
        int index = std::distance(m_jumpscareDirs.begin(), std::find(m_jumpscareDirs.begin(), m_jumpscareDirs.end(), configDir / m_currentJumpscare));
        if (index >= m_jumpscareDirs.size() - 1)
            index = 0;
        else 
            index++;

        m_currentJumpscare = m_jumpscareDirs[index].string();
        getChildOfType<InputNode>(this->getChildByID("button-menu"), 0)->setString(m_jumpscareDirs[index].filename().string());
        this->dispatchChanged();
    }

    void onPrev(CCObject* sender) {
        m_jumpscareDirs = getJumpscareSubDir(configDir);
        int index = std::distance(m_jumpscareDirs.begin(), std::find(m_jumpscareDirs.begin(), m_jumpscareDirs.end(), configDir / m_currentJumpscare));
        if (index >= m_jumpscareDirs.size() || index == 0)
            index = m_jumpscareDirs.size() - 1;
        else
            index--;

        m_currentJumpscare = m_jumpscareDirs[index].string();
        getChildOfType<InputNode>(this->getChildByID("button-menu"), 0)->setString(m_jumpscareDirs[index].filename().string());
        this->dispatchChanged();
    }

public:
    // to save the setting
    void commit() override {
        as<JumpscareValue*>(m_value)->setJumpscare(m_currentJumpscare);
        this->dispatchCommitted();
    }

    bool hasUncommittedChanges() override {
        return m_currentJumpscare != as<JumpscareValue*>(m_value)->getJumpscare();
    }

    bool hasNonDefaultValue() override {
        return m_currentJumpscare != defaultJumpscare;
    }

    void resetToDefault() override {
        m_currentJumpscare = defaultJumpscare;
    }

    static JumpscareNode* create(JumpscareValue* value, float width) {
        auto ret = new JumpscareNode();
        if (ret && ret->init(value, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

SettingNode* JumpscareValue::createNode(float width) {
    return JumpscareNode::create(this, width);
}


$on_mod(Loaded) {
	Mod::get()->addCustomSetting<JumpscareValue>("jumpscare_in_use", defaultJumpscare);
}