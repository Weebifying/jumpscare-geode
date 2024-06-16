#include <Geode/Geode.hpp>
#include <Geode/loader/SettingNode.hpp>
using namespace geode::prelude;


class OpenConfigValue : public SettingValue {
protected:
    std::string m_placeholder;
public:
    OpenConfigValue(std::string const& key, std::string const& modID, std::string const& placeholder)
      : SettingValue(key, modID), m_placeholder(placeholder) {}

    bool load(matjson::Value const& json) override {
        return true;
    }
    bool save(matjson::Value& json) const override {
        return true;
    }
    SettingNode* createNode(float width) override;
};

class OpenConfigNode : public SettingNode {
protected:
    bool init(OpenConfigValue* value, float width) {
        if (!SettingNode::init(value))
            return false;

        this->setContentSize({ width, 40.f });

        auto menu = CCMenu::create();
        menu->setPosition(width - 20.f, 20.f);
        menu->setID("button-menu");
        this->addChild(menu);

        auto label = CCLabelBMFont::create("Open config folder", "bigFont.fnt");
        label->setAnchorPoint({0.f, 0.5f});
        label->setPosition(20.f, 20.f);
        label->setScale(0.45f);
        this->addChild(label);

        auto spr = CCSprite::createWithSpriteFrameName("folderIcon_001.png");
        spr->setScale(0.8f);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(OpenConfigNode::onOpenConfig));
        btn->setPosition(-10.f, 0.f);
        menu->addChild(btn);

        return true;
    }

public:
    void onOpenConfig(CCObject*);

    void commit() override {
        this->dispatchCommitted();
    }

    bool hasUncommittedChanges() override {
        return false;
    }

    bool hasNonDefaultValue() override {
        return true;
    }

    void resetToDefault() override {

    }
    static OpenConfigNode* create(OpenConfigValue* value, float width) {
        auto ret = new OpenConfigNode;
        if (ret && ret->init(value, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

SettingNode* OpenConfigValue::createNode(float width) {
    return OpenConfigNode::create(this, width);
}

void OpenConfigNode::onOpenConfig(CCObject*) {
    log::warn("{}", utils::file::openFolder(Mod::get()->getConfigDir()));
}

$on_mod(Loaded) {
	Mod::get()->addCustomSetting<OpenConfigValue>("open_config", "none");
}