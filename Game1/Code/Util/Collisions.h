#ifndef UTIL_COLLISIONS_DEF
#define UTIL_COLLISIONS_DEF
/*
 * (C) Copytight 2017 Marek Bielawski
 * Util::Collisions
 */
#include <Util/Utils.h>
namespace Util {

namespace Collisions
{
	bool testObbObb(
		const vmath::Matrix4 &wpos1, const vmath::Vector3& extend1,
		const vmath::Matrix4 &wpos2, const vmath::Vector3& extend2 );
	bool testObbPoint( 
		const vmath::Matrix4 &wpos, const vmath::Vector3& extend, const vmath::Vector3& position);
}

}
#endif
