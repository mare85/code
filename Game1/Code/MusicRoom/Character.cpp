#include "Character.h"
#include <Game/Scene.h>
#include <Game/Transform.h>
#include <Game/Camera.h>
#include <Input/Listener.h>


void MusicRoom::Character::start()
{
	position_ = vmath::Vector3(.0f, 1.6f, .0f);
	pitch_ = .0f;
	yaw_ = .0f;
}

void MusicRoom::Character::update(const Game::UpdateContext* uctx)
{
	Input::Listener* input = uctx->input;
	yaw_ -= input->getMouseDeltaX() * .01f;
	if( yaw_ > 3.141592f )
	{
		yaw_ -= 6.283184f;
	}
	else if( yaw_ < -3.141592f )
	{
		yaw_ += 6.283184f;
	}
	pitch_ -= input->getMouseDeltaY() * .01f;
	pitch_ = max( min( pitch_ , 1.5f ), -1.5f );
	vmath::Quat yawQuat = vmath::Quat::rotationY( yaw_ );
	vmath::Quat pitchQuat = vmath::Quat::rotationX( pitch_ );
	vmath::Vector3 front = vmath::rotate( vmath::Quat::rotationY( yaw_ ), vmath::Vector3( .0f, .0f, -1.0f ) );
	vmath::Vector3 side = vmath::rotate( vmath::Quat::rotationY( yaw_ ), vmath::Vector3( 1.0f, .0f, .0f ) );
	if( input->isKeyPressed( Input::Key::W ) )
	{
		position_ += front * uctx->deltaTime * 3.0f;
	}
	if (input->isKeyPressed(Input::Key::S))
	{
		position_ -= front * uctx->deltaTime* 3.0f;
	}
	if (input->isKeyPressed(Input::Key::A))
	{
		position_ -= side * uctx->deltaTime * 2.0f;
	}
	if (input->isKeyPressed(Input::Key::D))
	{
		position_ += side * uctx->deltaTime * 2.0f;
	}

	float x = position_.getX();
	if( x < -16.0f )
	{
		x += 16.0f;
		position_.setX( x );
	}
	else if( x > 16.0f)
	{
		x -= 16.0f;
		position_.setX( x );
	}
	float z = position_.getZ();
	z = max( min( z, 1.7f ), -1.7f );
	position_.setZ( z );

	rootTransform_->setTranslation( position_ );
	rootTransform_->setRotation( yawQuat );
	cameraTransform_->setRotation( pitchQuat );
	front_ = -normalize( cameraTransform_->getWorldPose().getCol2().getXYZ() );
}

void MusicRoom::Character::stop()
{

}

MusicRoom::Character::Character(Game::Scene* scene, const char* name)
	: scene_(scene)
{
	rootTransform_ = new Game::Transform(name);
	scene_->addTransform(rootTransform_);

	char camName[200];
	sprintf_s(camName, "%sCamera", name);
	cameraTransform_ = new Game::Transform(camName);
	scene_->addTransform(cameraTransform_, rootTransform_);
	cam_ = scene_->createPersp(cameraTransform_->getHashName(), .75f);
	cam_->setNearFar( .1f, 100.0f );
	scene_->setActiveCamera(cameraTransform_->getHashName());

	scene_->addUpdater(this);
}
