#pragma once

#include <Game/Object.h>
#include <Util/RandomGenerator.h>

namespace Black
{
	class CharacterObject
		: public Game::Object
	{
	public:
		enum {
			eNoiseCount = 10,
			eSpineSegments = 10,
			eBoneNameLength = 80,
		};

		enum eBone{
			eBone_torso = 0,
			eBone_head = 1,
			eBone_lArm = 2,
			eBone_rArm = 3,
			eBone_lHand = 4,
			eBone_rHand = 5,
			eBone_lThigh = 6,
			eBone_rThigh = 7,
			eBone_lCalf = 8,
			eBone_rCalf = 9,
			eBone_lFoot = 10,
			eBone_rFoot = 11,
			eBone_braid1 = 12,
			eBone_braid2 = 13,
			eBone_braid3 = 14,
			eBone_braid4 = 15,
			eBoneCount,
		};
	private:

		char boneNameBuffer_[eBoneNameLength * eBoneCount];
		char *boneNames_[eBoneCount];
		vmath::Vector4 boneUV00_[eBoneCount];
		vmath::Vector4 boneUV01_[eBoneCount];
		vmath::Vector4 boneUV10_[eBoneCount];
		vmath::Vector4 boneUV11_[eBoneCount];

		vmath::Vector4 boneLocalA_[eBoneCount];
		vmath::Vector4 boneLocalB_[eBoneCount];
		vmath::Vector4 boneLocalC_[eBoneCount];
		vmath::Vector4 boneLocalD_[eBoneCount];

		vmath::Vector3 attachmentPoint_[eBoneCount];
		unsigned int attachment[eBoneCount];

		vmath::Vector3 bonePositions_[eBoneCount];
		float boneAngle_[eBoneCount];
		bool flip_ = false;
		vmath::Vector4* vertsGpu_ = nullptr;
		float noiseT_[eNoiseCount];
		float noiseSpeed_[eNoiseCount];
		float noiseVal_[eNoiseCount];
		float noiseTarget_[eNoiseCount];
		float noiseStart_[eNoiseCount];
		float spineBend_ = .0f;
		float heightOffset_ = .0f;
		float sideOffset_ = .0f;

	public:
		CharacterObject(const char* name, const char* filename, int layer);
		void loadData(Graphics::GdiContext* gdiContext);
		void unloadData(Graphics::GdiContext* gdiContext);
		void render(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext);
		void updateGfx(Graphics::GdiContext* gdiContext);

		void setAngles(float* val) { for (unsigned int i = 0; i < eBoneCount; ++i) { boneAngle_[i] = val[i]; } };
		void setFlip(bool val) { flip_ = val; }
		void clearAnim() { 
			for (unsigned int i = 0; i < eBoneCount; ++i) { boneAngle_[i] = .0f; } 
			spineBend_ = .0f;
			heightOffset_ = .0f;
			sideOffset_ = .0f;
		}
		void setHeightOffset(float val) { heightOffset_ = val; }
		void setSideOffset(float val) { sideOffset_ = val; }
		void blendInWalk(float phase, float lerp);
		void blendInJump(float phase, float lerp);

		void initNoise();
		void updateNoise(float deltaTime);
		bool visible() const { return visible_; }
		void setVisible(bool val) { visible_ = val; }
		unsigned int getNBones() { return eBoneCount; }
		const char* getBoneName(unsigned int i) const { return (const char*)boneNames_[i]; }
		
		~CharacterObject() {};
	protected:
		Util::RandomGenerator gen_;
		char filename_[255];
		Graphics::Shader* sh_ = nullptr;
		Graphics::Buffer* vBuff_[2];
		Graphics::Buffer* cBuff_ = nullptr;
		Graphics::Texture* texture_ = nullptr;
		unsigned char currRender_ = 0;
		unsigned char currUpdate_ = 1;
		char needUpdate = 2;
		bool visible_ = true;
		
	};

}

