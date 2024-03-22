#include <Geode/Geode.hpp>

using namespace geode::prelude;

class JumpscareValue : public SettingValue {
protected:
    std::string m_jumpscare;

public:
    JumpscareValue(std::string const& key, std::string const& modID, std::string jumpscare)
        : SettingValue(key, modID), m_jumpscare(jumpscare) {}

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