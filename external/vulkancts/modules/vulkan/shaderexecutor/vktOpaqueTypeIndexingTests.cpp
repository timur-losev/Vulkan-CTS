/*------------------------------------------------------------------------
 * Vulkan Conformance Tests
 * ------------------------
 *
 * Copyright (c) 2015 The Khronos Group Inc.
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 * Copyright (c) 2016 The Android Open Source Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice(s) and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 *
 *//*!
 * \file
 * \brief Opaque type (sampler, buffer, atomic counter, ...) indexing tests.
 *//*--------------------------------------------------------------------*/

#include "vktOpaqueTypeIndexingTests.hpp"

#include "tcuTexture.hpp"
#include "tcuTestLog.hpp"
#include "tcuVectorUtil.hpp"
#include "tcuTextureUtil.hpp"

#include "deStringUtil.hpp"
#include "deRandom.hpp"

#include "vktShaderExecutor.hpp"

#include <sstream>

namespace vkt
{
namespace shaderexecutor
{

namespace
{

enum IndexExprType
{
	INDEX_EXPR_TYPE_CONST_LITERAL	= 0,
	INDEX_EXPR_TYPE_CONST_EXPRESSION,
	INDEX_EXPR_TYPE_UNIFORM,
	INDEX_EXPR_TYPE_DYNAMIC_UNIFORM,

	INDEX_EXPR_TYPE_LAST
};

enum TextureType
{
	TEXTURE_TYPE_1D = 0,
	TEXTURE_TYPE_2D,
	TEXTURE_TYPE_CUBE,
	TEXTURE_TYPE_2D_ARRAY,
	TEXTURE_TYPE_3D,

	TEXTURE_TYPE_LAST
};

class OpaqueTypeIndexingCase : public TestCase
{
public:
										OpaqueTypeIndexingCase		(tcu::TestContext&			testCtx,
																	 const char*				name,
																	 const char*				description,
																	 const glu::ShaderType		shaderType,
																	 const IndexExprType		indexExprType);
	virtual								~OpaqueTypeIndexingCase		(void);
	virtual void						initPrograms				(vk::SourceCollections& programCollection) const
										{
											m_executor->setShaderSources(programCollection);
										}
	virtual TestInstance*				createInstance				(Context& context) const = 0;
	void								init						(void);

protected:
	const char*							m_name;
	const glu::ShaderType				m_shaderType;
	const IndexExprType					m_indexExprType;
	ShaderSpec							m_shaderSpec;
	de::MovePtr<ShaderExecutor>			m_executor;
	UniformSetup*						m_uniformSetup;
};

class OpaqueTypeIndexingTestInstance : public TestInstance
{
public:
										OpaqueTypeIndexingTestInstance		(Context&					context,
																			 const glu::ShaderType		shaderType,
																			 const ShaderSpec&			shaderSpec,
																			 ShaderExecutor&			executor,
																			 const char*				name,
																			 UniformSetup*				uniformSetup,
																			 const IndexExprType		indexExprType);
	virtual								~OpaqueTypeIndexingTestInstance		(void);

