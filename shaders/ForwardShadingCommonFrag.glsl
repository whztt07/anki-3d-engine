// Copyright (C) 2009-2018, Panagiotis Christopoulos Charitos and contributors.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#pragma once

// Common code for all fragment shaders of BS
#include <shaders/Common.glsl>
#include <shaders/Functions.glsl>
#include <shaders/glsl_cpp_common/Clusterer.h>

// Global resources
layout(ANKI_TEX_BINDING(0, 0)) uniform sampler2D anki_msDepthRt;
#define LIGHT_SET 0
#define LIGHT_UBO_BINDING 0
#define LIGHT_SS_BINDING 0
#define LIGHT_TEX_BINDING 1
#define LIGHT_LIGHTS
#define LIGHT_COMMON_UNIS
#include <shaders/ClusterLightCommon.glsl>

#define anki_u_time u_time
#define RENDERER_SIZE (u_rendererSize * 0.5)

layout(location = 0) out Vec4 out_color;

void writeGBuffer(in Vec4 color)
{
	out_color = Vec4(color.rgb, 1.0 - color.a);
}

Vec4 readAnimatedTextureRgba(sampler2DArray tex, F32 period, Vec2 uv, F32 time)
{
	F32 layerCount = F32(textureSize(tex, 0).z);
	F32 layer = mod(time * layerCount / period, layerCount);
	return texture(tex, Vec3(uv, layer));
}

Vec3 computeLightColor(Vec3 diffCol, Vec3 worldPos)
{
	Vec3 outColor = Vec3(0.0);

	// Find the cluster and then the light counts
	U32 clusterIdx = computeClusterIndex(
		u_clustererMagic, gl_FragCoord.xy / RENDERER_SIZE, worldPos, u_clusterCountX, u_clusterCountY);

	U32 idxOffset = u_clusters[clusterIdx];

	// Skip decals
	U32 count = u_lightIndices[idxOffset];
	idxOffset += count + 1;

	// Point lights
	count = u_lightIndices[idxOffset++];
	U32 idxOffsetEnd = idxOffset + count;
	ANKI_LOOP while(idxOffset < idxOffsetEnd)
	{
		PointLight light = u_pointLights[u_lightIndices[idxOffset++]];

		Vec3 diffC = diffuseLambert(diffCol) * light.m_diffuseColorTileSize.rgb;

		Vec3 frag2Light = light.m_posRadius.xyz - worldPos;
		F32 att = computeAttenuationFactor(light.m_posRadius.w, frag2Light);

#if LOD > 1
		const F32 shadow = 1.0;
#else
		F32 shadow = 1.0;
		if(light.m_diffuseColorTileSize.w >= 0.0)
		{
			shadow = computeShadowFactorOmni(
				frag2Light, light.m_radiusPad1.x, light.m_atlasTiles, light.m_diffuseColorTileSize.w, u_shadowTex);
		}
#endif

		outColor += diffC * (att * shadow);
	}

	// Spot lights
	count = u_lightIndices[idxOffset++];
	idxOffsetEnd = idxOffset + count;
	ANKI_LOOP while(idxOffset < idxOffsetEnd)
	{
		SpotLight light = u_spotLights[u_lightIndices[idxOffset++]];

		Vec3 diffC = diffuseLambert(diffCol) * light.m_diffuseColorShadowmapId.rgb;

		Vec3 frag2Light = light.m_posRadius.xyz - worldPos;
		F32 att = computeAttenuationFactor(light.m_posRadius.w, frag2Light);

		Vec3 l = normalize(frag2Light);

		F32 spot =
			computeSpotFactor(l, light.m_outerCosInnerCos.x, light.m_outerCosInnerCos.y, light.m_lightDirRadius.xyz);

#if LOD > 1
		const F32 shadow = 1.0;
#else
		F32 shadow = 1.0;
		F32 shadowmapLayerIdx = light.m_diffuseColorShadowmapId.w;
		if(shadowmapLayerIdx >= 0.0)
		{
			shadow = computeShadowFactorSpot(light.m_texProjectionMat, worldPos, light.m_lightDirRadius.w, u_shadowTex);
		}
#endif

		outColor += diffC * (att * spot * shadow);
	}

	return outColor;
}

void particleAlpha(Vec4 color, Vec4 scaleColor, Vec4 biasColor)
{
	writeGBuffer(color * scaleColor + biasColor);
}

void fog(Vec3 color, F32 fogAlphaScale, F32 fogDistanceOfMaxThikness, F32 zVSpace)
{
	const Vec2 screenSize = 1.0 / RENDERER_SIZE;

	Vec2 texCoords = gl_FragCoord.xy * screenSize;
	F32 depth = texture(anki_msDepthRt, texCoords).r;
	F32 zFeatherFactor;

	Vec4 fragPosVspace4 = u_invProjMat * Vec4(Vec3(UV_TO_NDC(texCoords), depth), 1.0);
	F32 sceneZVspace = fragPosVspace4.z / fragPosVspace4.w;

	F32 diff = max(0.0, zVSpace - sceneZVspace);

	zFeatherFactor = min(1.0, diff / fogDistanceOfMaxThikness);

	writeGBuffer(Vec4(color, zFeatherFactor * fogAlphaScale));
}
