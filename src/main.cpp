#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/LoadingLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>

#include <random>
#include "utils.hpp"
#include "JumpscareValue.hpp"

using namespace geode::prelude;
namespace fs = std::filesystem;

CCSprite* jumpscare = NULL;
CCSprite* background = NULL;
int currentSecond = 0;
int currentMilisecond = 0;


$on_mod(Loaded) {
	fs::path configDir = Mod::get()->getConfigDir();
	fs::path resourcesDir = Mod::get()->getResourcesDir();

	std::vector<fs::path> jumpscareDirs = getJumpscareSubDir(configDir);
	
	if (!fs::exists(configDir / "background.png"))
		fs::copy(resourcesDir / "background.png", configDir / "background.png");
}

class $modify(AltPlayerObject, PlayerObject) {
	
	void removeJumpscare() {
		const auto runningScene = CCDirector::get()->getRunningScene();
		// only set invisible if the sprite is already in the scene
		if (runningScene->getChildByID("jumpscare")) {
			runningScene->removeChild(background);
			runningScene->removeChild(jumpscare);
		}
	}

	void playerDestroyed(bool p0) {
    	PlayerObject::playerDestroyed(p0);

		fs::path configDir = Mod::get()->getConfigDir();
		fs::path resourcesDir = Mod::get()->getResourcesDir();
		fs::path dir;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::vector<fs::path> jumpscareDirs = getJumpscareSubDir(configDir);

		if (!fs::exists(configDir / "background.png"))
			fs::copy(resourcesDir / "background.png", configDir / "background.png");

		if (Mod::get()->getSettingValue<bool>("randomize")) {
			std::vector<fs::path> chosenDir(1);
			std::sample(jumpscareDirs.begin(), jumpscareDirs.end(), chosenDir.begin(), 1, gen);
			dir = chosenDir.front();
		} else {
			dir = as<JumpscareValue*>(Mod::get()->getSetting("jumpscare_in_use"))->getJumpscare();
		}
		
		// check if player is NOT in level editor
		if (!PlayLayer::get()) return;
		
		// check if player is not a ghost (globed and such)
		if (this != PlayLayer::get()->m_player1 && this != PlayLayer::get()->m_player2) return;

		// probability check
		auto chance = Mod::get()->getSettingValue<double>("chance");
		std::uniform_real_distribution<float> distrib(0.f, 100.f);
		if (distrib(gen) > chance) return;

		// after percentage/time check
		if (PlayLayer::get()->m_level->isPlatformer()) {
			// log::info("{}.{}s", currentSecond, currentMilisecond);
			if (currentSecond + currentMilisecond/100 < Mod::get()->getSettingValue<double>("from_time")) return;
		} else {
			// log::info("{}%", PlayLayer::get()->getCurrentPercent());
			if (PlayLayer::get()->getCurrentPercent() < Mod::get()->getSettingValue<double>("from_percent")) return;
		}
		
		// only from 0 check
		if (Mod::get()->getSettingValue<bool>("only_from_0"))
			if (PlayLayer::get()->m_isPracticeMode or PlayLayer::get()->m_isTestMode) return;

		// thanks nicknamegg
		const auto runningScene = CCDirector::get()->getRunningScene();
		auto winSize = CCDirector::get()->getWinSize();

		if (!runningScene->getChildByID("jumpscare")) {
			jumpscare = CCSprite::create((dir / "jumpscare.png").string().c_str());
			jumpscare->setID("jumpscare");

			jumpscare->setPosition({winSize.width / 2, winSize.height / 2});
		}

		if (!runningScene->getChildByID("jumpscare-background")) {
			background = CCSprite::create((configDir / "background.png").string().c_str());
			background->setID("jumpscare-background");
			// scale to screen
			background->setScaleX(winSize.width / background->getContentSize().width);
			background->setScaleY(winSize.height / background->getContentSize().height);

			background->setPosition({winSize.width / 2, winSize.height / 2});
		}

		// scale is to make it fit to screen but
		// i dont even fucking know anymore c++ is so stupid
		auto yscale = winSize.height / jumpscare->getContentSize().height;
		auto xscale = winSize.width / jumpscare->getContentSize().width;
		float scale;
		if (xscale < yscale) scale = xscale;
		else scale = yscale;

		if (!runningScene->getChildByID("jumpscare-background")) runningScene->addChild(background, 8);
		if (!runningScene->getChildByID("jumpscare")) runningScene->addChild(jumpscare, 9);

		jumpscare->setScale(1);
		jumpscare->runAction(CCScaleBy::create(0.2, scale))->setTag(1);	

		if (!Mod::get()->getSettingValue<bool>("disable_blink"))
			jumpscare->runAction(CCBlink::create(0.5, 10))->setTag(2);

		// thanks zmx
		auto fmae = FMODAudioEngine::sharedEngine();
		auto system = fmae->m_system;

		FMOD::Channel* channel;
		FMOD::Sound* sound;

		// fmod functions return a FMOD_RESULT enum type instead, so the actual return is passed as the last argument of the func
		system->createSound((dir / "jumpscareAudio.mp3").string().c_str(), FMOD_DEFAULT, nullptr, &sound);
		system->playSound(sound, nullptr, false, &channel);

		if (!Mod::get()->getSettingValue<bool>("full_volume"))
			channel->setVolume(fmae->getEffectsVolume());
		
		
		jumpscare->runAction(
			CCSequence::create(
				CCDelayTime::create(1.0),
				CCCallFunc::create(runningScene, callfunc_selector(AltPlayerObject::removeJumpscare)),
				nullptr
			)
		)->setTag(3);
	}
};

