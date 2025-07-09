#include <Geode/Geode.hpp>

#include "utils.hpp"
#include "JumpscareValue.hpp"

using namespace geode::prelude;
namespace fs = std::filesystem;

fs::path configDir = Mod::get()->getConfigDir();
std::string defaultJumpscare = (configDir / "jumpscare").string();

class JumpscareNode : public SettingNodeV3 {
protected:
    std::string m_currentJumpscare;
    std::vector<fs::path> m_jumpscareDirs;
    JumpscareValue* m_value {};

    bool init(std::shared_ptr<JumpscareValue> setting, float width) {
        if (!SettingNodeV3::init(setting, width))
            return false;

        m_value = setting.get();
        m_currentJumpscare = m_value->getJumpscare();
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

        auto textInput = TextInput::create(103.f, "...", "chatFont.fnt");
        textInput->setScale(0.65f);
        textInput->setPosition(-51.5f, 0);
        textInput->setString(fs::path(m_value->getJumpscare()).filename().string());
        textInput->getInputNode()->setTouchEnabled(false);
        menu->addChild(textInput);

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
        this->getChildByID("button-menu")->getChildByType<TextInput>(0)->setString(m_jumpscareDirs[index].filename().string());
        // this->dispatchChanged();
    }

    void onPrev(CCObject* sender) {
        m_jumpscareDirs = getJumpscareSubDir(configDir);
        int index = std::distance(m_jumpscareDirs.begin(), std::find(m_jumpscareDirs.begin(), m_jumpscareDirs.end(), configDir / m_currentJumpscare));
        if (index >= m_jumpscareDirs.size() || index == 0)
            index = m_jumpscareDirs.size() - 1;
        else
            index--;

        m_currentJumpscare = m_jumpscareDirs[index].string();
        this->getChildByID("button-menu")->getChildByType<TextInput>(0)->setString(m_jumpscareDirs[index].filename().string());
        // this->dispatchChanged();
    }

public:
    // to save the setting
    void onCommit() {}
    void onResetToDefault() {}
    void commit() const {
        m_value->setJumpscare(m_currentJumpscare);
        // this->dispatchCommitted();
    }

    bool hasUncommittedChanges() const {
        return m_currentJumpscare != m_value->getJumpscare();
    }

    bool hasNonDefaultValue() const {
        return m_value->getJumpscare() != defaultJumpscare;
    }

    void resetToDefault() {
        m_currentJumpscare = defaultJumpscare;
    }

    static JumpscareNode* create(std::shared_ptr<JumpscareValue> value, float width) {
        auto ret = new JumpscareNode();
        if (ret && ret->init(value, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

SettingNodeV3* JumpscareValue::createNode(float width) {
    return JumpscareNode::create(
        std::static_pointer_cast<JumpscareValue>(shared_from_this()),
        width
    );
}


$on_mod(Loaded) {
    Mod::get()->registerCustomSettingType("jumpscare_in_use", &JumpscareValue::parse);
}