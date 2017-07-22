#include "Character.h"
#include <Game/Transform.h>
#include <Game/Scene.h>
#include <Game/Camera.h>
#include <Black/GroundManager.h>
#include <Black/CharacterObject.h>
#include <Black/EventSystem.h>
#include <Black/TreeMesh.h>
#include <Util/RandomGenerator.h>
#include <Util/DebugDraw.h>

#include <Black/Ground.h>
#include <Sound/System.h>
#include <Sound/Buffer.h>
#include <Game/App.h>
#include <Input/Listener.h>
#include <Input/Constants.h>
#include <Black/Editor/Editor.h>


static Black::Character* __instance = nullptr;

Black::Character::Character(Game::Scene * scene, const char * name)
	: scene_(scene)
{
	transform_ = new Game::Transform(name);
	mesh_ = new CharacterObject(name, "assets/gfx/woman.jpg", 100);
	mesh_->setLayer(100);
	transform_->carryObject(mesh_);
	scene_->addTransform(transform_);
	scene_->addUpdater(this);

	char camName[200];
	sprintf_s(camName, "%sCamera", name);
	cam_.transform_ = new Game::Transform(camName);
	scene_->addTransform(cam_.transform_);
	cam_.cam_ = scene_->createOrtographicCamera(cam_.transform_->getHashName(), 2.0f);
	cam_.cam_->setXWrap( GroundManager::WorldWrap );
	scene_->setActiveCamera(cam_.transform_->getHashName());
	__instance = this;
}

void Black::Character::start()
{
	position_ = vmath::Vector3(0.0f, 3.0f, .0f);
	hair_.init(this);
}

void Black::Character::update(const Game::UpdateContext * uctx)
{
	float dt = uctx->deltaTime;
	//maxSpeed_ = targetMaxSpeed_;
	if (targetMaxSpeed_ > maxSpeed_)
	{
		maxSpeed_ = min( maxSpeed_ + 2.0f * dt, targetMaxSpeed_ );
	}
	else if (targetMaxSpeed_ < maxSpeed_)
	{
		maxSpeed_ = max(maxSpeed_ - 2.0f * dt, targetMaxSpeed_ );
	}
	if (dt > .5f)
	{
		dt = .5f;
	}
	input_.feed(uctx->input, dt);
	if( Game::App::EditorEnabled() && Editor::Editor::IsInEditionMode() )
		return;
	if (onGround_)
	{
		if (rundownProgress_ > .001f || runningDown_)
		{
			_UpdateRunningDown(dt, input_.leftX_, input_.jump);
		}
		else
		{
			_UpdateWalking(dt, input_.leftX_, input_.jump);
		}
		if (Game::App::EditorEnabled())
		{
			Editor::Editor::SetLayerID(GroundManager::getInstance()->getLayerIndex());
			Editor::Editor::SetPosition(position_);
		}
	}
	else
	{
		_UpdateFlying(dt, input_.leftX_, input_.jump, input_.leftY_);
	}
	if (position_.getX() < -420.0f)
	{
		position_.setX(position_.getX() + 800.0f);
		cam_.pos_.setX(cam_.pos_.getX() + 800.0f);
	}
	else if (position_.getX() > 420.0f)
	{
		position_.setX(position_.getX() - 800.0f);
		cam_.pos_.setX(cam_.pos_.getX() - 800.0f);
	}

	hair_.update(dt);
	transform_->setTranslation(position_);
	walkDirSmooth_ += (walkDir_ - walkDirSmooth_) * dt * 8.0f;

	mesh_->updateNoise(dt);
	_UpdateMesh();

	waveCenterLowPass_ +=  ( position_ - waveCenterLowPass_ ) * dt * 3.2f;
	vmath::Vector3 waveCenter = waveCenterLowPass_ + vmath::Vector3( .0f, .6f, .0f) ;
	waveLowPas_ += ( lengthSqr( velocity_ ) - waveLowPas_ ) * dt * .2f;

	waveCenter.setZ( waveLowPas_ * .48f);
	Game::App::Instance()->setWaveCenter( waveCenter);
	cam_._Update(dt, position_);


	EventSystem* evtSystem = EventSystem::GetInstance();
	GroundManager* gm = GroundManager::getInstance();
	evtSystem->feedPosition(position_, gm->getLayerIndex(), gm->getLevelId());
}

void Black::Character::stop()
{
}

Black::Character * Black::Character::GetInstance()
{
	return __instance;
}

void Black::Character::teleport(unsigned int layerId, float x)
{
	GroundManager* gm = GroundManager::getInstance();
	gm->setLayer(layerId);
	Ground* ground = gm->getGround(layerId);
	vmath::Vector3 newPos = vmath::Vector3(x, ground->getTopHeight(x), .0f);
	position_ = newPos;
}
void Black::Character::setVisible(bool val)
{
	mesh_->setVisible(val);
}

