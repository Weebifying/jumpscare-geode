#include <Geode/Geode.hpp>
#include "utils.hpp"

using namespace geode::prelude;

std::vector<fs::path> getJumpscareSubDir(fs::path path) {
	std::vector<fs::path> ret;
    fs::path configDir = Mod::get()->getConfigDir();
    fs::path resourcesDir = Mod::get()->getResourcesDir();

	for (auto& child : fs::directory_iterator(path)) 
		if (fs::is_directory(child)) 
			if (fs::exists(child.path() / "jumpscare.png") && fs::exists(child.path() / "jumpscareAudio.mp3")) 
				ret.push_back(child.path());

    if (ret.size() == 0) {
        std::vector<fs::path> ret;

        if (!fs::exists(configDir / "jumpscare")) 
            fs::create_directory(configDir / "jumpscare");
        
        if (!fs::exists(configDir / "jumpscare" / "jumpscare.png")) {
            if (fs::exists(configDir / "jumpscare.png")) 
                fs::rename(configDir / "jumpscare.png", configDir / "jumpscare" / "jumpscare.png");
            else
                fs::copy(resourcesDir / "jumpscare.png", configDir / "jumpscare" / "jumpscare.png");
        }
        
        if (!fs::exists(configDir / "jumpscare" / "jumpscareAudio.mp3")) {
            if (fs::exists(configDir / "jumpscareAudio.mp3"))
                fs::rename(configDir / "jumpscareAudio.mp3", configDir / "jumpscare" / "jumpscareAudio.mp3");
            else
                fs::copy(resourcesDir / "jumpscareAudio.mp3", configDir / "jumpscare" / "jumpscareAudio.mp3");
        }

        ret.push_back(configDir / "jumpscare");
    }
			
	return ret;
}