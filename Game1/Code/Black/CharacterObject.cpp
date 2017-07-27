#include "CharacterObject.h"
#include <Util/RandomGenerator.h>
#include <Game/Transform.h>

Black::CharacterObject::CharacterObject(const char* name, const char* filename, int layer)
	: Object(name)
{
	strcpy_s(filename_, filename); layer_ = layer;
	vBuff_[0] = nullptr;
	vBuff_[1] = nullptr;

	initNoise();

	struct TexDesc{
		float startX, startY, endX, endY, centerX, centerY, globAttX, globAttY;
	} ;
	TexDesc texDescs[eBoneCount];

	texDescs[eBone_torso  ] = { .1562f, .1172f, .9900f, .3389f, .5469f, .2422f, .0000f, .0000f };
	texDescs[eBone_head   ] = { .1329f, .0000f, .9531f, .1055f, .5195f, .0977f, .5000f, .1406f };
	texDescs[eBone_lArm   ] = { .0547f, .7603f, .4609f, .8926f, .2500f, .7803f, .4688f, .1423f };
	texDescs[eBone_rArm   ] = { .0547f, .7603f, .4609f, .8926f, .2500f, .7803f, .4922f, .1423f };
	texDescs[eBone_lHand  ] = { .4844f, .7656f, .9219f, .9243f, .6914f, .7817f, .2500f, .8642f };
	texDescs[eBone_rHand  ] = { .4844f, .7656f, .9219f, .9243f, .6914f, .7817f, .2500f, .8642f };
	texDescs[eBone_lThigh ] = { .1109f, .3647f, .9960f, .5317f, .5313f, .3887f, .5313f, .3145f };
	texDescs[eBone_rThigh ] = { .1109f, .3647f, .9960f, .5317f, .5313f, .3887f, .5313f, .3145f };
	texDescs[eBone_lCalf  ] = { .3750f, .5459f, .8320f, .6758f, .6172f, .5576f, .5938f, .5098f };
	texDescs[eBone_rCalf  ] = { .3750f, .5459f, .8320f, .6758f, .6172f, .5576f, .5938f, .5098f };
	texDescs[eBone_lFoot  ] = { .4336f, .6899f, .9960f, .7461f, .5781f, .7051f, .6563f, .6621f };
	texDescs[eBone_rFoot  ] = { .4336f, .6899f, .9960f, .7461f, .5781f, .7051f, .6563f, .6621f };
	texDescs[eBone_braid1 ] = { .1731f, .9238f, .4028f, .9561f, .3047f, .9287f, .2465f, .0498f };
	texDescs[eBone_braid2 ] = { .1731f, .9238f, .4028f, .9561f, .3047f, .9287f, .2969f, .9512f };
	texDescs[eBone_braid3 ] = { .1731f, .9238f, .4028f, .9561f, .3047f, .9287f, .2969f, .9512f };
	texDescs[eBone_braid4 ] = { .1731f, .9238f, .4028f, .9561f, .3047f, .9287f, .2969f, .9512f };


	
	attachment[eBone_torso] = eBone_torso;
	attachment[eBone_head] = eBone_torso;
	attachment[eBone_lArm] = eBone_torso;
	attachment[eBone_rArm] = eBone_torso;
	attachment[eBone_lHand] = eBone_lArm;
	attachment[eBone_rHand] = eBone_rArm;
	attachment[eBone_lThigh] = eBone_torso;
	attachment[eBone_rThigh] = eBone_torso;
	attachment[eBone_lCalf] = eBone_lThigh;
	attachment[eBone_rCalf] = eBone_rThigh;
	attachment[eBone_lFoot] = eBone_lCalf;
	attachment[eBone_rFoot] = eBone_rCalf;
	attachment[eBone_braid1] = eBone_head;
	attachment[eBone_braid2] = eBone_braid1;
	attachment[eBone_braid3] = eBone_braid2;
	attachment[eBone_braid4] = eBone_braid3;


	for( unsigned int i = 0; i < eBoneCount; ++i )
	{
		TexDesc& td =  texDescs[i];
		boneUV00_[i] = vmath::Vector4( .0f, .0f, td.startX, td.startY );
		boneUV01_[i] = vmath::Vector4( .0f, .0f, td.startX, td.endY );
		boneUV10_[i] = vmath::Vector4( .0f, .0f, td.endX,   td.startY );
		boneUV11_[i] = vmath::Vector4( .0f, .0f, td.endX,   td.endY );
		
		vmath::Vector4 center = vmath::Vector4( td.centerX, td.centerY, .0f, .0f );
		boneLocalA_[i] = vmath::Vector4( td.startX, td.startY, .0f, .0f ) - center;
		boneLocalB_[i] = vmath::Vector4( td.startX, td.endY, .0f, .0f ) - center;
		boneLocalC_[i] = vmath::Vector4( td.endX, td.endY, .0f, .0f ) - center;
		boneLocalD_[i] = vmath::Vector4( td.endX, td.startY, .0f, .0f ) - center;
		vmath::Vector4 scale ( .5f, -4.0f, .0f, .0f);
		boneLocalA_[i] = vmath::mulPerElem( boneLocalA_[i], scale );
		boneLocalB_[i] = vmath::mulPerElem( boneLocalB_[i], scale );
		boneLocalC_[i] = vmath::mulPerElem( boneLocalC_[i], scale );
		boneLocalD_[i] = vmath::mulPerElem( boneLocalD_[i], scale );
		TexDesc& roottd =  texDescs[attachment[i]];
		vmath::Vector4 rootCenter = vmath::Vector4( roottd.centerX, roottd.centerY, .0f, .0f );
		attachmentPoint_[ i ] = vmath::Vector3(td.globAttX, td.globAttY, .0f ) -  rootCenter.getXYZ();
		attachmentPoint_[ i ] = vmath::mulPerElem( attachmentPoint_[i], scale.getXYZ() );
		boneAngle_[i] = .0f;
	}


	for (unsigned int i = 0; i < eBoneCount; ++i)
	{
		boneNames_[i] = boneNameBuffer_ + eBoneNameLength * i;
	}
	strcpy_s(boneNames_[eBone_torso], eBoneNameLength, "torso");
	strcpy_s(boneNames_[eBone_head], eBoneNameLength, "head");
	strcpy_s(boneNames_[eBone_lArm], eBoneNameLength, "left arm");
	strcpy_s(boneNames_[eBone_rArm], eBoneNameLength, "right arm");

	strcpy_s(boneNames_[eBone_lHand], eBoneNameLength, "left hand");
	strcpy_s(boneNames_[eBone_rHand], eBoneNameLength, "right hand");
	strcpy_s(boneNames_[eBone_lThigh], eBoneNameLength, "left thigh");
	strcpy_s(boneNames_[eBone_rThigh], eBoneNameLength, "right thigh");

	strcpy_s(boneNames_[eBone_lCalf], eBoneNameLength, "left calf");
	strcpy_s(boneNames_[eBone_rCalf], eBoneNameLength, "right calf");
	strcpy_s(boneNames_[eBone_lFoot], eBoneNameLength, "left foot");
	strcpy_s(boneNames_[eBone_rFoot], eBoneNameLength, "right foot");

	strcpy_s(boneNames_[eBone_braid1], eBoneNameLength, "braid 1");
	strcpy_s(boneNames_[eBone_braid2], eBoneNameLength, "braid 2");
	strcpy_s(boneNames_[eBone_braid3], eBoneNameLength, "braid 3");
	strcpy_s(boneNames_[eBone_braid4], eBoneNameLength, "braid 4");

	bonePositions_[0] = vmath::Vector3(.0f, 1.5f, .0f);
}

