#include <Geode/Geode.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/ui/TextInput.hpp>

using namespace geode::prelude;

struct JumpscareValueStruct {
    std::string jumpscare;

    // all of this is copied from colon, who copied it from the docs, because neither i nor colon had any idea what needed to be done
    bool operator==(JumpscareValueStruct const& other) const = default;
    operator std::string() const { return jumpscare; }

    JumpscareValueStruct() = default;
    JumpscareValueStruct(std::string jumpscare) : jumpscare(jumpscare) {}
    JumpscareValueStruct(JumpscareValueStruct const&) = default;

    std::string getJumpscareValue() const { return jumpscare; }
};

class JumpscareValue : public SettingBaseValueV3<std::string> {
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
        if (json.asString().isErr()) return false;
        m_jumpscare = json.asString().unwrap();
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

    std::string getJumpscare() {
        return m_jumpscare;
    }

    bool isDefaultValue() const override {
        return m_jumpscare == (Mod::get()->getConfigDir() / "jumpscare").string();
    }

    void reset() override {
        m_jumpscare = (Mod::get()->getConfigDir() / "jumpscare").string();
    }
};

template <>
struct matjson::Serialize<JumpscareValueStruct> {
    static matjson::Value toJson(const JumpscareValueStruct& settingValue) { return settingValue.jumpscare; }
    static Result<JumpscareValueStruct> fromJson(const matjson::Value& json) {
        GEODE_UNWRAP_INTO(auto seymourSkinner, json.asString());
        return Ok(JumpscareValueStruct(seymourSkinner));
    }
};

template <>
struct geode::SettingTypeForValueType<JumpscareValueStruct> {
    using SettingType = JumpscareValue;
};