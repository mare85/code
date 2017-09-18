#include <GameUtils/FreeRoamCamera.h>
/*
 * (C) Copytight 2017 Marek Bielawski
 * GameUtils::FreeRoamCamera
 */
#include <Game/Scene.h>
#include <Game/Transform.h>
#include <Game/Camera.h>
#include <Input/Listener.h>
namespace GameUtils {

const float FreeRoamCamera::MaxSpeed = 15.0f;
const float FreeRoamCamera::Acceleration = 60.0f;
const float FreeRoamCamera::Damping = 5.0f;
const float FreeRoamCamera::AngleDamping = 7.0f;
const float FreeRoamCamera::MaxRotSpeed = 10.0f;
const float FreeRoamCamera::AngleAcceleration = 3.f;
	
FreeRoamCamera::FreeRoamCamera( Game::Scene* scene, const char* name)
	: scene_( scene )
{
	rootTransform_ = new Game::Transform(name);
	scene_->addTransform(rootTransform_);

	cam_ = scene_->createPersp(rootTransform_->getHashName(), .75f);
	cam_->setNearFar( .1f, 100.0f );
	scene_->setActiveCamera(rootTransform_->getHashName());

	scene_->addUpdater(this);
}

void FreeRoamCamera::start()
{
	position_ = vmath::Vector3( .0f, .0f, .0f );
	speed_ = vmath::Vector3( .0f );
	pitch_ = .0f;
	yaw_ = .0f;
	pitchSpeed_ = .0f;
	yawSpeed_ = .0f;
}

void FreeRoamCamera::update( const Game::UpdateContext* uctx )
{
	Input::Listener* input = uctx->input;
	pitchSpeed_ *= max( .5f, 1.0f - uctx->deltaTime * AngleDamping );
	yawSpeed_ *= max( .5f, 1.0f - uctx->deltaTime * AngleDamping );
	pitchSpeed_ -= input->getMouseDeltaY() * uctx->deltaTime * AngleAcceleration;
	pitchSpeed_ = max( min( pitchSpeed_, MaxRotSpeed ), -MaxRotSpeed );
	yawSpeed_ -= input->getMouseDeltaX() * uctx->deltaTime * AngleAcceleration;
	yaw_ += yawSpeed_ * uctx->deltaTime;
	pitch_ += pitchSpeed_ * uctx->deltaTime;
	if( yaw_ > 3.141592f )
	{
		yaw_ -= 6.283184f;
	}
	else if( yaw_ < -3.141592f )
	{
		yaw_ += 6.283184f;
	}
	
	pitch_ = max( min( pitch_ , 1.5f ), -1.5f );
	vmath::Quat yawQuat = vmath::Quat::rotationY( yaw_ );
	vmath::Quat pitchQuat = vmath::Quat::rotationX( pitch_ );
	vmath::Vector3 front = vmath::rotate( yawQuat, 
		vmath::rotate( pitchQuat, vmath::Vector3( .0f, .0f, -1.0f ) )
	);

	vmath::Vector3 side = vmath::rotate( yawQuat, vmath::Vector3( 1.0f, .0f, .0f ) );
	vmath::Vector3 top = cross( side, front );

	speed_ *= max( .5f, 1.0f - uctx->deltaTime * Damping );
	if( input->isKeyPressed( Input::Key::W ) )
	{
		speed_ += front * uctx->deltaTime * Acceleration;
	}
	if( input->isKeyPressed( Input::Key::S ) )
	{
		speed_ -= front * uctx->deltaTime * Acceleration;
	}
	if( input->isKeyPressed( Input::Key::A ) )
	{
		speed_ -= side * uctx->deltaTime * Acceleration;
	}
	if( input->isKeyPressed( Input::Key::D ) )
	{
		speed_ += side * uctx->deltaTime * Acceleration;
	}
	if( input->isKeyPressed( Input::Key::Q ) )
	{
		speed_ += top * uctx->deltaTime * Acceleration;
	}
	if( input->isKeyPressed( Input::Key::Z ) )
	{
		speed_ -= top * uctx->deltaTime * Acceleration;
	}
	speed_ = Util::limitVector3( speed_, MaxSpeed );

	position_ += speed_ * uctx->deltaTime;
	rootTransform_->setTranslation( position_ );
	rootTransform_->setRotation( yawQuat * pitchQuat );
	front_ = front;
}

void FreeRoamCamera::stop()
{
}



}
