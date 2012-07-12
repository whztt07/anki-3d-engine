#ifndef ANKI_RESOURCE_MESH_H
#define ANKI_RESOURCE_MESH_H

#include <array>
#include "anki/math/Math.h"
#include "anki/gl/Vbo.h"
#include "anki/collision/Obb.h"

namespace anki {

class MeshLoader;

/// This is the interface class for meshes. Its interface because the skin
/// nodes override it
class MeshBase
{
public:
	enum VboId
	{
		VBO_POSITIONS, ///< VBO never empty
		VBO_NORMALS, ///< VBO never empty
		VBO_TANGENTS, ///< VBO never empty
		VBO_TEX_COORDS, ///< VBO may be empty
		VBO_INDICES, ///< VBO never empty
		VBO_WEIGHTS, ///< VBO may be empty
		VBOS_NUMBER
	};

	virtual ~MeshBase()
	{}

	/// @name Accessors
	/// @{

	/// Get a VBO. Its nullptr if it does not exist
	virtual const Vbo* getVbo(VboId id) const = 0;

	virtual uint32_t getTextureChannelsNumber() const = 0;
	virtual uint32_t getLodsNumber() const = 0;
	virtual uint32_t getIndicesNumber(uint32_t lod) const = 0;
	virtual uint32_t getVerticesNumber(uint32_t lod) const = 0;

	virtual const Obb& getBoundingShape() const = 0;
	/// @}

	/// @name Ask for geometry properties
	/// @{
	bool hasTexCoords() const
	{
		return getTextureChannelsNumber() > 0;
	}

	bool hasWeights() const
	{
		return getVbo(VBO_WEIGHTS) != NULL;
	}

	bool hasNormalsAndTangents() const
	{
		return getVbo(VBO_NORMALS) && getVbo(VBO_TANGENTS);
	}
	/// @}
};

/// Mesh Resource. It contains the geometry packed in VBOs
class Mesh: public MeshBase
{
public:
	typedef std::array<Vbo, VBOS_NUMBER> VbosArray;

	/// @name Constructors
	/// @{

	/// Default constructor. Do nothing
	Mesh()
	{}

	/// Load file
	Mesh(const char* filename)
	{
		load(filename);
	}
	/// @}

	/// Does nothing
	~Mesh()
	{}

	/// @name Accessors
	/// @{

	/// Implements MeshBase::getVbo
	const Vbo* getVbo(VboId id) const
	{
		return vbos[id].isCreated() ? &vbos[id] : NULL;
	}

	/// Implements MeshBase::getTextureChannelsNumber
	uint32_t getTextureChannelsNumber() const
	{
		return vbos[VBO_TEX_COORDS].isCreated() ? 1 : 0;
	}

	/// Implements MeshBase::getLodsNumber
	uint32_t getLodsNumber() const
	{
		return 1;
	}

	/// Implements MeshBase::getIndicesNumber
	uint32_t getIndicesNumber(uint32_t) const
	{
		return vertIdsNum;
	}

	/// Implements MeshBase::getVerticesNumber
	uint32_t getVerticesNumber(uint32_t) const
	{
		return vertsNum;
	}

	/// Implements MeshBase::getBoundingShape
	const Obb& getBoundingShape() const
	{
		return visibilityShape;
	}
	/// @}

	/// Load from a file
	void load(const char* filename);

private:
	VbosArray vbos; ///< The vertex buffer objects
	uint32_t vertIdsNum; ///< The number of vertex IDs
	Obb visibilityShape;
	uint32_t vertsNum;

	/// Create the VBOs using the mesh data
	void createVbos(const MeshLoader& meshData);
};

} // end namespace anki

#endif