// clears the jumpscare sprite when the player respawns
class $modify(PlayLayer) {
	void resetLevel() {
		PlayLayer::resetLevel();
		const auto runningScene = CCDirector::get()->getRunningScene();
		// only set invisible if the sprite is already in the scene
		if (runningScene->getChildByID("jumpscare")) {
			runningScene->removeChild(background);
			runningScene->removeChild(jumpscare);	
		}
	}

	void updateTimeLabel(int p0, int p1, bool p2) {
		PlayLayer::updateTimeLabel(p0, p1, p2);
		currentSecond = p0;
		currentMilisecond = p1;
		// log::debug("updateTimeLabel({}, {}, {}) called!", p0, p1, p2);
	}
};


// jumpscare animation pauses when opening pause layer
// and resumes when closing
class $modify(MyPauseLayer, PauseLayer) {
	void onButton(CCObject*) {
		openSettingsPopup(Mod::get());
	}

	void customSetup() {
		PauseLayer::customSetup();

		const auto runningScene = CCDirector::get()->getRunningScene();
		if (runningScene->getChildByID("jumpscare")) {
			if (jumpscare->getActionByTag(1)) CCDirector::get()->getActionManager()->pauseTarget(jumpscare);
			if (jumpscare->getActionByTag(2)) CCDirector::get()->getActionManager()->pauseTarget(jumpscare);
			if (jumpscare->getActionByTag(3)) CCDirector::get()->getActionManager()->pauseTarget(jumpscare);
		}

		if (Mod::get()->getSettingValue<bool>("button")) {
			if (auto menu = this->getChildByID("right-button-menu")) {
				auto sprite = CCSprite::create("GJ_jumpscareBtn_001.png"_spr);
				sprite->setScale(0.8f);

				auto button = CCMenuItemSpriteExtra::create(
					sprite,
					this,
					menu_selector(MyPauseLayer::onButton)
				);

				menu->addChild(button);
				menu->updateLayout();
			}
		}
	}

	void onResume(CCObject* sender) {
		PauseLayer::onResume(sender);

		const auto runningScene = CCDirector::get()->getRunningScene();
		if (runningScene->getChildByID("jumpscare")) {
			if (jumpscare->getActionByTag(1)) CCDirector::get()->getActionManager()->resumeTarget(jumpscare);
			if (jumpscare->getActionByTag(2)) CCDirector::get()->getActionManager()->resumeTarget(jumpscare);
			if (jumpscare->getActionByTag(3)) CCDirector::get()->getActionManager()->resumeTarget(jumpscare);
		}
	}
};