void Black::CharacterObject::loadData(Graphics::GdiContext* gdiContext)
{
	Graphics::VertexDesc vDescPos = Graphics::VertexDesc::get(Graphics::POS4);
	sh_ = gdiContext->createShader("assets/Shaders/BlackSprite.fx", &vDescPos, filename_);

	Graphics::TextureDesc textureDesc;
	textureDesc.filter_ = Graphics::TextureDesc::ePoint;
	textureDesc.mode_ = Graphics::TextureDesc::eClamp;
	texture_ = gdiContext->createTexture(filename_, &textureDesc, filename_);

	sh_->setTexture(0, texture_);
	
	unsigned int bufferSize = sizeof(vmath::Vector4) * 6 * (eBoneCount - 1 + eSpineSegments);
	vBuff_[ 0 ] = gdiContext->createBuffer(nullptr, bufferSize, Graphics::BufferType::DynamicVertex);
	vBuff_[ 1 ] = gdiContext->createBuffer(nullptr, bufferSize, Graphics::BufferType::DynamicVertex);
	cBuff_ = gdiContext->createBuffer(nullptr, sizeof(Graphics::ConstantBufferData), Graphics::BufferType::Constant);

}
void Black::CharacterObject::unloadData(Graphics::GdiContext* gdiContext)
{
	gdiContext->releaseTexture(texture_);
	gdiContext->releaseShader(sh_);
	gdiContext->releaseBuffer(vBuff_[ 0 ]);
	gdiContext->releaseBuffer(vBuff_[ 1 ]);
	gdiContext->releaseBuffer(cBuff_);

}

