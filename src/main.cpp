#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/LoadingLayer.hpp>


using namespace geode::prelude;

CCSprite* jumpscare = NULL;
CCSprite* background = NULL;
int currentSecond = 0;
int currentMilisecond = 0;

$on_mod(Loaded) {
	srand((unsigned int)time(NULL));
	
	std::filesystem::path configDir = Mod::get()->getConfigDir().string();
	std::filesystem::path resourcesDir = Mod::get()->getResourcesDir().string();
	if (!std::filesystem::exists(configDir / "jumpscare.png")) {
		std::filesystem::copy(resourcesDir / "jumpscare.png", configDir / "jumpscare.png");
	}
	if (!std::filesystem::exists(configDir / "background.png")) {
		std::filesystem::copy(resourcesDir / "background.png", configDir / "background.png");
	}
	if (!std::filesystem::exists(configDir / "jumpscareAudio.mp3")) {
		std::filesystem::copy(resourcesDir / "jumpscareAudio.mp3", configDir / "jumpscareAudio.mp3");
	}
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

	TodoReturn playerDestroyed(bool p0) {
    	PlayerObject::playerDestroyed(p0);
		
		std::filesystem::path configDir = Mod::get()->getConfigDir().string();

		// check if player is NOT in level editor
		if (!PlayLayer::get()) return;

		// probability check
		auto chance = Mod::get()->getSettingValue<double>("chance");
		if (rand()/(RAND_MAX+1.0) > chance/100) return;

		// after percentage/time check
		if (PlayLayer::get()->m_level->isPlatformer()) {
			log::info("{}.{}s", currentSecond, currentMilisecond);
			if (currentSecond + currentMilisecond/100 < Mod::get()->getSettingValue<double>("from_time")) return;
		} else {
			log::info("{}%", PlayLayer::get()->getCurrentPercent());
			if (PlayLayer::get()->getCurrentPercent() < Mod::get()->getSettingValue<double>("from_percent")) return;
		}
		
		// only from 0 check
		if (Mod::get()->getSettingValue<bool>("only_from_0"))
			if (PlayLayer::get()->m_isPracticeMode or PlayLayer::get()->m_isTestMode) return;

		// thanks nicknamegg
		const auto runningScene = CCDirector::get()->getRunningScene();
		auto winSize = CCDirector::get()->getWinSize();

		if (!runningScene->getChildByID("jumpscare")) {
			jumpscare = CCSprite::create((configDir / "jumpscare.png").string().c_str());
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

		runningScene->addChild(background, 8);
		runningScene->addChild(jumpscare, 9);

		jumpscare->setScale(1);
		jumpscare->runAction(CCScaleBy::create(0.2, scale))->setTag(1);	
		jumpscare->runAction(CCBlink::create(0.5, 10))->setTag(2);

		// fucking works now thanks dank_meme
		Loader::get()->queueInMainThread([configDir] {
			FMODAudioEngine::sharedEngine()->playEffect((configDir / "jumpscareAudio.mp3").string().c_str());
		});

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

	// void resetLevel() {
	// 	PlayLayer::resetLevel();
	// 	const auto runningScene = CCDirector::get()->getRunningScene();
	// 	// only set invisible if the sprite is already in the scene
	// 	if (runningScene->getChildByID("jumpscare")) {
	// 		runningScene->removeChild(background);
	// 		runningScene->removeChild(jumpscare);	
	// 	}
	// }

	void updateTimeLabel(int p0, int p1, bool p2) {
		PlayLayer::updateTimeLabel(p0, p1, p2);
		currentSecond = p0;
		currentMilisecond = p1;
		// log::debug("updateTimeLabel({}, {}, {}) called!", p0, p1, p2);
	}
};


// jumpscare animation pauses when opening pause layer
// and resumes when closing
class $modify(PauseLayer) {
	TodoReturn customSetup() {
		PauseLayer::customSetup();

		const auto runningScene = CCDirector::get()->getRunningScene();
		if (runningScene->getChildByID("jumpscare")) {
			if (jumpscare->getActionByTag(1)) CCDirector::get()->getActionManager()->pauseTarget(jumpscare);
			if (jumpscare->getActionByTag(2)) CCDirector::get()->getActionManager()->pauseTarget(jumpscare);
			if (jumpscare->getActionByTag(3)) CCDirector::get()->getActionManager()->pauseTarget(jumpscare);
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
