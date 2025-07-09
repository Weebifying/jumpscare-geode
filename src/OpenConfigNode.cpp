#include <Geode/Geode.hpp>
#include <Geode/loader/SettingV3.hpp>

using namespace geode::prelude;

class OpenConfigValue : public SettingV3 {
protected:
    std::string m_placeholder;
public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
		auto res = std::make_shared<OpenConfigValue>();
		auto root = checkJson(json, "OpenConfigValue");
		res->init(key, modID, root);
		res->parseNameAndDescription(root);
		res->parseEnableIf(root);
		return root.ok(std::static_pointer_cast<SettingV3>(res));
	}
    bool load(matjson::Value const& json) override {
        return true;
    }
    bool save(matjson::Value& json) const override {
        return true;
    }
	bool isDefaultValue() const override {
		return true;
	}
	void reset() override {}
    SettingNode* createNode(float width) override;
};

class OpenConfigNode : public SettingNodeV3 {
protected:
    bool init(OpenConfigValue* value, float width) {
        if (!SettingNodeV3::init(value))
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
	Mod::get()->registerCustomSettingType("open_config", &OpenConfigValue::parse);
}