	virtual tcu::TestStatus				iterate								(void) = 0;

protected:
	void								checkSupported						(const VkDescriptorType descriptorType);

protected:
	tcu::TestContext&					m_testCtx;
	const glu::ShaderType				m_shaderType;
	const ShaderSpec&					m_shaderSpec;
	const char*							m_name;
	const IndexExprType					m_indexExprType;
	ShaderExecutor&						m_executor;
	UniformSetup*						m_uniformSetup;
};

OpaqueTypeIndexingCase::OpaqueTypeIndexingCase (tcu::TestContext&			testCtx,
												const char*					name,
												const char*					description,
												const glu::ShaderType		shaderType,
												const IndexExprType			indexExprType)
	: TestCase			(testCtx, name, description)
	, m_name			(name)
	, m_shaderType		(shaderType)
	, m_indexExprType	(indexExprType)
	, m_executor		(DE_NULL)
	, m_uniformSetup	(new UniformSetup())
{
}

OpaqueTypeIndexingCase::~OpaqueTypeIndexingCase (void)
{
}

void OpaqueTypeIndexingCase::init (void)
{
	DE_ASSERT(!m_executor);

	m_executor = de::MovePtr<ShaderExecutor>(createExecutor(m_shaderType, m_shaderSpec));
	m_testCtx.getLog() << *m_executor;
}

OpaqueTypeIndexingTestInstance::OpaqueTypeIndexingTestInstance (Context&					context,
																const glu::ShaderType		shaderType,
																const ShaderSpec&			shaderSpec,
																ShaderExecutor&				executor,
																const char*					name,
																UniformSetup*				uniformSetup,
																const IndexExprType			indexExprType)
	: TestInstance		(context)
	, m_testCtx			(context.getTestContext())
	, m_shaderType		(shaderType)
	, m_shaderSpec		(shaderSpec)
	, m_name			(name)
	, m_indexExprType	(indexExprType)
	, m_executor		(executor)
	, m_uniformSetup	(uniformSetup)
{
}

OpaqueTypeIndexingTestInstance::~OpaqueTypeIndexingTestInstance (void)
{
}

void OpaqueTypeIndexingTestInstance::checkSupported (const VkDescriptorType descriptorType)
{
	const VkPhysicalDeviceFeatures& deviceFeatures = m_context.getDeviceFeatures();

	if (m_indexExprType != INDEX_EXPR_TYPE_CONST_LITERAL && m_indexExprType != INDEX_EXPR_TYPE_CONST_EXPRESSION)
	{
		switch (descriptorType)
		{
			case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
				if (!deviceFeatures.shaderSampledImageArrayDynamicIndexing)
					TCU_THROW(NotSupportedError, "Dynamic indexing of sampler arrays is not supported");
				break;

			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
				if (!deviceFeatures.shaderUniformBufferArrayDynamicIndexing)
					TCU_THROW(NotSupportedError, "Dynamic indexing of uniform buffer arrays is not supported");
				break;

			case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
				if (!deviceFeatures.shaderStorageBufferArrayDynamicIndexing)
					TCU_THROW(NotSupportedError, "Dynamic indexing of storage buffer arrays is not supported");
				break;

			default:
				break;
		}
	}
}

static deUint32 getFirstFreeBindingLocation (const glu::ShaderType shaderType)
{
	deUint32 location;

	switch (shaderType)
	{
		case glu::SHADERTYPE_TESSELLATION_CONTROL:
		case glu::SHADERTYPE_TESSELLATION_EVALUATION:
		case glu::SHADERTYPE_COMPUTE:
			// 0 - input buffer
			// 1 - output buffer
			location = 2u;
			break;

		default:
			location = 0u;
			break;
	}

	return location;
}

static void declareUniformIndexVars (std::ostream& str, const char* varPrefix, int numVars, deUint32& bindingLocation)
{
	for (int varNdx = 0; varNdx < numVars; varNdx++)
		str << "layout(set = 0, binding = " << bindingLocation++ << ") uniform buf" << varNdx << " { highp int " << varPrefix << varNdx << "; }" << ";\n";
}

static void uploadUniformIndices (UniformSetup* uniformSetup, int numIndices, const int* indices, deUint32& bindingLocation)
{
	for (int varNdx = 0; varNdx < numIndices; varNdx++)
		uniformSetup->addData(new UniformData<int>(bindingLocation++, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, indices[varNdx]));
}

static TextureType getTextureType (glu::DataType samplerType)
{
	switch (samplerType)
	{
		case glu::TYPE_SAMPLER_1D:
		case glu::TYPE_INT_SAMPLER_1D:
		case glu::TYPE_UINT_SAMPLER_1D:
		case glu::TYPE_SAMPLER_1D_SHADOW:
			return TEXTURE_TYPE_1D;

		case glu::TYPE_SAMPLER_2D:
		case glu::TYPE_INT_SAMPLER_2D:
		case glu::TYPE_UINT_SAMPLER_2D:
		case glu::TYPE_SAMPLER_2D_SHADOW:
			return TEXTURE_TYPE_2D;

		case glu::TYPE_SAMPLER_CUBE:
		case glu::TYPE_INT_SAMPLER_CUBE:
		case glu::TYPE_UINT_SAMPLER_CUBE:
		case glu::TYPE_SAMPLER_CUBE_SHADOW:
			return TEXTURE_TYPE_CUBE;

		case glu::TYPE_SAMPLER_2D_ARRAY:
		case glu::TYPE_INT_SAMPLER_2D_ARRAY:
		case glu::TYPE_UINT_SAMPLER_2D_ARRAY:
		case glu::TYPE_SAMPLER_2D_ARRAY_SHADOW:
			return TEXTURE_TYPE_2D_ARRAY;

		case glu::TYPE_SAMPLER_3D:
		case glu::TYPE_INT_SAMPLER_3D:
		case glu::TYPE_UINT_SAMPLER_3D:
			return TEXTURE_TYPE_3D;

		default:
			throw tcu::InternalError("Invalid sampler type");
	}
}

static bool isShadowSampler (glu::DataType samplerType)
{
	return samplerType == glu::TYPE_SAMPLER_1D_SHADOW		||
		   samplerType == glu::TYPE_SAMPLER_2D_SHADOW		||
		   samplerType == glu::TYPE_SAMPLER_2D_ARRAY_SHADOW	||
		   samplerType == glu::TYPE_SAMPLER_CUBE_SHADOW;
}

static glu::DataType getSamplerOutputType (glu::DataType samplerType)
{
	switch (samplerType)
	{
		case glu::TYPE_SAMPLER_1D:
		case glu::TYPE_SAMPLER_2D:
		case glu::TYPE_SAMPLER_CUBE:
		case glu::TYPE_SAMPLER_2D_ARRAY:
		case glu::TYPE_SAMPLER_3D:
			return glu::TYPE_FLOAT_VEC4;

		case glu::TYPE_SAMPLER_1D_SHADOW:
		case glu::TYPE_SAMPLER_2D_SHADOW:
		case glu::TYPE_SAMPLER_CUBE_SHADOW:
		case glu::TYPE_SAMPLER_2D_ARRAY_SHADOW:
			return glu::TYPE_FLOAT;

		case glu::TYPE_INT_SAMPLER_1D:
		case glu::TYPE_INT_SAMPLER_2D:
		case glu::TYPE_INT_SAMPLER_CUBE:
		case glu::TYPE_INT_SAMPLER_2D_ARRAY:
		case glu::TYPE_INT_SAMPLER_3D:
			return glu::TYPE_INT_VEC4;

		case glu::TYPE_UINT_SAMPLER_1D:
		case glu::TYPE_UINT_SAMPLER_2D:
		case glu::TYPE_UINT_SAMPLER_CUBE:
		case glu::TYPE_UINT_SAMPLER_2D_ARRAY:
		case glu::TYPE_UINT_SAMPLER_3D:
			return glu::TYPE_UINT_VEC4;

		default:
			throw tcu::InternalError("Invalid sampler type");
	}
}

static tcu::TextureFormat getSamplerTextureFormat (glu::DataType samplerType)
{
	const glu::DataType		outType			= getSamplerOutputType(samplerType);
	const glu::DataType		outScalarType	= glu::getDataTypeScalarType(outType);

	switch (outScalarType)
	{
		case glu::TYPE_FLOAT:
			if (isShadowSampler(samplerType))
				return tcu::TextureFormat(tcu::TextureFormat::D, tcu::TextureFormat::UNORM_INT16);
			else
				return tcu::TextureFormat(tcu::TextureFormat::RGBA, tcu::TextureFormat::UNORM_INT8);

		case glu::TYPE_INT:		return tcu::TextureFormat(tcu::TextureFormat::RGBA, tcu::TextureFormat::SIGNED_INT8);
		case glu::TYPE_UINT:	return tcu::TextureFormat(tcu::TextureFormat::RGBA, tcu::TextureFormat::UNSIGNED_INT8);

		default:
			throw tcu::InternalError("Invalid sampler type");
	}
}

static glu::DataType getSamplerCoordType (glu::DataType samplerType)
{
	const TextureType	texType		= getTextureType(samplerType);
	int					numCoords	= 0;

	switch (texType)
	{
		case TEXTURE_TYPE_1D:		numCoords = 1;	break;
		case TEXTURE_TYPE_2D:		numCoords = 2;	break;
		case TEXTURE_TYPE_2D_ARRAY:	numCoords = 3;	break;
		case TEXTURE_TYPE_CUBE:		numCoords = 3;	break;
		case TEXTURE_TYPE_3D:		numCoords = 3;	break;
		default:
			DE_ASSERT(false);
	}

	if (isShadowSampler(samplerType))
		numCoords += 1;

	DE_ASSERT(de::inRange(numCoords, 1, 4));

	return numCoords == 1 ? glu::TYPE_FLOAT : glu::getDataTypeFloatVec(numCoords);
}

static void fillTextureData (const tcu::PixelBufferAccess& access, de::Random& rnd)
{
	DE_ASSERT(access.getHeight() == 1 && access.getDepth() == 1);

	if (access.getFormat().order == tcu::TextureFormat::D)
	{
		// \note Texture uses odd values, lookup even values to avoid precision issues.
		const float values[] = { 0.1f, 0.3f, 0.5f, 0.7f, 0.9f };

		for (int ndx = 0; ndx < access.getWidth(); ndx++)
			access.setPixDepth(rnd.choose<float>(DE_ARRAY_BEGIN(values), DE_ARRAY_END(values)), ndx, 0);
	}
	else
	{
		TCU_CHECK_INTERNAL(access.getFormat().order == tcu::TextureFormat::RGBA && access.getFormat().getPixelSize() == 4);

		for (int ndx = 0; ndx < access.getWidth(); ndx++)
			*((deUint32*)access.getDataPtr() + ndx) = rnd.getUint32();
	}
}

static vk::VkImageType getVkImageType (TextureType texType)
{
	switch (texType)
	{
		case TEXTURE_TYPE_1D:			return vk::VK_IMAGE_TYPE_1D;
		case TEXTURE_TYPE_2D:
		case TEXTURE_TYPE_2D_ARRAY:		return vk::VK_IMAGE_TYPE_2D;
		case TEXTURE_TYPE_CUBE:			return vk::VK_IMAGE_TYPE_2D;
		case TEXTURE_TYPE_3D:			return vk::VK_IMAGE_TYPE_3D;
		default:
			DE_FATAL("Impossible");
			return (vk::VkImageType)0;
	}
}

static vk::VkImageViewType getVkImageViewType (TextureType texType)
{
	switch (texType)
	{
		case TEXTURE_TYPE_1D:			return vk::VK_IMAGE_VIEW_TYPE_1D;
		case TEXTURE_TYPE_2D:			return vk::VK_IMAGE_VIEW_TYPE_2D;
		case TEXTURE_TYPE_2D_ARRAY:		return vk::VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		case TEXTURE_TYPE_CUBE:			return vk::VK_IMAGE_VIEW_TYPE_CUBE;
		case TEXTURE_TYPE_3D:			return vk::VK_IMAGE_VIEW_TYPE_3D;
		default:
			DE_FATAL("Impossible");
			return (vk::VkImageViewType)0;
	}
}

// SamplerIndexingCaseInstance

class SamplerIndexingCaseInstance : public OpaqueTypeIndexingTestInstance
{
public:
	enum
	{
		NUM_INVOCATIONS		= 64,
		NUM_SAMPLERS		= 8,
		NUM_LOOKUPS			= 4
	};

