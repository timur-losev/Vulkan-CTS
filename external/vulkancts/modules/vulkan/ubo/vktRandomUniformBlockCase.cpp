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
 * \brief Random uniform block layout case.
 *//*--------------------------------------------------------------------*/

#include "vktRandomUniformBlockCase.hpp"
#include "deRandom.hpp"

namespace vkt
{
namespace ubo
{

namespace
{

static std::string genName (char first, char last, int ndx)
{
	std::string	str			= "";
	int			alphabetLen	= last - first + 1;

	while (ndx > alphabetLen)
	{
		str.insert(str.begin(), (char)(first + ((ndx - 1) % alphabetLen)));
		ndx = (ndx - 1) / alphabetLen;
	}

	str.insert(str.begin(), (char)(first + (ndx % (alphabetLen + 1)) - 1));

	return str;
}

} // anonymous

RandomUniformBlockCase::RandomUniformBlockCase (tcu::TestContext&	testCtx,
												const std::string&	name,
												const std::string&	description,
												BufferMode			bufferMode,
												deUint32			features,
												deUint32			seed)
	: UniformBlockCase		(testCtx, name, description, bufferMode)
	, m_features			(features)
	, m_maxVertexBlocks		((features & FEATURE_VERTEX_BLOCKS)		? 4 : 0)
	, m_maxFragmentBlocks	((features & FEATURE_FRAGMENT_BLOCKS)	? 4 : 0)
	, m_maxSharedBlocks		((features & FEATURE_SHARED_BLOCKS)		? 4 : 0)
	, m_maxInstances		((features & FEATURE_INSTANCE_ARRAYS)	? 3 : 0)
	, m_maxArrayLength		((features & FEATURE_ARRAYS)			? 8 : 0)
	, m_maxStructDepth		((features & FEATURE_STRUCTS)			? 2 : 0)
	, m_maxBlockMembers		(5)
	, m_maxStructMembers	(4)
	, m_seed				(seed)
	, m_blockNdx			(1)
	, m_uniformNdx			(1)
	, m_structNdx			(1)
{
	de::Random rnd(m_seed);

	int numShared		= m_maxSharedBlocks				> 0	? rnd.getInt(1, m_maxSharedBlocks)				: 0;
	int numVtxBlocks	= m_maxVertexBlocks-numShared	> 0	? rnd.getInt(1, m_maxVertexBlocks - numShared)	: 0;
	int	numFragBlocks	= m_maxFragmentBlocks-numShared	> 0 ? rnd.getInt(1, m_maxFragmentBlocks - numShared): 0;

	for (int ndx = 0; ndx < numShared; ndx++)
		generateBlock(rnd, DECLARE_VERTEX | DECLARE_FRAGMENT);

	for (int ndx = 0; ndx < numVtxBlocks; ndx++)
		generateBlock(rnd, DECLARE_VERTEX);

	for (int ndx = 0; ndx < numFragBlocks; ndx++)
		generateBlock(rnd, DECLARE_FRAGMENT);

	init();
}

void RandomUniformBlockCase::generateBlock (de::Random& rnd, deUint32 layoutFlags)
{
	DE_ASSERT(m_blockNdx <= 'z' - 'a');

	const float		instanceArrayWeight	= 0.3f;
	UniformBlock&	block				= m_interface.allocBlock(std::string("Block") + (char)('A' + m_blockNdx));
	int				numInstances		= (m_maxInstances > 0 && rnd.getFloat() < instanceArrayWeight) ? rnd.getInt(0, m_maxInstances) : 0;
	int				numUniforms			= rnd.getInt(1, m_maxBlockMembers);

	if (numInstances > 0)
		block.setArraySize(numInstances);

	if (numInstances > 0 || rnd.getBool())
		block.setInstanceName(std::string("block") + (char)('A' + m_blockNdx));

	// Layout flag candidates.
	std::vector<deUint32> layoutFlagCandidates;
	layoutFlagCandidates.push_back(0);

	if (m_features & FEATURE_STD140_LAYOUT)
		layoutFlagCandidates.push_back(LAYOUT_STD140);

	layoutFlags |= rnd.choose<deUint32>(layoutFlagCandidates.begin(), layoutFlagCandidates.end());

	if (m_features & FEATURE_MATRIX_LAYOUT)
	{
		static const deUint32 matrixCandidates[] = { 0, LAYOUT_ROW_MAJOR, LAYOUT_COLUMN_MAJOR };
		layoutFlags |= rnd.choose<deUint32>(&matrixCandidates[0], &matrixCandidates[DE_LENGTH_OF_ARRAY(matrixCandidates)]);
	}

	block.setFlags(layoutFlags);

	for (int ndx = 0; ndx < numUniforms; ndx++)
		generateUniform(rnd, block);

	m_blockNdx += 1;
}

void RandomUniformBlockCase::generateUniform (de::Random& rnd, UniformBlock& block)
{
	const float		unusedVtxWeight		= 0.15f;
	const float		unusedFragWeight	= 0.15f;
	bool			unusedOk			= (m_features & FEATURE_UNUSED_UNIFORMS) != 0;
	deUint32		flags				= 0;
	std::string		name				= genName('a', 'z', m_uniformNdx);
	VarType			type				= generateType(rnd, 0, true);

	flags |= (unusedOk && rnd.getFloat() < unusedVtxWeight)		? UNUSED_VERTEX		: 0;
	flags |= (unusedOk && rnd.getFloat() < unusedFragWeight)	? UNUSED_FRAGMENT	: 0;

	block.addUniform(Uniform(name, type, flags));

	m_uniformNdx += 1;
}

VarType RandomUniformBlockCase::generateType (de::Random& rnd, int typeDepth, bool arrayOk)
{
	const float structWeight	= 0.1f;
	const float arrayWeight		= 0.1f;

	if (typeDepth < m_maxStructDepth && rnd.getFloat() < structWeight)
	{
		const float				unusedVtxWeight		= 0.15f;
		const float				unusedFragWeight	= 0.15f;
		bool					unusedOk			= (m_features & FEATURE_UNUSED_MEMBERS) != 0;
		std::vector<VarType>	memberTypes;
		int						numMembers = rnd.getInt(1, m_maxStructMembers);

		// Generate members first so nested struct declarations are in correct order.
		for (int ndx = 0; ndx < numMembers; ndx++)
			memberTypes.push_back(generateType(rnd, typeDepth+1, true));

		StructType& structType = m_interface.allocStruct(std::string("s") + genName('A', 'Z', m_structNdx));
		m_structNdx += 1;

		DE_ASSERT(numMembers <= 'Z' - 'A');
		for (int ndx = 0; ndx < numMembers; ndx++)
		{
			deUint32 flags = 0;

			flags |= (unusedOk && rnd.getFloat() < unusedVtxWeight)		? UNUSED_VERTEX		: 0;
			flags |= (unusedOk && rnd.getFloat() < unusedFragWeight)	? UNUSED_FRAGMENT	: 0;

			structType.addMember(std::string("m") + (char)('A' + ndx), memberTypes[ndx], flags);
		}

		return VarType(&structType);
	}
	else if (m_maxArrayLength > 0 && arrayOk && rnd.getFloat() < arrayWeight)
	{
		const bool	arraysOfArraysOk	= (m_features & FEATURE_ARRAYS_OF_ARRAYS) != 0;
		const int	arrayLength			= rnd.getInt(1, m_maxArrayLength);
		VarType		elementType			= generateType(rnd, typeDepth, arraysOfArraysOk);
		return VarType(elementType, arrayLength);
	}
	else
	{
		std::vector<glu::DataType> typeCandidates;

		typeCandidates.push_back(glu::TYPE_FLOAT);
		typeCandidates.push_back(glu::TYPE_INT);
		typeCandidates.push_back(glu::TYPE_UINT);
		typeCandidates.push_back(glu::TYPE_BOOL);

		if (m_features & FEATURE_VECTORS)
		{
			typeCandidates.push_back(glu::TYPE_FLOAT_VEC2);
			typeCandidates.push_back(glu::TYPE_FLOAT_VEC3);
			typeCandidates.push_back(glu::TYPE_FLOAT_VEC4);
			typeCandidates.push_back(glu::TYPE_INT_VEC2);
			typeCandidates.push_back(glu::TYPE_INT_VEC3);
			typeCandidates.push_back(glu::TYPE_INT_VEC4);
			typeCandidates.push_back(glu::TYPE_UINT_VEC2);
			typeCandidates.push_back(glu::TYPE_UINT_VEC3);
			typeCandidates.push_back(glu::TYPE_UINT_VEC4);
			typeCandidates.push_back(glu::TYPE_BOOL_VEC2);
			typeCandidates.push_back(glu::TYPE_BOOL_VEC3);
			typeCandidates.push_back(glu::TYPE_BOOL_VEC4);
		}

		if (m_features & FEATURE_MATRICES)
		{
			typeCandidates.push_back(glu::TYPE_FLOAT_MAT2);
			typeCandidates.push_back(glu::TYPE_FLOAT_MAT2X3);
			typeCandidates.push_back(glu::TYPE_FLOAT_MAT3X2);
			typeCandidates.push_back(glu::TYPE_FLOAT_MAT3);
			typeCandidates.push_back(glu::TYPE_FLOAT_MAT3X4);
			typeCandidates.push_back(glu::TYPE_FLOAT_MAT4X2);
			typeCandidates.push_back(glu::TYPE_FLOAT_MAT4X3);
			typeCandidates.push_back(glu::TYPE_FLOAT_MAT4);
		}

		glu::DataType	type	= rnd.choose<glu::DataType>(typeCandidates.begin(), typeCandidates.end());
		deUint32		flags	= 0;

		if (!glu::isDataTypeBoolOrBVec(type))
		{
			// Precision.
			static const deUint32 precisionCandidates[] = { PRECISION_LOW, PRECISION_MEDIUM, PRECISION_HIGH };
			flags |= rnd.choose<deUint32>(&precisionCandidates[0], &precisionCandidates[DE_LENGTH_OF_ARRAY(precisionCandidates)]);
		}

		return VarType(type, flags);
	}
}

} // ubo
} // vkt
