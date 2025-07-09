#include <Geode/Geode.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/ui/TextInput.hpp>

using namespace geode::prelude;

class JumpscareValue : public SettingV3 {
protected:
    std::string m_jumpscare;

public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
		auto res = std::make_shared<JumpscareValue>();
		auto root = checkJson(json, "JumpscareValue");
		res->init(key, modID, root);
		res->parseNameAndDescription(root);
		res->parseEnableIf(root);
		return root.ok(std::static_pointer_cast<SettingV3>(res));
	}

    bool load(matjson::Value const& json) override {
        if (!json.is<std::string>()) return false;
        m_jumpscare = json.as<std::string>();
        return true;
    }

    bool save(matjson::Value& json) const override {
        json = m_jumpscare;
        return true;
    }

    SettingNode* createNode(float width) override;

    void setJumpscare(std::string jumpscare) {
        m_jumpscare = jumpscare;
    }

    std::string getJumpscare() const {
        return m_jumpscare;
    }
};