void Black::CharacterObject::render(Graphics::GdiContext * gdiContext, Graphics::RenderContext * renderContext)
{
	if (needUpdate != 0 && !visible_)
		return;
	gdiContext->bindShader(sh_);
	Graphics::ConstantBufferData cbData;
	cbData.view_ = renderContext->view_;
	cbData.projection_ = renderContext->projection_;
	cbData.world_ = getTransform()->getWorldPose();

	gdiContext->updateBuffer(cBuff_, &cbData);
	gdiContext->setConstantBuffer(cBuff_);
	gdiContext->drawTriangles(vBuff_[currRender_ ], 6 * (eBoneCount - 1 + eSpineSegments));
}

void Black::CharacterObject::updateGfx(Graphics::GdiContext * gdiContext)
{
	if (!visible_)
	{
		needUpdate = 1;
		return;
	}
	if (vertsGpu_)
		gdiContext->unmap(vBuff_[currUpdate_]);
	currUpdate_ = currUpdate_ ^ 1;
	currRender_ = currRender_ ^ 1;
	vertsGpu_ = reinterpret_cast< vmath::Vector4* > (gdiContext->mapWrite(vBuff_[currUpdate_]));

	needUpdate = max(needUpdate - 1, 0);


	vmath::Vector4* meshOut = vertsGpu_;

	bonePositions_[0] = vmath::Vector3(sideOffset_, 1.5f + heightOffset_, .0f);
	
	for (unsigned int i = 0; i < eBoneCount; ++i)
	{
		if (i > 0)
		{
			unsigned int parent = attachment[i];
			vmath::Vector3 attachmentPt = attachmentPoint_[i];
			if (flip_)
			{
				attachmentPt.setX(-attachmentPt.getX());
			}
			bonePositions_[i] =vmath::rotate( vmath::Quat::rotationZ(boneAngle_[parent]), attachmentPt ) + bonePositions_[parent];
		}

		vmath::Vector3 localA = boneLocalA_[i].getXYZ();
		vmath::Vector3 localB = boneLocalB_[i].getXYZ();
		vmath::Vector3 localC = boneLocalC_[i].getXYZ();
		vmath::Vector3 localD = boneLocalD_[i].getXYZ();
		if (flip_)
		{
			localA.setX(-localA.getX());
			localB.setX(-localB.getX());
			localC.setX(-localC.getX());
			localD.setX(-localD.getX());
		}

		vmath::Vector4 meshA = vmath::Vector4( vmath::rotate( vmath::Quat::rotationZ(boneAngle_[i]), localA ) + bonePositions_[i] ) + boneUV00_[ i ];
		vmath::Vector4 meshB = vmath::Vector4( vmath::rotate( vmath::Quat::rotationZ(boneAngle_[i]), localB ) + bonePositions_[i]) + boneUV01_[i];
		vmath::Vector4 meshC = vmath::Vector4( vmath::rotate( vmath::Quat::rotationZ(boneAngle_[i]), localC ) + bonePositions_[i]) + boneUV11_[i];
		vmath::Vector4 meshD = vmath::Vector4( vmath::rotate( vmath::Quat::rotationZ(boneAngle_[i]), localD ) + bonePositions_[i]) + boneUV10_[i];


		if (i == 0)
		{
			vmath::Vector4 startL = meshA;
			vmath::Vector4 startR = meshD;
			vmath::Vector4 diff = meshB - meshA;


			vmath::Vector4 bendDir = vmath::Vector4(diff.getY(), -diff.getX(), .0f, .0f);
			
			vmath::Vector4 step = diff * (1.0f / eSpineSegments);
			float iStep  = (1.0f / (eSpineSegments + 1 ) );
			for ( unsigned int i = 0; i < eSpineSegments; ++i )
			{
				float bendFactor0 = i * iStep;
				float bendFactor1 = (i + 1 )* iStep;
				bendFactor1 = 5.0f * ( bendFactor1 - .1f ) * (.7f - bendFactor1) *spineBend_;
				bendFactor0 = 5.0f * (bendFactor0 - .1f) * (.7f - bendFactor0) * spineBend_;
				vmath::Vector4 bend0 = bendDir * bendFactor0;
				vmath::Vector4 bend1 = bendDir * bendFactor1;
				if (flip_)
				{
					*meshOut = startL + bend0; ++meshOut;
					*meshOut = startL + step + bend1; ++meshOut;
					*meshOut = startR + step + bend1; ++meshOut;
					*meshOut = startL + bend0; ++meshOut;
					*meshOut = startR + step + bend1; ++meshOut;
					*meshOut = startR + bend0; ++meshOut;
				}
				else
				{
					*meshOut = startL + bend0; ++meshOut;
					*meshOut = startR + step + bend1; ++meshOut;
					*meshOut = startL + step + bend1; ++meshOut;
					*meshOut = startL + bend0; ++meshOut;
					*meshOut = startR + bend0; ++meshOut;
					*meshOut = startR + step + bend1; ++meshOut;
				}
				startL += step;
				startR += step;
			}
		}
		else
		{
			if (flip_)
			{
				*meshOut = meshA; ++meshOut;
				*meshOut = meshB; ++meshOut;
				*meshOut = meshC; ++meshOut;
				*meshOut = meshA; ++meshOut;
				*meshOut = meshC; ++meshOut;
				*meshOut = meshD; ++meshOut;
			}
			else
			{
				*meshOut = meshA; ++meshOut;
				*meshOut = meshC; ++meshOut;
				*meshOut = meshB; ++meshOut;
				*meshOut = meshA; ++meshOut;
				*meshOut = meshD; ++meshOut;
				*meshOut = meshC; ++meshOut;
			}
		}

		

		
	}
}

