#include "Text.h"
#include <cstdio>
#include <Game/Transform.h>

void Game::Text::prepareFont(const char* fontName)
{
	strcpy_s(fontName_, fontName);
	FILE* mshFile = nullptr;
	char mshFilename[250];
	sprintf_s(mshFilename, "%s.msh", fontName);
	fopen_s(&mshFile, mshFilename, "r");
	for (unsigned int i = 0; i < 256; ++i)
	{
		vmath::Vector3 positions[4];
		vmath::Vector3 uvs[4];

		for (unsigned int vi = 0; vi < 4; ++vi)
		{
			float posX, posY, uvX, uvY;
			fscanf_s(mshFile, "%f %f %f %f \n", &posX ,&posY, &uvX, &uvY);
			positions[vi] = vmath::Vector3(posX, posY, .0f);
			uvs[vi] = vmath::Vector3(uvX, uvY, .0f);
		}
		fscanf_s(mshFile, "%f \n", asciiWidths_ +  i  );
		asciiPositions_[6 * i + 0] = positions[0];		 asciiUvs_[6 * i + 0] = uvs[0];
		asciiPositions_[6 * i + 1] = positions[1];		 asciiUvs_[6 * i + 1] = uvs[1];
		asciiPositions_[6 * i + 2] = positions[2];		 asciiUvs_[6 * i + 2] = uvs[2];
		asciiPositions_[6 * i + 3] = positions[1];		 asciiUvs_[6 * i + 3] = uvs[1];
		asciiPositions_[6 * i + 4] = positions[3];		 asciiUvs_[6 * i + 4] = uvs[3];
		asciiPositions_[6 * i + 5] = positions[2];		 asciiUvs_[6 * i + 5] = uvs[2];
	}
	fclose(mshFile);
	
}

void Game::Text::setText(const char * src)
{
	if (text_)
	{
		delete[] text_;
		text_ = nullptr;
	}
	if (!src )
		return;
	unsigned int srcLen = (unsigned int)strlen(src);
	if (!srcLen)
		return;
	if (vBuff_ && srcLen > maxCharNumber_)
		return;
	textToUpdate_ = true;
	text_ = new unsigned char[srcLen + 1];
	for (unsigned int i = 0; i < srcLen; ++i)
	{
		text_[i] = src[i];
	}
	text_[srcLen] = 0;
	//strcpy(text_, src);
	if (!vBuff_)
	{
		maxCharNumber_ = max(maxCharNumber_, srcLen);
	}
	charNumber_ = srcLen;
}

void Game::Text::setMaxCharNumber(unsigned int num)
{
	if (vBuff_)
		return;
	maxCharNumber_ = num;
}

void Game::Text::loadData(Graphics::GdiContext * gdiContext)
{
	Graphics::VertexDesc vDesc = Graphics::VertexDesc::get(Graphics::POS4UV4);
	sh_ = gdiContext->createShader("assets/Shaders/Text.fx", &vDesc, fontName_);

	char texFilename[250];
	sprintf_s(texFilename, "%s.png", fontName_);

	Graphics::TextureDesc textureDesc;
	textureDesc.filter_ = Graphics::TextureDesc::eLinear;
	textureDesc.mode_ = Graphics::TextureDesc::eClamp;
	texture_ = gdiContext->createTexture(texFilename, &textureDesc, fontName_);
	float scale = 1.0f / 240.0f;
	sh_->setTexture(0, texture_);

	Graphics::VertexP4UV4 *points = new Graphics::VertexP4UV4[ 6 * maxCharNumber_ ];
	vmath::Vector3 offset(.0f);
	for (unsigned int i = 0; i < charNumber_; ++i)
	{
		unsigned int index = text_[i];
		for (unsigned int i2 = 0; i2 < 6; ++i2)
		{
			points[i * 6 + i2].pos = vmath::mulPerElem( vmath::Vector4( asciiPositions_[index * 6 + i2] * scale + offset, 1.0f ), vmath::Vector4(baseScale_, 1.0f) );
			points[i * 6 + i2].uv = vmath::Vector4( asciiUvs_[index * 6 + i2], 1.0f );
		}
		offset += vmath::Vector3(asciiWidths_[index] * scale, .0f, .0f);
	}
	textToUpdate_ = false;
	unsigned int bufferSize = sizeof(Graphics::VertexP4UV4) * 6 * maxCharNumber_;
	vBuff_ = gdiContext->createBuffer(points, bufferSize, Graphics::BufferType::DynamicVertex );
	cBuff_ = gdiContext->createBuffer(nullptr, sizeof(_TextConstantBuffer), Graphics::BufferType::Constant);
	delete points;


}

void Game::Text::unloadData(Graphics::GdiContext * gdiContext)
{
	gdiContext->releaseTexture(texture_);
	gdiContext->releaseShader(sh_);
	gdiContext->releaseBuffer(vBuff_);
	gdiContext->releaseBuffer(cBuff_);
}

void Game::Text::render(Graphics::GdiContext * gdiContext, Graphics::RenderContext * renderContext)
{
	if (textToUpdate_)
	{
		Graphics::VertexP4UV4 *points = reinterpret_cast<Graphics::VertexP4UV4 *>(gdiContext->mapWrite(vBuff_));
		vmath::Vector3 offset(.0f);
		float scale = 1.0f / 720.0f;
		for (unsigned int i = 0; i < charNumber_; ++i)
		{
			unsigned int index = (unsigned char)text_[i];
			for (unsigned int i2 = 0; i2 < 6; ++i2)
			{
				points[i * 6 + i2].pos = vmath::mulPerElem(vmath::Vector4(asciiPositions_[index * 6 + i2] * scale + offset, 1.0f), vmath::Vector4(baseScale_, 1.0f));
				points[i * 6 + i2].uv = vmath::Vector4(asciiUvs_[index * 6 + i2], 1.0f);
			}
			offset += vmath::Vector3(asciiWidths_[index] * scale, .0f, .0f);
		}
		gdiContext->unmap(vBuff_);
	}
	

	gdiContext->bindShader(sh_);
	_TextConstantBuffer cbData;
	cbData.projection_ = renderContext->projection_;
	cbData.world_ = getTransform()->getWorldPose();
	cbData.color_ = color_;

	gdiContext->updateBuffer(cBuff_, &cbData);
	gdiContext->setConstantBuffer(cBuff_);
	gdiContext->drawTriangles(vBuff_, 6 * charNumber_);
}