								SamplerIndexingCaseInstance		(Context&					context,
																 const glu::ShaderType		shaderType,
																 const ShaderSpec&			shaderSpec,
																 ShaderExecutor&			executor,
																 const char*				name,
																 glu::DataType				samplerType,
																 const IndexExprType		indexExprType,
																 UniformSetup*				uniformSetup,
																 const std::vector<int>&	lookupIndices);
	virtual						~SamplerIndexingCaseInstance	(void);

	virtual tcu::TestStatus		iterate							(void);

protected:
	const glu::DataType			m_samplerType;
	const std::vector<int>&		m_lookupIndices;
};

SamplerIndexingCaseInstance::SamplerIndexingCaseInstance (Context&						context,
														  const glu::ShaderType			shaderType,
														  const ShaderSpec&				shaderSpec,
														  ShaderExecutor&				executor,
														  const char*					name,
														  glu::DataType					samplerType,
														  const IndexExprType			indexExprType,
														  UniformSetup*					uniformSetup,
														  const std::vector<int>&		lookupIndices)
	: OpaqueTypeIndexingTestInstance	(context, shaderType, shaderSpec, executor, name, uniformSetup, indexExprType)
	, m_samplerType						(samplerType)
	, m_lookupIndices					(lookupIndices)
{
}

SamplerIndexingCaseInstance::~SamplerIndexingCaseInstance (void)
{
}

bool isIntegerFormat (const tcu::TextureFormat& format)
{
	const tcu::TextureChannelClass	chnClass	= tcu::getTextureChannelClass(format.type);

	return chnClass == tcu::TEXTURECHANNELCLASS_UNSIGNED_INTEGER ||
		   chnClass == tcu::TEXTURECHANNELCLASS_SIGNED_INTEGER;
}

tcu::TestStatus SamplerIndexingCaseInstance::iterate (void)
{
	const int						numInvocations		= SamplerIndexingCaseInstance::NUM_INVOCATIONS;
	const int						numSamplers			= SamplerIndexingCaseInstance::NUM_SAMPLERS;
	const int						numLookups			= SamplerIndexingCaseInstance::NUM_LOOKUPS;
	const glu::DataType				coordType			= getSamplerCoordType(m_samplerType);
	const glu::DataType				outputType			= getSamplerOutputType(m_samplerType);
	const tcu::TextureFormat		texFormat			= getSamplerTextureFormat(m_samplerType);
	const int						outLookupStride		= numInvocations*getDataTypeScalarSize(outputType);
	std::vector<float>				coords;
	std::vector<deUint32>			outData;
	std::vector<deUint8>			texData				(numSamplers * texFormat.getPixelSize());
	const tcu::PixelBufferAccess	refTexAccess		(texFormat, numSamplers, 1, 1, &texData[0]);
	de::Random						rnd					(deInt32Hash(m_samplerType) ^ deInt32Hash(m_shaderType) ^ deInt32Hash(m_indexExprType));
	const TextureType				texType				= getTextureType(m_samplerType);
	const vk::VkImageType			imageType			= getVkImageType(texType);
	const vk::VkImageViewType		imageViewType		= getVkImageViewType(texType);
	const tcu::Sampler::FilterMode	filterMode			= (isShadowSampler(m_samplerType) || isIntegerFormat(texFormat)) ? tcu::Sampler::NEAREST : tcu::Sampler::LINEAR;
	const tcu::Sampler				refSampler			= isShadowSampler(m_samplerType)
																? tcu::Sampler(tcu::Sampler::CLAMP_TO_EDGE, tcu::Sampler::CLAMP_TO_EDGE, tcu::Sampler::CLAMP_TO_EDGE,
																				filterMode, filterMode, 0.0f, false /* non-normalized */,
																				tcu::Sampler::COMPAREMODE_LESS)
																: tcu::Sampler(tcu::Sampler::CLAMP_TO_EDGE, tcu::Sampler::CLAMP_TO_EDGE, tcu::Sampler::CLAMP_TO_EDGE,
																				filterMode, filterMode);

	checkSupported(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

	coords.resize(numInvocations * getDataTypeScalarSize(coordType));

	if (texType == TEXTURE_TYPE_CUBE)
	{
		if (isShadowSampler(m_samplerType))
		{
			for (size_t i = 0; i < coords.size() / 4; i++)
			{
				coords[4 * i] = 1.0f;
				coords[4 * i + 1] = coords[4 * i + 2] = coords[4 * i + 3] = 0.0f;
			}
		}
		else
		{
			for (size_t i = 0; i < coords.size() / 3; i++)
			{
				coords[3 * i] = 1.0f;
				coords[3 * i + 1] = coords[3 * i + 2] = 0.0f;
			}
		}
	}

	if (isShadowSampler(m_samplerType))
	{
		// Use different comparison value per invocation.
		// \note Texture uses odd values, comparison even values.
		const int	numCoordComps	= getDataTypeScalarSize(coordType);
		const float	cmpValues[]		= { 0.0f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f };

		for (int invocationNdx = 0; invocationNdx < numInvocations; invocationNdx++)
			coords[invocationNdx*numCoordComps + (numCoordComps-1)] = rnd.choose<float>(DE_ARRAY_BEGIN(cmpValues), DE_ARRAY_END(cmpValues));
	}

	fillTextureData(refTexAccess, rnd);

	outData.resize(numLookups*outLookupStride);

	{
		std::vector<void*>		inputs;
		std::vector<void*>		outputs;
		std::vector<int>		expandedIndices;
		deUint32				bindingLocation		= getFirstFreeBindingLocation(m_shaderType);

		inputs.push_back(&coords[0]);

		m_uniformSetup->addData(new SamplerUniformData(bindingLocation++, (deUint32)numSamplers, refSampler, texFormat, tcu::IVec3(1, 1, 1), imageType, imageViewType, &texData[0]));

		if (m_indexExprType == INDEX_EXPR_TYPE_DYNAMIC_UNIFORM)
		{
			expandedIndices.resize(numInvocations * m_lookupIndices.size());
			for (int lookupNdx = 0; lookupNdx < numLookups; lookupNdx++)
			{
				for (int invNdx = 0; invNdx < numInvocations; invNdx++)
					expandedIndices[lookupNdx*numInvocations + invNdx] = m_lookupIndices[lookupNdx];
			}

			for (int lookupNdx = 0; lookupNdx < numLookups; lookupNdx++)
				inputs.push_back(&expandedIndices[lookupNdx*numInvocations]);
		}
		else if (m_indexExprType == INDEX_EXPR_TYPE_UNIFORM)
			uploadUniformIndices(m_uniformSetup, numLookups, &m_lookupIndices[0], bindingLocation);

		for (int lookupNdx = 0; lookupNdx < numLookups; lookupNdx++)
			outputs.push_back(&outData[outLookupStride*lookupNdx]);

		m_executor.setUniforms(m_uniformSetup);

		m_executor.execute(m_context, numInvocations, &inputs[0], &outputs[0]);
	}

	{
		tcu::TestLog&		log				= m_context.getTestContext().getLog();
		tcu::TestStatus		testResult		= tcu::TestStatus::pass("Pass");

		if (isShadowSampler(m_samplerType))
		{
			const int			numCoordComps	= getDataTypeScalarSize(coordType);

			TCU_CHECK_INTERNAL(getDataTypeScalarSize(outputType) == 1);

			// Each invocation may have different results.
			for (int invocationNdx = 0; invocationNdx < numInvocations; invocationNdx++)
			{
				const float	coord	= coords[invocationNdx*numCoordComps + (numCoordComps-1)];

				for (int lookupNdx = 0; lookupNdx < numLookups; lookupNdx++)
				{
					const int		texNdx		= m_lookupIndices[lookupNdx];
					const float		result		= *((const float*)(const deUint8*)&outData[lookupNdx*outLookupStride + invocationNdx]);
					const float		reference	= refTexAccess.sample2DCompare(refSampler, tcu::Sampler::NEAREST, coord, (float)texNdx, 0.0f, tcu::IVec3(0));

					if (de::abs(result-reference) > 0.005f)
					{
						log << tcu::TestLog::Message << "ERROR: at invocation " << invocationNdx << ", lookup " << lookupNdx << ": expected "
							<< reference << ", got " << result
							<< tcu::TestLog::EndMessage;

						if (testResult.getCode() == QP_TEST_RESULT_PASS)
							testResult = tcu::TestStatus::fail("Got invalid lookup result");
					}
				}
			}
		}
		else
		{
			TCU_CHECK_INTERNAL(getDataTypeScalarSize(outputType) == 4);

			// Validate results from first invocation
			for (int lookupNdx = 0; lookupNdx < numLookups; lookupNdx++)
			{
				const int		texNdx	= m_lookupIndices[lookupNdx];
				const deUint8*	resPtr	= (const deUint8*)&outData[lookupNdx*outLookupStride];
				bool			isOk;

				if (outputType == glu::TYPE_FLOAT_VEC4)
				{
					const float			threshold		= 1.0f / 256.0f;
					const tcu::Vec4		reference		= refTexAccess.getPixel(texNdx, 0);
					const float*		floatPtr		= (const float*)resPtr;
					const tcu::Vec4		result			(floatPtr[0], floatPtr[1], floatPtr[2], floatPtr[3]);

					isOk = boolAll(lessThanEqual(abs(reference-result), tcu::Vec4(threshold)));

					if (!isOk)
					{
						log << tcu::TestLog::Message << "ERROR: at lookup " << lookupNdx << ": expected "
							<< reference << ", got " << result
							<< tcu::TestLog::EndMessage;
					}
				}
				else
				{
					const tcu::UVec4	reference		= refTexAccess.getPixelUint(texNdx, 0);
					const deUint32*		uintPtr			= (const deUint32*)resPtr;
					const tcu::UVec4	result			(uintPtr[0], uintPtr[1], uintPtr[2], uintPtr[3]);

					isOk = boolAll(equal(reference, result));

					if (!isOk)
					{
						log << tcu::TestLog::Message << "ERROR: at lookup " << lookupNdx << ": expected "
							<< reference << ", got " << result
							<< tcu::TestLog::EndMessage;
					}
				}

				if (!isOk && testResult.getCode() == QP_TEST_RESULT_PASS)
					testResult = tcu::TestStatus::fail("Got invalid lookup result");
			}

			// Check results of other invocations against first one
			for (int invocationNdx = 1; invocationNdx < numInvocations; invocationNdx++)
			{
				for (int lookupNdx = 0; lookupNdx < numLookups; lookupNdx++)
				{
					const deUint32*		refPtr		= &outData[lookupNdx*outLookupStride];
					const deUint32*		resPtr		= refPtr + invocationNdx*4;
					bool				isOk		= true;

					for (int ndx = 0; ndx < 4; ndx++)
						isOk = isOk && (refPtr[ndx] == resPtr[ndx]);

					if (!isOk)
					{
						log << tcu::TestLog::Message << "ERROR: invocation " << invocationNdx << " result "
							<< tcu::formatArray(tcu::Format::HexIterator<deUint32>(resPtr), tcu::Format::HexIterator<deUint32>(resPtr+4))
							<< " for lookup " << lookupNdx << " doesn't match result from first invocation "
							<< tcu::formatArray(tcu::Format::HexIterator<deUint32>(refPtr), tcu::Format::HexIterator<deUint32>(refPtr+4))
							<< tcu::TestLog::EndMessage;

						if (testResult.getCode() == QP_TEST_RESULT_PASS)
							testResult = tcu::TestStatus::fail("Inconsistent lookup results");
					}
				}
			}
		}

		return testResult;
	}
}

class SamplerIndexingCase : public OpaqueTypeIndexingCase
{
public:
								SamplerIndexingCase			(tcu::TestContext&			testCtx,
															 const char*				name,
															 const char*				description,
															 const glu::ShaderType		shaderType,
															 glu::DataType				samplerType,
															 IndexExprType				indexExprType);
	virtual						~SamplerIndexingCase		(void);

	virtual TestInstance*		createInstance				(Context& ctx) const;

private:
								SamplerIndexingCase			(const SamplerIndexingCase&);
	SamplerIndexingCase&		operator=					(const SamplerIndexingCase&);

	void						createShaderSpec			(void);

	const glu::DataType			m_samplerType;
	const int					m_numSamplers;
	const int					m_numLookups;
	std::vector<int>			m_lookupIndices;
};

SamplerIndexingCase::SamplerIndexingCase (tcu::TestContext&			testCtx,
										  const char*				name,
										  const char*				description,
										  const glu::ShaderType		shaderType,
										  glu::DataType				samplerType,
										  IndexExprType				indexExprType)
	: OpaqueTypeIndexingCase	(testCtx, name, description, shaderType, indexExprType)
	, m_samplerType				(samplerType)
	, m_numSamplers				(SamplerIndexingCaseInstance::NUM_SAMPLERS)
	, m_numLookups				(SamplerIndexingCaseInstance::NUM_LOOKUPS)
	, m_lookupIndices			(m_numLookups)
{
	createShaderSpec();
	init();
}

SamplerIndexingCase::~SamplerIndexingCase (void)
{
}

TestInstance* SamplerIndexingCase::createInstance (Context& ctx) const
{
	return new SamplerIndexingCaseInstance(ctx,
										   m_shaderType,
										   m_shaderSpec,
										   *m_executor,
										   m_name,
										   m_samplerType,
										   m_indexExprType,
										   m_uniformSetup,
										   m_lookupIndices);
}

void SamplerIndexingCase::createShaderSpec (void)
{
	de::Random			rnd				(deInt32Hash(m_samplerType) ^ deInt32Hash(m_shaderType) ^ deInt32Hash(m_indexExprType));
	deUint32			binding			= getFirstFreeBindingLocation(m_shaderType);
	const char*			samplersName	= "texSampler";
	const char*			coordsName		= "coords";
	const char*			indicesPrefix	= "index";
	const char*			resultPrefix	= "result";
	const glu::DataType	coordType		= getSamplerCoordType(m_samplerType);
	const glu::DataType	outType			= getSamplerOutputType(m_samplerType);
	std::ostringstream	global, code;

	for (int ndx = 0; ndx < m_numLookups; ndx++)
		m_lookupIndices[ndx] = rnd.getInt(0, m_numSamplers-1);

	m_shaderSpec.inputs.push_back(Symbol(coordsName, glu::VarType(coordType, glu::PRECISION_HIGHP)));

	if (m_indexExprType != INDEX_EXPR_TYPE_CONST_LITERAL)
		global << "#extension GL_EXT_gpu_shader5 : require\n";

	if (m_indexExprType == INDEX_EXPR_TYPE_CONST_EXPRESSION)
		global << "const highp int indexBase = 1;\n";

	global <<
		"layout(set = 0, binding = " << binding++ << ") uniform highp " << getDataTypeName(m_samplerType) << " " << samplersName << "[" << m_numSamplers << "];\n";

	if (m_indexExprType == INDEX_EXPR_TYPE_DYNAMIC_UNIFORM)
	{
		for (int lookupNdx = 0; lookupNdx < m_numLookups; lookupNdx++)
		{
			const std::string varName = indicesPrefix + de::toString(lookupNdx);
			m_shaderSpec.inputs.push_back(Symbol(varName, glu::VarType(glu::TYPE_INT, glu::PRECISION_HIGHP)));
		}
	}
	else if (m_indexExprType == INDEX_EXPR_TYPE_UNIFORM)
		declareUniformIndexVars(global, indicesPrefix, m_numLookups, binding);

	for (int lookupNdx = 0; lookupNdx < m_numLookups; lookupNdx++)
	{
		const std::string varName = resultPrefix + de::toString(lookupNdx);
		m_shaderSpec.outputs.push_back(Symbol(varName, glu::VarType(outType, glu::PRECISION_HIGHP)));
	}

	for (int lookupNdx = 0; lookupNdx < m_numLookups; lookupNdx++)
	{
		code << resultPrefix << "" << lookupNdx << " = texture(" << samplersName << "[";

		if (m_indexExprType == INDEX_EXPR_TYPE_CONST_LITERAL)
			code << m_lookupIndices[lookupNdx];
		else if (m_indexExprType == INDEX_EXPR_TYPE_CONST_EXPRESSION)
			code << "indexBase + " << (m_lookupIndices[lookupNdx]-1);
		else
			code << indicesPrefix << lookupNdx;

		code << "], " << coordsName << ");\n";
	}

	m_shaderSpec.globalDeclarations	= global.str();
	m_shaderSpec.source				= code.str();
}

enum BlockType
{
	BLOCKTYPE_UNIFORM = 0,
	BLOCKTYPE_BUFFER,

	BLOCKTYPE_LAST
};

class BlockArrayIndexingCaseInstance : public OpaqueTypeIndexingTestInstance
{
public:
	enum
	{
		NUM_INVOCATIONS		= 32,
		NUM_INSTANCES		= 4,
		NUM_READS			= 4
	};

									BlockArrayIndexingCaseInstance	(Context&						context,
																	 const glu::ShaderType			shaderType,
																	 const ShaderSpec&				shaderSpec,
																	 ShaderExecutor&				executor,
																	 const char*					name,
																	 BlockType						blockType,
																	 const IndexExprType			indexExprType,
																	 UniformSetup*					uniformSetup,
																	 const std::vector<int>&		readIndices,
																	 const std::vector<deUint32>&	inValues);
	virtual							~BlockArrayIndexingCaseInstance	(void);

	virtual tcu::TestStatus			iterate							(void);

private:
	const BlockType					m_blockType;
	const std::vector<int>&			m_readIndices;
	const std::vector<deUint32>&	m_inValues;
};

BlockArrayIndexingCaseInstance::BlockArrayIndexingCaseInstance (Context&						context,
																const glu::ShaderType			shaderType,
																const ShaderSpec&				shaderSpec,
																ShaderExecutor&					executor,
																const char*						name,
																BlockType						blockType,
																const IndexExprType				indexExprType,
																UniformSetup*					uniformSetup,
																const std::vector<int>&			readIndices,
																const std::vector<deUint32>&	inValues)
	: OpaqueTypeIndexingTestInstance	(context, shaderType, shaderSpec, executor, name, uniformSetup, indexExprType)
	, m_blockType						(blockType)
	, m_readIndices						(readIndices)
	, m_inValues						(inValues)
{
}

BlockArrayIndexingCaseInstance::~BlockArrayIndexingCaseInstance (void)
{
}

tcu::TestStatus BlockArrayIndexingCaseInstance::iterate (void)
{
	const int					numInvocations		= NUM_INVOCATIONS;
	const int					numReads			= NUM_READS;
	std::vector<deUint32>		outValues			(numInvocations*numReads);

	{
		tcu::TestLog&			log					= m_context.getTestContext().getLog();
		tcu::TestStatus			testResult			= tcu::TestStatus::pass("Pass");
		std::vector<int>		expandedIndices;
		std::vector<void*>		inputs;
		std::vector<void*>		outputs;
		deUint32				bindingLocation		= getFirstFreeBindingLocation(m_shaderType);
		VkDescriptorType		descriptorType		= m_blockType == BLOCKTYPE_UNIFORM ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

		checkSupported(descriptorType);

		for (size_t i = 0 ; i < m_inValues.size(); i++)
			m_uniformSetup->addData(new UniformData<deUint32>(bindingLocation++, descriptorType, m_inValues[i]));

		if (m_indexExprType == INDEX_EXPR_TYPE_DYNAMIC_UNIFORM)
		{
			expandedIndices.resize(numInvocations * m_readIndices.size());

			for (int readNdx = 0; readNdx < numReads; readNdx++)
			{
				int* dst = &expandedIndices[numInvocations*readNdx];
				std::fill(dst, dst+numInvocations, m_readIndices[readNdx]);
			}

			for (int readNdx = 0; readNdx < numReads; readNdx++)
				inputs.push_back(&expandedIndices[readNdx*numInvocations]);
		}
		else if (m_indexExprType == INDEX_EXPR_TYPE_UNIFORM)
			uploadUniformIndices(m_uniformSetup, numReads, &m_readIndices[0], bindingLocation);

		for (int readNdx = 0; readNdx < numReads; readNdx++)
			outputs.push_back(&outValues[readNdx*numInvocations]);

		m_executor.setUniforms(m_uniformSetup);

		m_executor.execute(m_context, numInvocations, inputs.empty() ? DE_NULL : &inputs[0], &outputs[0]);

		for (int invocationNdx = 0; invocationNdx < numInvocations; invocationNdx++)
		{
			for (int readNdx = 0; readNdx < numReads; readNdx++)
			{
				const deUint32	refValue	= m_inValues[m_readIndices[readNdx]];
				const deUint32	resValue	= outValues[readNdx*numInvocations + invocationNdx];

				if (refValue != resValue)
				{
					log << tcu::TestLog::Message << "ERROR: at invocation " << invocationNdx
						<< ", read " << readNdx << ": expected "
						<< tcu::toHex(refValue) << ", got " << tcu::toHex(resValue)
						<< tcu::TestLog::EndMessage;

					if (testResult.getCode() == QP_TEST_RESULT_PASS)
						testResult = tcu::TestStatus::fail("Invalid result value");
				}
			}
		}

		return testResult;
	}
}

class BlockArrayIndexingCase : public OpaqueTypeIndexingCase
{
public:
								BlockArrayIndexingCase		(tcu::TestContext&			testCtx,
															 const char*				name,
															 const char*				description,
															 BlockType					blockType,
															 IndexExprType				indexExprType,
															 const glu::ShaderType		shaderType);
	virtual						~BlockArrayIndexingCase		(void);

	virtual TestInstance*		createInstance				(Context& ctx) const;

private:
								BlockArrayIndexingCase		(const BlockArrayIndexingCase&);
	BlockArrayIndexingCase&		operator=					(const BlockArrayIndexingCase&);

	void						createShaderSpec			(void);

	const BlockType				m_blockType;
	std::vector<int>			m_readIndices;
	std::vector<deUint32>		m_inValues;
};

BlockArrayIndexingCase::BlockArrayIndexingCase (tcu::TestContext&			testCtx,
												const char*					name,
												const char*					description,
												BlockType					blockType,
												IndexExprType				indexExprType,
												const glu::ShaderType		shaderType)
	: OpaqueTypeIndexingCase	(testCtx, name, description, shaderType, indexExprType)
	, m_blockType				(blockType)
	, m_readIndices				(BlockArrayIndexingCaseInstance::NUM_READS)
	, m_inValues				(BlockArrayIndexingCaseInstance::NUM_INSTANCES)
{
	createShaderSpec();
	init();
}

BlockArrayIndexingCase::~BlockArrayIndexingCase (void)
{
}

TestInstance* BlockArrayIndexingCase::createInstance (Context& ctx) const
{
	return new BlockArrayIndexingCaseInstance(ctx,
											  m_shaderType,
											  m_shaderSpec,
											  *m_executor,
											  m_name,
											  m_blockType,
											  m_indexExprType,
											  m_uniformSetup,
											  m_readIndices,
											  m_inValues);
}

void BlockArrayIndexingCase::createShaderSpec (void)
{
	const int			numInstances	= BlockArrayIndexingCaseInstance::NUM_INSTANCES;
	const int			numReads		= BlockArrayIndexingCaseInstance::NUM_READS;
	de::Random			rnd				(deInt32Hash(m_shaderType) ^ deInt32Hash(m_blockType) ^ deInt32Hash(m_indexExprType));
	deUint32			binding			= getFirstFreeBindingLocation(m_shaderType);
	const char*			blockName		= "Block";
	const char*			instanceName	= "block";
	const char*			indicesPrefix	= "index";
	const char*			resultPrefix	= "result";
	const char*			interfaceName	= m_blockType == BLOCKTYPE_UNIFORM ? "uniform" : "buffer";
	std::ostringstream	global, code;

	for (int readNdx = 0; readNdx < numReads; readNdx++)
		m_readIndices[readNdx] = rnd.getInt(0, numInstances-1);

	for (int instanceNdx = 0; instanceNdx < numInstances; instanceNdx++)
		m_inValues[instanceNdx] = rnd.getUint32();

	if (m_indexExprType != INDEX_EXPR_TYPE_CONST_LITERAL)
		global << "#extension GL_EXT_gpu_shader5 : require\n";

	if (m_indexExprType == INDEX_EXPR_TYPE_CONST_EXPRESSION)
		global << "const highp int indexBase = 1;\n";

	global <<
		"layout(set = 0, binding = " << binding << ") " << interfaceName << " " << blockName << "\n"
		"{\n"
		"	highp uint value;\n"
		"} " << instanceName << "[" << numInstances << "];\n";

	binding += numInstances;

	if (m_indexExprType == INDEX_EXPR_TYPE_DYNAMIC_UNIFORM)
	{
		for (int readNdx = 0; readNdx < numReads; readNdx++)
		{
			const std::string varName = indicesPrefix + de::toString(readNdx);
			m_shaderSpec.inputs.push_back(Symbol(varName, glu::VarType(glu::TYPE_INT, glu::PRECISION_HIGHP)));
		}
	}
	else if (m_indexExprType == INDEX_EXPR_TYPE_UNIFORM)
		declareUniformIndexVars(global, indicesPrefix, numReads, binding);

	for (int readNdx = 0; readNdx < numReads; readNdx++)
	{
		const std::string varName = resultPrefix + de::toString(readNdx);
		m_shaderSpec.outputs.push_back(Symbol(varName, glu::VarType(glu::TYPE_UINT, glu::PRECISION_HIGHP)));
	}

	for (int readNdx = 0; readNdx < numReads; readNdx++)
	{
		code << resultPrefix << readNdx << " = " << instanceName << "[";

		if (m_indexExprType == INDEX_EXPR_TYPE_CONST_LITERAL)
			code << m_readIndices[readNdx];
		else if (m_indexExprType == INDEX_EXPR_TYPE_CONST_EXPRESSION)
			code << "indexBase + " << (m_readIndices[readNdx]-1);
		else
			code << indicesPrefix << readNdx;

		code << "].value;\n";
	}

	m_shaderSpec.globalDeclarations	= global.str();
	m_shaderSpec.source				= code.str();
}

class AtomicCounterIndexingCaseInstance : public OpaqueTypeIndexingTestInstance
{
public:
	enum
	{
		NUM_INVOCATIONS		= 32,
		NUM_COUNTERS		= 4,
		NUM_OPS				= 4
	};

								AtomicCounterIndexingCaseInstance	(Context&					context,
																	 const glu::ShaderType		shaderType,
																	 const ShaderSpec&			shaderSpec,
																	 ShaderExecutor&			executor,
																	 const char*				name,
																	 UniformSetup*				uniformSetup,
																	 const std::vector<int>&	opIndices,
																	 const IndexExprType		indexExprType);
	virtual						~AtomicCounterIndexingCaseInstance	(void);

	virtual	tcu::TestStatus		iterate								(void);

private:
	const std::vector<int>&		m_opIndices;
};

AtomicCounterIndexingCaseInstance::AtomicCounterIndexingCaseInstance (Context&					context,
																	  const glu::ShaderType		shaderType,
																	  const ShaderSpec&			shaderSpec,
																	  ShaderExecutor&			executor,
																	  const char*				name,
																	  UniformSetup*				uniformSetup,
																	  const std::vector<int>&	opIndices,
																	  const IndexExprType		indexExprType)
	: OpaqueTypeIndexingTestInstance	(context, shaderType, shaderSpec, executor, name, uniformSetup, indexExprType)
	, m_opIndices						(opIndices)
{
}

AtomicCounterIndexingCaseInstance::~AtomicCounterIndexingCaseInstance (void)
{
}

tcu::TestStatus AtomicCounterIndexingCaseInstance::iterate (void)
{
	// \todo [2015-12-02 elecro] Add vertexPipelineStoresAndAtomics feature check.
	const int					numInvocations		= NUM_INVOCATIONS;
	const int					numCounters			= NUM_COUNTERS;
	const int					numOps				= NUM_OPS;
	std::vector<int>			expandedIndices;
	std::vector<void*>			inputs;
	std::vector<void*>			outputs;
	std::vector<deUint32>		outValues			(numInvocations*numOps);
	deUint32					bindingLocation		= getFirstFreeBindingLocation(m_shaderType);

	const deUint32 atomicCounterLocation = bindingLocation++;

	checkSupported(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

	{
		DE_ASSERT(numCounters <= 4);
		// Add the atomic counters' base value, all zero.
		m_uniformSetup->addData(new UniformData<tcu::Mat4>(atomicCounterLocation, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, tcu::Mat4(0.0)));

		if (m_indexExprType == INDEX_EXPR_TYPE_DYNAMIC_UNIFORM)
		{
			expandedIndices.resize(numInvocations * m_opIndices.size());

			for (int opNdx = 0; opNdx < numOps; opNdx++)
			{
				int* dst = &expandedIndices[numInvocations*opNdx];
				std::fill(dst, dst+numInvocations, m_opIndices[opNdx]);
			}

			for (int opNdx = 0; opNdx < numOps; opNdx++)
				inputs.push_back(&expandedIndices[opNdx*numInvocations]);
		}
		else if (m_indexExprType == INDEX_EXPR_TYPE_UNIFORM)
			uploadUniformIndices(m_uniformSetup, numOps, &m_opIndices[0], bindingLocation);

		for (int opNdx = 0; opNdx < numOps; opNdx++)
			outputs.push_back(&outValues[opNdx*numInvocations]);

		m_executor.setUniforms(m_uniformSetup);

		m_executor.execute(m_context, numInvocations, inputs.empty() ? DE_NULL : &inputs[0], &outputs[0]);
	}

	{
		tcu::TestLog&					log				= m_context.getTestContext().getLog();
		tcu::TestStatus					testResult		= tcu::TestStatus::pass("Pass");
		std::vector<int>				numHits			(numCounters, 0);	// Number of hits per counter.
		std::vector<deUint32>			counterValues	(numCounters);
		std::vector<std::vector<bool> >	counterMasks	(numCounters);

		for (int opNdx = 0; opNdx < numOps; opNdx++)
			numHits[m_opIndices[opNdx]] += 1;

		// Read counter values
		{
			const void* mapPtr = m_executor.getBufferPtr(atomicCounterLocation);
			DE_ASSERT(mapPtr != DE_NULL);
			std::copy((const deUint32*)mapPtr, (const deUint32*)mapPtr + numCounters, &counterValues[0]);
		}

		// Verify counter values
		for (int counterNdx = 0; counterNdx < numCounters; counterNdx++)
		{
			const deUint32		refCount	= (deUint32)(numHits[counterNdx]*numInvocations);
			const deUint32		resCount	= counterValues[counterNdx];

			if (refCount != resCount)
			{
				log << tcu::TestLog::Message << "ERROR: atomic counter " << counterNdx << " has value " << resCount
					<< ", expected " << refCount
					<< tcu::TestLog::EndMessage;

				if (testResult.getCode() == QP_TEST_RESULT_PASS)
					testResult = tcu::TestStatus::fail("Invalid atomic counter value");
			}
		}

		// Allocate bitmasks - one bit per each valid result value
		for (int counterNdx = 0; counterNdx < numCounters; counterNdx++)
		{
			const int	counterValue	= numHits[counterNdx]*numInvocations;
			counterMasks[counterNdx].resize(counterValue, false);
		}

		// Verify result values from shaders
		for (int invocationNdx = 0; invocationNdx < numInvocations; invocationNdx++)
		{
			for (int opNdx = 0; opNdx < numOps; opNdx++)
			{
				const int		counterNdx	= m_opIndices[opNdx];
				const deUint32	resValue	= outValues[opNdx*numInvocations + invocationNdx];
				const bool		rangeOk		= de::inBounds(resValue, 0u, (deUint32)counterMasks[counterNdx].size());
				const bool		notSeen		= rangeOk && !counterMasks[counterNdx][resValue];
				const bool		isOk		= rangeOk && notSeen;

				if (!isOk)
				{
					log << tcu::TestLog::Message << "ERROR: at invocation " << invocationNdx
						<< ", op " << opNdx << ": got invalid result value "
						<< resValue
						<< tcu::TestLog::EndMessage;

					if (testResult.getCode() == QP_TEST_RESULT_PASS)
						testResult = tcu::TestStatus::fail("Invalid result value");
				}
				else
				{
					// Mark as used - no other invocation should see this value from same counter.
					counterMasks[counterNdx][resValue] = true;
				}
			}
		}

		if (testResult.getCode() == QP_TEST_RESULT_PASS)
		{
			// Consistency check - all masks should be 1 now
			for (int counterNdx = 0; counterNdx < numCounters; counterNdx++)
			{
				for (std::vector<bool>::const_iterator i = counterMasks[counterNdx].begin(); i != counterMasks[counterNdx].end(); i++)
					TCU_CHECK_INTERNAL(*i);
			}
		}

		return testResult;
	}
}

class AtomicCounterIndexingCase : public OpaqueTypeIndexingCase
{
public:
								AtomicCounterIndexingCase	(tcu::TestContext&			testCtx,
															 const char*				name,
															 const char*				description,
															 IndexExprType				indexExprType,
															 const glu::ShaderType		shaderType);
	virtual						~AtomicCounterIndexingCase	(void);

	virtual TestInstance*		createInstance				(Context& ctx) const;

private:
								AtomicCounterIndexingCase	(const BlockArrayIndexingCase&);
	AtomicCounterIndexingCase&	operator=					(const BlockArrayIndexingCase&);

	void						createShaderSpec			(void);

	std::vector<int>			m_opIndices;
};

AtomicCounterIndexingCase::AtomicCounterIndexingCase (tcu::TestContext&			testCtx,
													  const char*				name,
													  const char*				description,
													  IndexExprType				indexExprType,
													  const glu::ShaderType		shaderType)
	: OpaqueTypeIndexingCase	(testCtx, name, description, shaderType, indexExprType)
	, m_opIndices				(AtomicCounterIndexingCaseInstance::NUM_OPS)
{
	createShaderSpec();
	init();
}

AtomicCounterIndexingCase::~AtomicCounterIndexingCase (void)
{
}

TestInstance* AtomicCounterIndexingCase::createInstance (Context& ctx) const
{
	return new AtomicCounterIndexingCaseInstance(ctx,
												 m_shaderType,
												 m_shaderSpec,
												 *m_executor,
												 m_name,
												 m_uniformSetup,
												 m_opIndices,
												 m_indexExprType);
}

void AtomicCounterIndexingCase::createShaderSpec (void)
{
	const int				numCounters		= AtomicCounterIndexingCaseInstance::NUM_COUNTERS;
	const int				numOps			= AtomicCounterIndexingCaseInstance::NUM_OPS;
	deUint32				binding			= getFirstFreeBindingLocation(m_shaderType);
	de::Random				rnd				(deInt32Hash(m_shaderType) ^ deInt32Hash(m_indexExprType));

	for (int opNdx = 0; opNdx < numOps; opNdx++)
		m_opIndices[opNdx] = rnd.getInt(0, numOps-1);

	{
		const char*			indicesPrefix	= "index";
		const char*			resultPrefix	= "result";
		std::ostringstream	global, code;

		if (m_indexExprType != INDEX_EXPR_TYPE_CONST_LITERAL)
			global << "#extension GL_EXT_gpu_shader5 : require\n";

		if (m_indexExprType == INDEX_EXPR_TYPE_CONST_EXPRESSION)
			global << "const highp int indexBase = 1;\n";

		global <<
			"layout(set = 0, binding = " << binding++ << ") buffer AtomicBuffer { highp uint counter[" << numCounters << "]; };\n";

		if (m_indexExprType == INDEX_EXPR_TYPE_DYNAMIC_UNIFORM)
		{
			for (int opNdx = 0; opNdx < numOps; opNdx++)
			{
				const std::string varName = indicesPrefix + de::toString(opNdx);
				m_shaderSpec.inputs.push_back(Symbol(varName, glu::VarType(glu::TYPE_INT, glu::PRECISION_HIGHP)));
			}
		}
		else if (m_indexExprType == INDEX_EXPR_TYPE_UNIFORM)
			declareUniformIndexVars(global, indicesPrefix, numOps, binding);

		for (int opNdx = 0; opNdx < numOps; opNdx++)
		{
			const std::string varName = resultPrefix + de::toString(opNdx);
			m_shaderSpec.outputs.push_back(Symbol(varName, glu::VarType(glu::TYPE_UINT, glu::PRECISION_HIGHP)));
		}

		for (int opNdx = 0; opNdx < numOps; opNdx++)
		{
			code << resultPrefix << opNdx << " = atomicAdd(counter[";

			if (m_indexExprType == INDEX_EXPR_TYPE_CONST_LITERAL)
				code << m_opIndices[opNdx];
			else if (m_indexExprType == INDEX_EXPR_TYPE_CONST_EXPRESSION)
				code << "indexBase + " << (m_opIndices[opNdx]-1);
			else
				code << indicesPrefix << opNdx;

			code << "], uint(1));\n";
		}

		m_shaderSpec.globalDeclarations	= global.str();
		m_shaderSpec.source				= code.str();
	}
}

class OpaqueTypeIndexingTests : public tcu::TestCaseGroup
{
public:
								OpaqueTypeIndexingTests		(tcu::TestContext& testCtx);
	virtual						~OpaqueTypeIndexingTests	(void);

	virtual void				init						(void);

private:
								OpaqueTypeIndexingTests		(const OpaqueTypeIndexingTests&);
	OpaqueTypeIndexingTests&	operator=					(const OpaqueTypeIndexingTests&);
};

OpaqueTypeIndexingTests::OpaqueTypeIndexingTests (tcu::TestContext& testCtx)
	: tcu::TestCaseGroup(testCtx, "opaque_type_indexing", "Opaque Type Indexing Tests")
{
}

OpaqueTypeIndexingTests::~OpaqueTypeIndexingTests (void)
{
}

void OpaqueTypeIndexingTests::init (void)
{
	static const struct
	{
		IndexExprType	type;
		const char*		name;
		const char*		description;
	} indexingTypes[] =
	{
		{ INDEX_EXPR_TYPE_CONST_LITERAL,	"const_literal",		"Indexing by constant literal"					},
		{ INDEX_EXPR_TYPE_CONST_EXPRESSION,	"const_expression",		"Indexing by constant expression"				},
		{ INDEX_EXPR_TYPE_UNIFORM,			"uniform",				"Indexing by uniform value"						},
		{ INDEX_EXPR_TYPE_DYNAMIC_UNIFORM,	"dynamically_uniform",	"Indexing by dynamically uniform expression"	}
	};

	static const struct
	{
		glu::ShaderType	type;
		const char*		name;
	} shaderTypes[] =
	{
		{ glu::SHADERTYPE_VERTEX,		"vertex"	},
		{ glu::SHADERTYPE_FRAGMENT,		"fragment"	},
		{ glu::SHADERTYPE_COMPUTE,		"compute"	}
	};

	// .sampler
	{
		static const glu::DataType samplerTypes[] =
		{
			// \note 1D images will be added by a later extension.
//			glu::TYPE_SAMPLER_1D,
			glu::TYPE_SAMPLER_2D,
			glu::TYPE_SAMPLER_CUBE,
			glu::TYPE_SAMPLER_2D_ARRAY,
			glu::TYPE_SAMPLER_3D,
//			glu::TYPE_SAMPLER_1D_SHADOW,
			glu::TYPE_SAMPLER_2D_SHADOW,
			glu::TYPE_SAMPLER_CUBE_SHADOW,
			glu::TYPE_SAMPLER_2D_ARRAY_SHADOW,
//			glu::TYPE_INT_SAMPLER_1D,
			glu::TYPE_INT_SAMPLER_2D,
			glu::TYPE_INT_SAMPLER_CUBE,
			glu::TYPE_INT_SAMPLER_2D_ARRAY,
			glu::TYPE_INT_SAMPLER_3D,
//			glu::TYPE_UINT_SAMPLER_1D,
			glu::TYPE_UINT_SAMPLER_2D,
			glu::TYPE_UINT_SAMPLER_CUBE,
			glu::TYPE_UINT_SAMPLER_2D_ARRAY,
			glu::TYPE_UINT_SAMPLER_3D,
		};

		tcu::TestCaseGroup* const samplerGroup = new tcu::TestCaseGroup(m_testCtx, "sampler", "Sampler Array Indexing Tests");
		addChild(samplerGroup);

		for (int indexTypeNdx = 0; indexTypeNdx < DE_LENGTH_OF_ARRAY(indexingTypes); indexTypeNdx++)
		{
			const IndexExprType			indexExprType	= indexingTypes[indexTypeNdx].type;
			tcu::TestCaseGroup* const	indexGroup		= new tcu::TestCaseGroup(m_testCtx, indexingTypes[indexTypeNdx].name, indexingTypes[indexTypeNdx].description);
			samplerGroup->addChild(indexGroup);

			for (int shaderTypeNdx = 0; shaderTypeNdx < DE_LENGTH_OF_ARRAY(shaderTypes); shaderTypeNdx++)
			{
				const glu::ShaderType		shaderType		= shaderTypes[shaderTypeNdx].type;
				tcu::TestCaseGroup* const	shaderGroup		= new tcu::TestCaseGroup(m_testCtx, shaderTypes[shaderTypeNdx].name, "");
				indexGroup->addChild(shaderGroup);

				for (int samplerTypeNdx = 0; samplerTypeNdx < DE_LENGTH_OF_ARRAY(samplerTypes); samplerTypeNdx++)
				{
					const glu::DataType	samplerType	= samplerTypes[samplerTypeNdx];
					const char*			samplerName	= getDataTypeName(samplerType);
					const std::string	caseName	= de::toLower(samplerName);

					shaderGroup->addChild(new SamplerIndexingCase(m_testCtx, caseName.c_str(), "", shaderType, samplerType, indexExprType));
				}
			}
		}
	}

	// .ubo / .ssbo / .atomic_counter
	{
		tcu::TestCaseGroup* const	uboGroup	= new tcu::TestCaseGroup(m_testCtx, "ubo",				"Uniform Block Instance Array Indexing Tests");
		tcu::TestCaseGroup* const	ssboGroup	= new tcu::TestCaseGroup(m_testCtx, "ssbo",				"Buffer Block Instance Array Indexing Tests");
		tcu::TestCaseGroup* const	acGroup		= new tcu::TestCaseGroup(m_testCtx, "atomic_counter",	"Atomic Counter Array Indexing Tests");
		addChild(uboGroup);
		addChild(ssboGroup);
		addChild(acGroup);

		for (int indexTypeNdx = 0; indexTypeNdx < DE_LENGTH_OF_ARRAY(indexingTypes); indexTypeNdx++)
		{
			const IndexExprType		indexExprType		= indexingTypes[indexTypeNdx].type;
			const char*				indexExprName		= indexingTypes[indexTypeNdx].name;
			const char*				indexExprDesc		= indexingTypes[indexTypeNdx].description;

			for (int shaderTypeNdx = 0; shaderTypeNdx < DE_LENGTH_OF_ARRAY(shaderTypes); shaderTypeNdx++)
			{
				const glu::ShaderType	shaderType		= shaderTypes[shaderTypeNdx].type;
				const std::string		name			= std::string(indexExprName) + "_" + shaderTypes[shaderTypeNdx].name;

				uboGroup->addChild	(new BlockArrayIndexingCase		(m_testCtx, name.c_str(), indexExprDesc, BLOCKTYPE_UNIFORM,	indexExprType, shaderType));
				acGroup->addChild	(new AtomicCounterIndexingCase	(m_testCtx, name.c_str(), indexExprDesc, indexExprType, shaderType));

				if (indexExprType == INDEX_EXPR_TYPE_CONST_LITERAL || indexExprType == INDEX_EXPR_TYPE_CONST_EXPRESSION)
					ssboGroup->addChild	(new BlockArrayIndexingCase	(m_testCtx, name.c_str(), indexExprDesc, BLOCKTYPE_BUFFER, indexExprType, shaderType));
			}
		}
	}
}

} // anonymous

tcu::TestCaseGroup* createOpaqueTypeIndexingTests (tcu::TestContext& testCtx)
{
	return new OpaqueTypeIndexingTests(testCtx);
}

} // shaderexecutor
} // vkt