void Black::CharacterObject::blendInWalk(float phase, float lerp)
{
	float angles[eBoneCount];
	float extraNoiseHandBend = (noiseVal_[9] + noiseVal_[7] + noiseVal_[5]) * .333f;
	extraNoiseHandBend *= extraNoiseHandBend * 1.5f;

	angles[CharacterObject::eBone_torso] = -.05f -.1f * noiseVal_[ 0 ];
	angles[CharacterObject::eBone_head] = -.2f * noiseVal_[4];
	angles[CharacterObject::eBone_lArm] = sin(phase) * .3f -.4f * noiseVal_[1] - extraNoiseHandBend * .3f;
	angles[CharacterObject::eBone_rArm] = sin(phase) * -.3f -.4f * noiseVal_[2] - extraNoiseHandBend* .3f;
	
	angles[CharacterObject::eBone_lHand] = angles[CharacterObject::eBone_lArm] * 1.1f + .3f + extraNoiseHandBend;
	angles[CharacterObject::eBone_rHand] = angles[CharacterObject::eBone_rArm] * 1.1f + .3f + extraNoiseHandBend;
	angles[CharacterObject::eBone_lThigh] = cos(phase) * .6f - .2f;
	angles[CharacterObject::eBone_rThigh] = -cos(phase) * .6f - .2f;
	angles[CharacterObject::eBone_lCalf] = min( sin(phase) * .8f - .2f, angles[CharacterObject::eBone_lThigh] );
	angles[CharacterObject::eBone_rCalf] = min(sin(phase) * -.8f - .2f, angles[CharacterObject::eBone_rThigh]);

	
	angles[CharacterObject::eBone_lFoot] = angles[CharacterObject::eBone_lCalf] * .7f;
	angles[CharacterObject::eBone_rFoot] = angles[CharacterObject::eBone_rCalf] * .7f;

	angles[CharacterObject::eBone_braid1] = cos(phase + noiseVal_[7] * 4.0f) * .2f;
	angles[CharacterObject::eBone_braid2] = cos(phase + 1.0f + noiseVal_[5] * 4.0f) * .2f - .2f;
	angles[CharacterObject::eBone_braid3] = cos(phase + 2.0f + noiseVal_[6] * 4.0f) * .2f - .2f;
	angles[CharacterObject::eBone_braid4] = cos(phase + 3.0f) * .4f - .2f;



	for (unsigned int i = 0; i < eBoneCount; ++i)
	{
		boneAngle_[i] += lerp * angles[i];


		
	}
	heightOffset_ += .02f * sin(2.0f * phase) * lerp;
	
}

