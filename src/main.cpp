#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

CCSprite* jumpscare = NULL;
CCSprite* background = NULL;

class $modify(PlayerObject) {
	TodoReturn playerDestroyed(bool p0) {
    	PlayerObject::playerDestroyed(p0);

		// probability check
		auto chance = Mod::get()->getSettingValue<double>("chance");
		if (rand()/(RAND_MAX+1.0) > chance/100) return;

		// thanks nicknamegg
		const auto runningScene = CCDirector::get()->getRunningScene();
		auto winSize = CCDirector::get()->getWinSize();

		if (!runningScene->getChildByID("jumpscare")) {
			jumpscare = CCSprite::create("jumpscare.png"_spr);
			jumpscare->setID("jumpscare");

			jumpscare->setPosition({winSize.width / 2, winSize.height / 2});
			runningScene->addChild(jumpscare, 9);
		}

		if (!runningScene->getChildByID("jumpscare-background")) {
			background = CCSprite::create("background.png"_spr);
			background->setID("jumpscare-background");
			// scale to screen
			background->setScaleX(winSize.width / background->getContentSize().width);
			background->setScaleY(winSize.height / background->getContentSize().height);

			background->setPosition({winSize.width / 2, winSize.height / 2});
			runningScene->addChild(background, 8);
		}

		//scale is to make it fit to screen but
		//i dont even fucking know anymore c++ is so fucking stupid
		auto yscale = winSize.height / jumpscare->getContentSize().height;
		auto xscale = winSize.width / jumpscare->getContentSize().width;
		float scale;
		if (xscale < yscale) scale = xscale;
		else scale = yscale;

		background->setOpacity(255);
		jumpscare->setScale(scale);

		jumpscare->setOpacity(255);
		jumpscare->setScale(1);
		jumpscare->runAction(CCScaleBy::create(0.2, scale))->setTag(1);	
		jumpscare->runAction(CCBlink::create(0.5, 10))->setTag(2);

		// fucking works now thanks dank_meme
		Loader::get()->queueInMainThread([] {
			FMODAudioEngine::sharedEngine()->playEffect("jumpscareAudio.mp3"_spr);
		});
	}
};

// clears the jumpscare sprite when the player respawns
class $modify(PlayLayer) {
	// i love mat :pray:
	TodoReturn resetLevel() {
		PlayLayer::resetLevel();
		const auto runningScene = CCDirector::get()->getRunningScene();
		// only set opacity if the sprite is already in the scene
		if (runningScene->getChildByID("jumpscare")) {
			jumpscare->setOpacity(0);
			background->setOpacity(0);	
		}
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
		}
	}

	void onResume(CCObject* sender) {
		PauseLayer::onResume(sender);

		const auto runningScene = CCDirector::get()->getRunningScene();
		if (runningScene->getChildByID("jumpscare")) {
			if (jumpscare->getActionByTag(1)) CCDirector::get()->getActionManager()->resumeTarget(jumpscare);
			if (jumpscare->getActionByTag(2)) CCDirector::get()->getActionManager()->resumeTarget(jumpscare);
		}
	}
};