void Black::Character::_UpdateMesh()
{
	mesh_->clearAnim();
	mesh_->blendInWalk(walkPhase_, walkDirSmooth_);
	mesh_->blendInJump(walkPhase_, jumpBlend_);
	mesh_->setFlip(walkDir_ < .0f);
}

void Black::Character::_Camera::_Update(float dt, const vmath::Vector3& pos)
{
	vmath::Vector3 diff = pos + vmath::Vector3(.0, .9f, .0f) - pos_;
	float extend = length(diff) * 1.2f + 2.1f;
	extend_ += (extend - extend_ )* (1.0f * dt);
	extend_ = sqrt(extend_) * 3.0f;
	diff = Util::limitVector3(diff, 20.0f);
	vmath::Vector3 force = diff;
	if (lengthSqr(force) < 1.0f)
	{
		force = vmath::Vector3( .0f );
	}
	vel_ += force * dt * 10.0f;
	vel_ *= max(1.0f - damping_ * dt, .5f);
	vel_ = Util::limitVector3(vel_, maxSpeed_);
	pos_ += vel_ * dt;

	transform_->setTranslation(pos_);
	cam_->setExtend(5.0f);
 }

void Black::Character::_UpdateWalking(float dt, float left, bool jumpButton )
{
	jumpBlend_ = max(jumpBlend_ - dt, .0f);
	vmath::Vector3 force = left * 50.0f * lastGroundTangent_;//(left * 50.0f, .0f, .0f);
	float velX = dot(velocity_, lastGroundTangent_);
	walkDir_ = Util::clamp(velX * 4.2f, -1.0f, 1.0f);
	float prevWalkPhase = walkPhase_;
	walkPhase_ += abs(velX) * 3.0f * dt;
	if (walkPhase_ > 6.283f)
		walkPhase_ -= 6.283f;

	const float step1 = 1.0f;
	const float step2 = 4.14f;
	if (prevWalkPhase < step1 && walkPhase_ > step1)
	{
		Sound::playSound("bank.bnk", "step", 1.0f, -0.2f);
	}
	else if(prevWalkPhase < step2 && walkPhase_ > step2)
	{
		Sound::playSound("bank.bnk", "step", 1.0f, 0.2f);
	}
	


	Ground* ground = GroundManager::getInstance()->getCurrentGround();
	vmath::Vector3 velocityX = velocity_;
	velocityX += force * dt;
	velocityX -= dot(lastGroundNormal_, velocityX) * lastGroundNormal_;
	float speedLimit = maxSpeed_ * 1.0f / .707f;
	vmath::Vector3 slopeOffset = vmath::Vector3(.0f, .707f * maxSpeed_, .0f);
	velocityX = Util::limitVector3(velocityX + slopeOffset, speedLimit) - slopeOffset;
	velocityX *= fmaxf(1.0f - groundSpeedDamping_ * dt, .5f);
	position_ += velocityX * dt;
	float tx1 = position_.getX() + .02f;
	float tx0 = position_.getX() - .02f;
	float ty1 = ground->getTopHeight(tx1);
	float ty0 = ground->getTopHeight(tx0);
	lastGroundTangent_ = normalize(vmath::Vector3(tx1 - tx0, ty1 - ty0, .0f));
	lastGroundNormal_ = normalize(vmath::Vector3(-lastGroundTangent_.getY(), lastGroundTangent_.getX(), .0f));
	velocity_ = velocityX;
	position_.setY(ground->getTopHeight(position_.getX()));
	if (jumpButton)
	{
		switchedPlane_ = false;
		onGround_ = false;
		Sound::playSound("bank.bnk", "jump");
		jumpSoundId_= Sound::playSound("bank.bnk", "noiseLow", 1.0f, .0f);
		jumpTimer_ = .05f;
	}
}

void Black::Character::_UpdateRunningDown(float dt, float left, bool jumpButton)
{
	if (runningDown_)
	{
		rundownProgress_ = min(rundownProgress_ + dt * .5f, 1.0f);
	}
	else
	{
		rundownProgress_ = max(rundownProgress_ - dt * .5f, .0f);
	}
	//bool runningDown_ = false;
	if (walkDir_ > -.01f && walkDir_ < .01f)
	{
		_UpdateWalking(dt, left, jumpButton);
		return;
	}
	else
	{
		float dir = (walkDir_ > .0f) ? 1.0f : -1.0f;
		jumpTimer_ = .0f;
		_UpdateWalking(dt, dir, false);
		Ground* ground = GroundManager::getInstance()->getCurrentGround();
		position_.setY(ground->getTopHeight(position_.getX()) - rundownProgress_ * 2.0f);
	}
}