void Black::CharacterObject::blendInJump(float phase, float lerp)
{
	float angles[eBoneCount];
	float extraNoiseHandBend = (noiseVal_[9] + noiseVal_[7] + noiseVal_[5]) * .333f;
	extraNoiseHandBend *= extraNoiseHandBend * 1.5f;

	angles[CharacterObject::eBone_torso] = -.6f;
	angles[CharacterObject::eBone_head] = -.4f;
	angles[CharacterObject::eBone_lArm] = - .6f - extraNoiseHandBend * .3f;
	angles[CharacterObject::eBone_rArm] = - .6f - extraNoiseHandBend * .3f;

	angles[CharacterObject::eBone_lHand] = .5f - .3f + extraNoiseHandBend;
	angles[CharacterObject::eBone_rHand] = .5f - .3f + extraNoiseHandBend;
	angles[CharacterObject::eBone_lThigh] = .8f;
	angles[CharacterObject::eBone_rThigh] = 1.0f;
	angles[CharacterObject::eBone_lCalf] = -.5f;
	angles[CharacterObject::eBone_rCalf] = -.5f;


	angles[CharacterObject::eBone_lFoot] = angles[CharacterObject::eBone_lCalf] * 1.0f - .3f;
	angles[CharacterObject::eBone_rFoot] = angles[CharacterObject::eBone_rCalf] * 1.0f - .3f;

	angles[CharacterObject::eBone_braid1] = cos(phase * 2 + noiseVal_[7] * 4.0f) * .2f;
	angles[CharacterObject::eBone_braid2] = cos(phase * 2 + 1.0f + noiseVal_[5] * 4.0f) * .2f - .2f;
	angles[CharacterObject::eBone_braid3] = cos(phase * 2 + 2.0f + noiseVal_[6] * 4.0f) * .2f - .2f;
	angles[CharacterObject::eBone_braid4] = cos(phase * 2 + 3.0f) * .4f - .2f;

	float lrp = lerp;
	if (flip_)
		lrp *= -1.0f;
	for (unsigned int i = 0; i < eBoneCount; ++i)
	{
		boneAngle_[i] += lrp * angles[i];
	}
	spineBend_ += .15f * lrp;
	heightOffset_ -= .5f * lerp;

}

void Black::CharacterObject::initNoise()
{
	for (unsigned int i = 0; i < eNoiseCount; ++i)
	{
		
		noiseStart_[i] = gen_.getFloat();
		noiseVal_[i] = noiseStart_[i];
		noiseTarget_[i] = gen_.getFloat();
		noiseSpeed_[i] = gen_.getFloat( .25f, 5.0f);
		noiseT_[i] = .0f;

	}
}

void Black::CharacterObject::updateNoise(float deltaTime)
{
	for (unsigned int i = 0; i < eNoiseCount; ++i)
	{
		noiseT_[i] += deltaTime * noiseSpeed_[ i ];
		if (noiseT_[i] > 1.0f)
		{
			noiseT_[i] -= 1.0f;
			noiseStart_[i]  = noiseTarget_[i];
			noiseTarget_[i] = gen_.getFloat();
			noiseSpeed_[i] = gen_.getFloat(.25f, 5.0f);
		}
		float t = noiseT_[i] * noiseT_[i] * (3 - 2 * noiseT_[i]);

		noiseVal_[i] = noiseStart_[i] + (noiseTarget_[i] - noiseStart_[i]) * t;
	}
}
