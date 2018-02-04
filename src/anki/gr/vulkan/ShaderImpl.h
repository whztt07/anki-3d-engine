// Copyright (C) 2009-2018, Panagiotis Christopoulos Charitos and contributors.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#pragma once

#include <anki/gr/Shader.h>
#include <anki/gr/vulkan/VulkanObject.h>
#include <anki/gr/vulkan/DescriptorSet.h>
#include <anki/util/BitSet.h>
#include <iosfwd>

// Forward
namespace spirv_cross
{
struct SpecializationConstant;
} // end namespace spirv_cross

namespace anki
{

/// @addtogroup vulkan
/// @{

/// Shader vulkan implementation.
class ShaderImpl final : public Shader, public VulkanObject<Shader, ShaderImpl>
{
public:
	VkShaderModule m_handle = VK_NULL_HANDLE;

	Array<DynamicArray<DescriptorBinding>, MAX_DESCRIPTOR_SETS> m_bindings;
	BitSet<MAX_COLOR_ATTACHMENTS, U8> m_colorAttachmentWritemask = {false};
	BitSet<MAX_VERTEX_ATTRIBUTES, U8> m_attributeMask = {false};
	BitSet<MAX_DESCRIPTOR_SETS, U8> m_descriptorSetMask = {false};
	Array<BitSet<MAX_BINDINGS_PER_DESCRIPTOR_SET, U8>, MAX_DESCRIPTOR_SETS> m_activeBindingMask = {{{false}, {false}}};

	ShaderImpl(GrManager* manager)
		: Shader(manager)
	{
	}

	~ShaderImpl();

	ANKI_USE_RESULT Error init(const ShaderInitInfo& init);

	const VkSpecializationInfo* getSpecConstInfo() const
	{
		return (m_specConstInfo.mapEntryCount) ? &m_specConstInfo : nullptr;
	}

private:
	VkSpecializationInfo m_specConstInfo = {};

	void doReflection(ConstWeakArray<U8> spirv, std::vector<spirv_cross::SpecializationConstant>& specConstIds);
};
/// @}

} // end namespace anki
