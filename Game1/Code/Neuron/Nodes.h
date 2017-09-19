#ifndef NEURON_NODES_DEF
#define NEURON_NODES_DEF
/*
 * (C) Copytight 2017 Marek Bielawski
 * Neuron::Nodes
 */
#include <Util/Utils.h>
namespace Neuron {

struct Nodes
{
	static const float StartingAmp;
	static const float AmpFadeOff;
	static const float DefaultProgressSpeed;
	static const float FadeOffTime;
	static const float BoostFadeOffSpeed;
	static const float BoostStrength;
	enum {
		nNodes = 300,
		nEdges = 600,
		nEdgesPerNode = 15,
	};
	struct Node
	{
		vmath::Vector3 pos_;
		vmath::Vector3 offsetPhase_;
		vmath::Vector3 offsetSpeed_;
		float offsetBoost_ = .0f;
		float boostAmp_ = .0f;
		float fadeOffTimer_ = .0f;
		unsigned int nConnections_ = 0;
		unsigned int edgeIndices_[nEdgesPerNode];
		vmath::Vector3 edgeDirs_[nEdgesPerNode];
	};
	struct Edge
	{
		unsigned int index0_;
		unsigned int index1_;
		float progress_ = .0f;
		float progressSpeed_ = .0f;
		float fadeOffTime_ = .0f;
		float amp_ = .0f;
		void setup ( unsigned int index0, unsigned int index1)
		{
			index0_ = index0;
			index1_ = index1;
		}
	};
	Node nodes_[nNodes];
	Edge edges_[nEdges];

	Nodes( float range);
	bool isFull( unsigned int index ) const { return nodes_[index].nConnections_ == nEdgesPerNode;}
	bool isConnected( unsigned int index0, unsigned int index1) const;
	void addConnection( unsigned int index0, unsigned int index1, unsigned int & nCurEdges);
	void update( float deltaTime );
	void burstSignal( unsigned int nodeIndex, float amp );
};

}
#endif
