#ifndef ANKI_SCENE_SECTOR_H
#define ANKI_SCENE_SECTOR_H

#include "anki/scene/Octree.h"

namespace anki {

// Forward
class SceneNode;
class Sector;

/// 2 way Portal
struct Portal
{
	Array<Sector*, 2> sectors;

	Portal();
};

/// A sector. It consists of an octree and some portals
struct Sector
{
	Octree octree;
	SceneVector<Portal*> portals;

	/// Default constructor
	Sector(const SceneAllocator<U8>& alloc, const Aabb& box);

	/// Called when a node was moved or a change in shape happened
	Bool placeSceneNode(SceneNode* sp);

	const Aabb& getAabb() const
	{
		return octree.getRoot().getAabb();
	}
};

/// Sector group. This is supposed to represent the whole sceene
class SectorGroup
{
public:
	/// Default constructor
	SectorGroup(const SceneAllocator<U8>& alloc);

	/// Destructor
	~SectorGroup();

	/// Called when a node was moved or a change in shape happened. The node 
	/// must be Spatial
	///
	/// @return false if scene node is out of all sectors.
	Bool placeSceneNode(SceneNode* sp);

private:
	SceneAllocator<U8> alloc; ///< Keep a copy of the scene allocator
	SceneVector<Sector*> sectors;
	SceneVector<Portal*> portals;
};

} // end namespace anki

#endif
