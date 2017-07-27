#include "TreeMesh.h"
#include <Util/RandomGenerator.h>
#include <Game/Transform.h>
#include <Game/ParamSet.h>
#include <Black/GroundManager.h>
#include <Black/Ground.h>

Black::TreeMesh::TreeMesh(const char * name)
	: Game::StaticMesh( name)
{
	Util::RandomGenerator rnd(getHashName());
	allocParamSet();
	Game::ParamSet* ps = getPaarmSet();
	ps->clear();
	ps->registerFloat( "elevation", &(elevation), __SetElevationCallback);
	const unsigned int nBranches = 50;
	const unsigned int nSegments = 20;
	unsigned int vcount = nBranches * (nSegments + 1) * 2;
	typedef Graphics::Semantic S;
	Graphics::VertexDesc vdesc = { {S::Pos4} };
	vmath::Vector4 *data = reinterpret_cast<vmath::Vector4* >( allocateData(vcount, &vdesc) );
	vmath::Vector4 *out = data;
	//setShader("assets/Shaders/Black.fx", "Black");
	setShader("assets/Shaders/BlackWave.fx", "BlackWave");
	setBounds(-10.0f, 10.0f);


	vmath::Vector3 centers[nBranches * nSegments];
	vmath::Vector3 tangents[nBranches * nSegments];
	float widths[nBranches * nSegments];
	float extraPBendPhase = rnd.getFloat(.0f, 6.283f);
	
	unsigned int index = 0;
	for (unsigned int ib = 0; ib < nBranches; ++ib)
	{

		vmath::Vector3 pos;
		vmath::Vector3 tangent;
		float width = 1.0f;
		if (ib == 0)
		{
			pos = vmath::Vector3(.0f, .0f, .0f);
			tangent = vmath::Vector3(.0f, 1.0f, .0f);
			width = 1.0f;
		}
		else
		{
			//unsigned int indexhalf = index / 2;
			unsigned int startIndex = rnd.getUint(index - 10) + 10;// +indexhalf;
			pos = centers[startIndex];
			tangent = tangents[startIndex];
			width = widths[startIndex];
		}
		float phase = rnd.getFloat(.0f, 6.283f);
		float phaseStep = rnd.getFloat(.4f, .8f); //min( rnd.getFloat(.2f, .4f) + ib * .05f, .6f );
		float angleStep = min( rnd.getFloat(.05f, .2f) + ib * .05f, .7f );
		float curWidth = width;
		float widthStep = width / nSegments;
		for (unsigned int si = 0; si < nSegments; ++si)
		{
				
			centers[index ] = pos;  
			tangents[index] = tangent;
			widths[index] = curWidth;

			vmath::Vector3 side (tangent.getY(), -tangent.getX(), .0f );
			vmath::Vector3 left = pos - side * min( .3f * curWidth, .1f );
			vmath::Vector3 right = pos + side * min(.3f * curWidth, .1f);

			phase += phaseStep;
			float angle = sin(phase) * angleStep;
			float wavePhase = (tangent.getY() ) * 0.15f + extraPBendPhase;
			
			float bend = max( (1.0f - curWidth) * .2f - .1f, .0f );
			left.setZ(bend);
			right.setZ(bend);

			if (!si)
			{
				*out = vmath::Vector4(left, wavePhase);
				++out;
			}
			*out = vmath::Vector4(left, wavePhase);
			out++;
			*out = vmath::Vector4(right, wavePhase);
			out++;
			if( si == nSegments - 1)
			{ 
				*out = vmath::Vector4(right, wavePhase);
				out++;
			}
			curWidth -= widthStep;
			tangent = vmath::rotate(vmath::Quat::rotationZ(angle), tangent);
			pos += tangent * .2f;
			++index;
		}
	}
}

Black::TreeMesh::~TreeMesh()
{
}

void Black::TreeMesh::serialize(unsigned char* buffer, unsigned int* outBytes, unsigned int maxBytes)
{
	char data[220];
	float x = getTransform()->getTranslation().getX();
	unsigned int layerId = getLayer();
	sprintf_s(data, "level.tree %s %1.2f %d\n", getName(), x, layerId);
	if (hasPaarmSet())
	{
		getPaarmSet()->serialize(data, 220, this);
	}
	*outBytes = min( (unsigned int)strlen(data) + 1, maxBytes );
	memcpy( buffer, data, *outBytes );
}

void Black::TreeMesh::__SetElevationCallback(Game::Object * obj)
{
	Black::TreeMesh* treeMesh = reinterpret_cast<Black::TreeMesh*>(obj);
	Game::Transform* t = obj->getTransform();
	float x = t->getTranslation().getX();
	float y = Black::GroundManager::getInstance()->getGround(treeMesh->getLayer())->getTopHeight(x) + treeMesh->elevation;
	t->setTranslation(vmath::Vector3(x, y, .0f));
}