void Black::Character::_UpdateFlying(float dt, float left, bool jumpButton, float up)
{
	jumpBlend_ = min(jumpBlend_ + dt * 4.0f, 1.0f);
	Ground* ground = GroundManager::getInstance()->getCurrentGround();
	vmath::Vector3 force = vmath::Vector3(left * 20.0f, -gravity_, .0f);
	
	if (jumpTimer_ > .00001f )
	{
		if( jumpButton )
		force += vmath::Vector3(.0f, 8.0f * gravity_, .0f);
		jumpTimer_ = max(jumpTimer_ - dt, .0f);
		
	}
	if(!switchedPlane_ && switchPlaneAllowed_)
	{
		{
			if( GroundManager::getInstance()->switchToNextLowerGround(position_.getX(), position_.getY(), -up) )
			{
				switchedPlane_ = true;
			}

		}
	}
	velocity_ += force * dt;
	vmath::Vector3 velX = velocity_;
	vmath::Vector3 velY = velocity_;
	velX.setY(.0f);
	velY.setX(.0f);

	velocity_ = Util::limitVector3(velX, maxSpeed_) + Util::limitVector3(velY, maxSpeed_ * 16);
	position_ += velocity_ * dt;

	lastGroundTangent_ = normalize(lastGroundTangent_ + vmath::Vector3(dt * 3.0f, .0f, .0f));
	lastGroundNormal_ = normalize(vmath::Vector3(-lastGroundTangent_.getY(), lastGroundTangent_.getX(), .0f));
	float height = ground->getTopHeight(position_.getX() );
	if (height > position_.getY() && jumpTimer_ < .05f )
	{
		onGround_ = true;
		//Sound::playSound("bank.bnk", "land");
		if (jumpSoundId_)
		{
			Sound::stopSound(jumpSoundId_);
		}
	}
}

void Black::Character::_Hair::init(Character* thisNode)
{
	thisNode_ = thisNode;
	Util::RandomGenerator gen;
	for (unsigned int i = 0; i < eHairCount; ++i)
	{
		float primaryAngle = gen.getFloat(-1.0f, 1.0f );
		start_[i] = thisNode_->headCenter_ + vmath::Matrix3::rotationZ(primaryAngle) * vmath::Vector3(.0f, .12f, .0f);
		nextPointBase_[i] = start_[i] + vmath::Matrix3::rotationZ(gen.getFloat(.0f, 0.6f) + primaryAngle * .5f ) * vmath::Vector3(.0f, .1f, .0f);
		lastPointBase_[i] = nextPointBase_[i] + vmath::Matrix3::rotationZ(gen.getFloat(-1.5f, -.9f)) * vmath::Vector3(.0f, .1f, .0f);
		offset1_[i] = gen.getVector3(maxOffset_ * .25f);
		offset2_[i] = gen.getVector3(maxOffset_ * .5f);
		velocity1_[i] = gen.getVector3(maxOffset_ * .25f);
		velocity2_[i] = gen.getVector3(maxOffset_ * .25f);
		forceAngle_[i] = gen.getFloat(-.7f, .7f);
		damping_[i] = gen.getFloat(0.1f, 0.3f);
		massInv_[i] = gen.getFloat(20.3f, 80.0f);
	}
}

void Black::Character::_Hair::update(float dt)
{
	



	float maxOffset05 = maxOffset_  *.5f;
	vmath::Vector3 vel05 = .5f * thisNode_->velocity_;
	vmath::Vector3 vel = thisNode_->velocity_;
	for (unsigned int i = 0; i < eHairCount; ++i)
	{
		vmath::Vector3 force1 = 50.0f * ( -offset1_[i] - massInv_[i] * vmath::rotate( vmath::Quat::rotationZ(forceAngle_[ i ] ), vel05 ) );
		vmath::Vector3 force2 = 50.0f * ( -offset2_[i] - massInv_[i] * vmath::rotate(vmath::Quat::rotationZ(forceAngle_[i]), vel) );
		velocity1_[i] = Util::limitVector3(velocity1_[i] + force1 * dt, maxOffset05);
		velocity2_[i] = Util::limitVector3(velocity2_[i] + force2 * dt, maxOffset_);
		velocity1_[i] *= fmaxf(1.0f - dt * damping_[i], .5f);
		velocity2_[i] *= fmaxf(1.0f - dt * damping_[i], .5f);
		offset1_[i] = Util::limitVector3(offset1_[i] + velocity1_[i] * dt, maxOffset05);
		offset2_[i] = Util::limitVector3(offset2_[i] + velocity2_[i] * dt, maxOffset_);
	}
}
