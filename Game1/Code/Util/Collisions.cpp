#include <Util/Collisions.h>
/*
 * (C) Copytight 2017 Marek Bielawski
 * Util::Collisions
 */
#include <Util/DebugDraw.h>
namespace Util {


bool Collisions::testObbObb(
	const vmath::Matrix4 &wpos1, const vmath::Vector3& extend1,
	const vmath::Matrix4 &wpos2, const vmath::Vector3& extend2 )
{
	vmath::Matrix4 pos2InSpace1 = vmath::affineInverse( wpos1 ) * wpos2;
	vmath::Matrix3 orient = pos2InSpace1.getUpper3x3();
	vmath::Vector3 t = pos2InSpace1.getTranslation();
	vmath::Vector3 x = orient.getCol0();
	vmath::Vector3 y = orient.getCol1();
	vmath::Vector3 z = orient.getCol2();
	if( dot(x,t) > .0f ) x = -x;
	if( dot(y,t) > .0f ) y = -y;
	if( dot(z,t) > .0f ) z = -z;
	vmath::Vector3 extendOffset = extend1;
	if(t.getX() < .0f ) extendOffset.setX( extendOffset.getX() * -1.0f);
	if(t.getY() < .0f ) extendOffset.setY( extendOffset.getY() * -1.0f);
	if(t.getZ() < .0f ) extendOffset.setZ( extendOffset.getZ() * -1.0f);
	t += extend2.getX() * x + extend2.getY() * y + extend2.getZ() * z; 
	t -=extendOffset;
	float dotx = dot(x,t);
	float doty = dot(y,t);
	float dotz = dot(z,t);
	return (dotx > .0f && doty > .0f && dotz > .0f);
}

bool Collisions::testObbPoint( 
	const vmath::Matrix4 &wpos, const vmath::Vector3& extend, const vmath::Vector3& position)
{
	vmath::Vector3 localPos = (vmath::affineInverse(wpos) * vmath::Point3(position) ).getXYZ();
	localPos = vmath::absPerElem( localPos );
	return 
		localPos.getX() < extend.getX() && 
		localPos.getY() < extend.getY() && 
		localPos.getZ() < extend.getZ();
}

}
