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
 * \brief SSBO layout tests.
 *//*--------------------------------------------------------------------*/

#include "vktSSBOLayoutTests.hpp"
#include "vktSSBOLayoutCase.hpp"

#include "deUniquePtr.hpp"
#include "tcuCommandLine.hpp"
#include "deRandom.hpp"
#include "deStringUtil.hpp"
#include "deString.h"

namespace vkt
{
namespace ssbo
{
namespace
{

using std::string;
using std::vector;
using glu::VarType;
using glu::StructType;

enum FeatureBits
{
	FEATURE_VECTORS				= (1<<0),
	FEATURE_MATRICES			= (1<<1),
	FEATURE_ARRAYS				= (1<<2),
	FEATURE_STRUCTS				= (1<<3),
	FEATURE_NESTED_STRUCTS		= (1<<4),
	FEATURE_INSTANCE_ARRAYS		= (1<<5),
	FEATURE_UNUSED_VARS			= (1<<6),
	FEATURE_UNUSED_MEMBERS		= (1<<7),
	FEATURE_STD140_LAYOUT		= (1<<8),
	FEATURE_STD430_LAYOUT		= (1<<9),
	FEATURE_MATRIX_LAYOUT		= (1<<10),	//!< Matrix layout flags.
	FEATURE_UNSIZED_ARRAYS		= (1<<11),
	FEATURE_ARRAYS_OF_ARRAYS	= (1<<12)
};

class RandomSSBOLayoutCase : public SSBOLayoutCase
{
public:

							RandomSSBOLayoutCase		(tcu::TestContext& testCtx, const char* name, const char* description, BufferMode bufferMode, deUint32 features, deUint32 seed);

private:
	void					generateBlock				(de::Random& rnd, deUint32 layoutFlags);
	void					generateBufferVar			(de::Random& rnd, BufferBlock& block, bool isLastMember);
	glu::VarType			generateType				(de::Random& rnd, int typeDepth, bool arrayOk, bool unusedArrayOk);

	deUint32				m_features;
	int						m_maxBlocks;
	int						m_maxInstances;
	int						m_maxArrayLength;
	int						m_maxStructDepth;
	int						m_maxBlockMembers;
	int						m_maxStructMembers;
	deUint32				m_seed;

	int						m_blockNdx;
	int						m_bufferVarNdx;
	int						m_structNdx;
};

RandomSSBOLayoutCase::RandomSSBOLayoutCase (tcu::TestContext& testCtx, const char* name, const char* description, BufferMode bufferMode, deUint32 features, deUint32 seed)
	: SSBOLayoutCase		(testCtx, name, description, bufferMode)
	, m_features			(features)
	, m_maxBlocks			(4)
	, m_maxInstances		((features & FEATURE_INSTANCE_ARRAYS)	? 3 : 0)
	, m_maxArrayLength		((features & FEATURE_ARRAYS)			? 8 : 1)
	, m_maxStructDepth		((features & FEATURE_STRUCTS)			? 2 : 0)
	, m_maxBlockMembers		(5)
	, m_maxStructMembers	(4)
	, m_seed				(seed)
	, m_blockNdx			(1)
	, m_bufferVarNdx		(1)
	, m_structNdx			(1)
{
	de::Random rnd(m_seed);

	const int numBlocks = rnd.getInt(1, m_maxBlocks);

	for (int ndx = 0; ndx < numBlocks; ndx++)
		generateBlock(rnd, 0);

	init();
}

void RandomSSBOLayoutCase::generateBlock (de::Random& rnd, deUint32 layoutFlags)
{
	DE_ASSERT(m_blockNdx <= 'z' - 'a');

	const float		instanceArrayWeight	= 0.3f;
	BufferBlock&	block				= m_interface.allocBlock((string("Block") + (char)('A' + m_blockNdx)).c_str());
	int				numInstances		= (m_maxInstances > 0 && rnd.getFloat() < instanceArrayWeight) ? rnd.getInt(0, m_maxInstances) : 0;
	int				numVars				= rnd.getInt(1, m_maxBlockMembers);

	if (numInstances > 0)
		block.setArraySize(numInstances);

	if (numInstances > 0 || rnd.getBool())
		block.setInstanceName((string("block") + (char)('A' + m_blockNdx)).c_str());

	// Layout flag candidates.
	vector<deUint32> layoutFlagCandidates;
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

	for (int ndx = 0; ndx < numVars; ndx++)
		generateBufferVar(rnd, block, (ndx+1 == numVars));

	if (numVars > 0)
	{
		const BufferVar&	lastVar			= *(block.end()-1);
		const glu::VarType&	lastType		= lastVar.getType();
		const bool			isUnsizedArr	= lastType.isArrayType() && (lastType.getArraySize() == glu::VarType::UNSIZED_ARRAY);

		if (isUnsizedArr)
		{
			for (int instanceNdx = 0; instanceNdx < (numInstances ? numInstances : 1); instanceNdx++)
			{
				const int arrSize = rnd.getInt(1, m_maxArrayLength);
				block.setLastUnsizedArraySize(instanceNdx, arrSize);
			}
		}
	}

	m_blockNdx += 1;
}

static std::string genName (char first, char last, int ndx)
{
	std::string	str			= "";
	int			alphabetLen	= last - first + 1;

	while (ndx > alphabetLen)
	{
		str.insert(str.begin(), (char)(first + ((ndx-1)%alphabetLen)));
		ndx = ((ndx-1) / alphabetLen);
	}

	str.insert(str.begin(), (char)(first + (ndx%(alphabetLen+1)) - 1));

	return str;
}

void RandomSSBOLayoutCase::generateBufferVar (de::Random& rnd, BufferBlock& block, bool isLastMember)
{
	const float			readWeight			= 0.7f;
	const float			writeWeight			= 0.7f;
	const float			accessWeight		= 0.85f;
	const bool			unusedOk			= (m_features & FEATURE_UNUSED_VARS) != 0;
	const std::string	name				= genName('a', 'z', m_bufferVarNdx);
	const glu::VarType	type				= generateType(rnd, 0, true, isLastMember && (m_features & FEATURE_UNSIZED_ARRAYS));
	const bool			access				= !unusedOk || (rnd.getFloat() < accessWeight);
	const bool			read				= access ? (rnd.getFloat() < readWeight) : false;
	const bool			write				= access ? (!read || (rnd.getFloat() < writeWeight)) : false;
	const deUint32		flags				= (read ? ACCESS_READ : 0) | (write ? ACCESS_WRITE : 0);

	block.addMember(BufferVar(name.c_str(), type, flags));

	m_bufferVarNdx += 1;
}

glu::VarType RandomSSBOLayoutCase::generateType (de::Random& rnd, int typeDepth, bool arrayOk, bool unsizedArrayOk)
{
	const float structWeight		= 0.1f;
	const float arrayWeight			= 0.1f;
	const float	unsizedArrayWeight	= 0.8f;

	DE_ASSERT(arrayOk || !unsizedArrayOk);

	if (unsizedArrayOk && (rnd.getFloat() < unsizedArrayWeight))
	{
		const bool			childArrayOk	= (m_features & FEATURE_ARRAYS_OF_ARRAYS) != 0;
		const glu::VarType	elementType		= generateType(rnd, typeDepth, childArrayOk, false);
		return glu::VarType(elementType, glu::VarType::UNSIZED_ARRAY);
	}
	else if (typeDepth < m_maxStructDepth && rnd.getFloat() < structWeight)
	{
		vector<glu::VarType>	memberTypes;
		int						numMembers = rnd.getInt(1, m_maxStructMembers);

		// Generate members first so nested struct declarations are in correct order.
		for (int ndx = 0; ndx < numMembers; ndx++)
			memberTypes.push_back(generateType(rnd, typeDepth+1, true, false));

		glu::StructType& structType = m_interface.allocStruct((string("s") + genName('A', 'Z', m_structNdx)).c_str());
		m_structNdx += 1;

		DE_ASSERT(numMembers <= 'Z' - 'A');
		for (int ndx = 0; ndx < numMembers; ndx++)
		{
			structType.addMember((string("m") + (char)('A' + ndx)).c_str(), memberTypes[ndx]);
		}

		return glu::VarType(&structType);
	}
	else if (m_maxArrayLength > 0 && arrayOk && rnd.getFloat() < arrayWeight)
	{
		const int			arrayLength		= rnd.getInt(1, m_maxArrayLength);
		const bool			childArrayOk	= (m_features & FEATURE_ARRAYS_OF_ARRAYS) != 0;
		const glu::VarType	elementType		= generateType(rnd, typeDepth, childArrayOk, false);

		return glu::VarType(elementType, arrayLength);
	}
	else
	{
		vector<glu::DataType> typeCandidates;

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

		glu::DataType	type		= rnd.choose<glu::DataType>(typeCandidates.begin(), typeCandidates.end());
		glu::Precision	precision;

		if (!glu::isDataTypeBoolOrBVec(type))
		{
			// Precision.
			static const glu::Precision precisionCandidates[] = { glu::PRECISION_LOWP, glu::PRECISION_MEDIUMP, glu::PRECISION_HIGHP };
			precision = rnd.choose<glu::Precision>(&precisionCandidates[0], &precisionCandidates[DE_LENGTH_OF_ARRAY(precisionCandidates)]);
		}
		else
			precision = glu::PRECISION_LAST;

		return glu::VarType(type, precision);
	}
}

class BlockBasicTypeCase : public SSBOLayoutCase
{
public:
	BlockBasicTypeCase (tcu::TestContext& testCtx, const char* name, const char* description, const VarType& type, deUint32 layoutFlags, int numInstances)
		: SSBOLayoutCase(testCtx, name, description, BUFFERMODE_PER_BLOCK)
	{
		BufferBlock& block = m_interface.allocBlock("Block");
		block.addMember(BufferVar("var", type, ACCESS_READ|ACCESS_WRITE));
		block.setFlags(layoutFlags);

		if (numInstances > 0)
		{
			block.setArraySize(numInstances);
			block.setInstanceName("block");
		}

		init();
	}
};

class BlockBasicUnsizedArrayCase : public SSBOLayoutCase
{
public:
	BlockBasicUnsizedArrayCase (tcu::TestContext& testCtx, const char* name, const char* description, const VarType& elementType, int arraySize, deUint32 layoutFlags)
		: SSBOLayoutCase(testCtx, name, description, BUFFERMODE_PER_BLOCK)
	{
		BufferBlock& block = m_interface.allocBlock("Block");
		block.addMember(BufferVar("var", VarType(elementType, VarType::UNSIZED_ARRAY), ACCESS_READ|ACCESS_WRITE));
		block.setFlags(layoutFlags);

		block.setLastUnsizedArraySize(0, arraySize);

		init();
	}
};

static void createRandomCaseGroup (tcu::TestCaseGroup* parentGroup, tcu::TestContext& testCtx, const char* groupName, const char* description, SSBOLayoutCase::BufferMode bufferMode, deUint32 features, int numCases, deUint32 baseSeed)
{
	tcu::TestCaseGroup* group = new tcu::TestCaseGroup(testCtx, groupName, description);
	parentGroup->addChild(group);

	baseSeed += (deUint32)testCtx.getCommandLine().getBaseSeed();

	for (int ndx = 0; ndx < numCases; ndx++)
		group->addChild(new RandomSSBOLayoutCase(testCtx, de::toString(ndx).c_str(), "", bufferMode, features, (deUint32)ndx+baseSeed));
}

class BlockSingleStructCase : public SSBOLayoutCase
{
public:
	BlockSingleStructCase (tcu::TestContext& testCtx, const char* name, const char* description, deUint32 layoutFlags, BufferMode bufferMode, int numInstances)
		: SSBOLayoutCase	(testCtx, name, description, bufferMode)
		, m_layoutFlags		(layoutFlags)
		, m_numInstances	(numInstances)
	{
		StructType& typeS = m_interface.allocStruct("S");
		typeS.addMember("a", VarType(glu::TYPE_INT_VEC3, glu::PRECISION_HIGHP)); // \todo [pyry] First member is unused.
		typeS.addMember("b", VarType(VarType(glu::TYPE_FLOAT_MAT3, glu::PRECISION_MEDIUMP), 4));
		typeS.addMember("c", VarType(glu::TYPE_FLOAT_VEC4, glu::PRECISION_HIGHP));

		BufferBlock& block = m_interface.allocBlock("Block");
		block.addMember(BufferVar("s", VarType(&typeS), ACCESS_READ|ACCESS_WRITE));
		block.setFlags(m_layoutFlags);

		if (m_numInstances > 0)
		{
			block.setInstanceName("block");
			block.setArraySize(m_numInstances);
		}

		init();
	}

private:
	deUint32	m_layoutFlags;
	int			m_numInstances;
};

class BlockSingleStructArrayCase : public SSBOLayoutCase
{
public:
	BlockSingleStructArrayCase (tcu::TestContext& testCtx, const char* name, const char* description, deUint32 layoutFlags, BufferMode bufferMode, int numInstances)
		: SSBOLayoutCase	(testCtx, name, description, bufferMode)
		, m_layoutFlags		(layoutFlags)
		, m_numInstances	(numInstances)
	{
		StructType& typeS = m_interface.allocStruct("S");
		typeS.addMember("a", VarType(glu::TYPE_INT_VEC3, glu::PRECISION_HIGHP)); // \todo [pyry] UNUSED
		typeS.addMember("b", VarType(VarType(glu::TYPE_FLOAT_MAT3, glu::PRECISION_MEDIUMP), 4));
		typeS.addMember("c", VarType(glu::TYPE_FLOAT_VEC4, glu::PRECISION_HIGHP));

		BufferBlock& block = m_interface.allocBlock("Block");
		block.addMember(BufferVar("u", VarType(glu::TYPE_UINT, glu::PRECISION_LOWP), 0 /* no access */));
		block.addMember(BufferVar("s", VarType(VarType(&typeS), 3), ACCESS_READ|ACCESS_WRITE));
		block.addMember(BufferVar("v", VarType(glu::TYPE_FLOAT_VEC4, glu::PRECISION_MEDIUMP), ACCESS_WRITE));
		block.setFlags(m_layoutFlags);

		if (m_numInstances > 0)
		{
			block.setInstanceName("block");
			block.setArraySize(m_numInstances);
		}

		init();
	}

private:
	deUint32	m_layoutFlags;
	int			m_numInstances;
};

class BlockSingleNestedStructCase : public SSBOLayoutCase
{
public:
	BlockSingleNestedStructCase (tcu::TestContext& testCtx, const char* name, const char* description, deUint32 layoutFlags, BufferMode bufferMode, int numInstances)
		: SSBOLayoutCase	(testCtx, name, description, bufferMode)
		, m_layoutFlags		(layoutFlags)
		, m_numInstances	(numInstances)
	{
		StructType& typeS = m_interface.allocStruct("S");
		typeS.addMember("a", VarType(glu::TYPE_INT_VEC3, glu::PRECISION_HIGHP));
		typeS.addMember("b", VarType(VarType(glu::TYPE_FLOAT_MAT3, glu::PRECISION_MEDIUMP), 4));
		typeS.addMember("c", VarType(glu::TYPE_FLOAT_VEC4, glu::PRECISION_HIGHP)); // \todo [pyry] UNUSED

		StructType& typeT = m_interface.allocStruct("T");
		typeT.addMember("a", VarType(glu::TYPE_FLOAT_MAT3, glu::PRECISION_MEDIUMP));
		typeT.addMember("b", VarType(&typeS));

		BufferBlock& block = m_interface.allocBlock("Block");
		block.addMember(BufferVar("s", VarType(&typeS), ACCESS_READ));
		block.addMember(BufferVar("v", VarType(glu::TYPE_FLOAT_VEC2, glu::PRECISION_LOWP), 0 /* no access */));
		block.addMember(BufferVar("t", VarType(&typeT), ACCESS_READ|ACCESS_WRITE));
		block.addMember(BufferVar("u", VarType(glu::TYPE_UINT, glu::PRECISION_HIGHP), ACCESS_WRITE));
		block.setFlags(m_layoutFlags);

		if (m_numInstances > 0)
		{
			block.setInstanceName("block");
			block.setArraySize(m_numInstances);
		}

		init();
	}

private:
	deUint32	m_layoutFlags;
	int			m_numInstances;
};

class BlockSingleNestedStructArrayCase : public SSBOLayoutCase
{
public:
	BlockSingleNestedStructArrayCase (tcu::TestContext& testCtx, const char* name, const char* description, deUint32 layoutFlags, BufferMode bufferMode, int numInstances)
		: SSBOLayoutCase	(testCtx, name, description, bufferMode)
		, m_layoutFlags		(layoutFlags)
		, m_numInstances	(numInstances)
	{
		StructType& typeS = m_interface.allocStruct("S");
		typeS.addMember("a", VarType(glu::TYPE_INT_VEC3, glu::PRECISION_HIGHP));
		typeS.addMember("b", VarType(VarType(glu::TYPE_INT_VEC2, glu::PRECISION_MEDIUMP), 4));
		typeS.addMember("c", VarType(glu::TYPE_FLOAT_VEC4, glu::PRECISION_HIGHP)); // \todo [pyry] UNUSED

		StructType& typeT = m_interface.allocStruct("T");
		typeT.addMember("a", VarType(glu::TYPE_FLOAT_MAT3, glu::PRECISION_MEDIUMP));
		typeT.addMember("b", VarType(VarType(&typeS), 3));

		BufferBlock& block = m_interface.allocBlock("Block");
		block.addMember(BufferVar("s", VarType(&typeS), ACCESS_WRITE));
		block.addMember(BufferVar("v", VarType(glu::TYPE_FLOAT_VEC2, glu::PRECISION_LOWP), 0 /* no access */));
		block.addMember(BufferVar("t", VarType(VarType(&typeT), 2), ACCESS_READ));
		block.addMember(BufferVar("u", VarType(glu::TYPE_UINT, glu::PRECISION_HIGHP), ACCESS_READ|ACCESS_WRITE));
		block.setFlags(m_layoutFlags);

		if (m_numInstances > 0)
		{
			block.setInstanceName("block");
			block.setArraySize(m_numInstances);
		}

		init();
	}

private:
	deUint32	m_layoutFlags;
	int			m_numInstances;
};

class BlockUnsizedStructArrayCase : public SSBOLayoutCase
{
public:
	BlockUnsizedStructArrayCase (tcu::TestContext& testCtx, const char* name, const char* description, deUint32 layoutFlags, BufferMode bufferMode, int numInstances)
		: SSBOLayoutCase	(testCtx, name, description, bufferMode)
		, m_layoutFlags		(layoutFlags)
		, m_numInstances	(numInstances)
	{
		StructType& typeS = m_interface.allocStruct("S");
		typeS.addMember("a", VarType(glu::TYPE_UINT_VEC2, glu::PRECISION_HIGHP)); // \todo [pyry] UNUSED
		typeS.addMember("b", VarType(VarType(glu::TYPE_FLOAT_MAT2X4, glu::PRECISION_MEDIUMP), 4));
		typeS.addMember("c", VarType(glu::TYPE_FLOAT_VEC3, glu::PRECISION_HIGHP));

		BufferBlock& block = m_interface.allocBlock("Block");
		block.addMember(BufferVar("u", VarType(glu::TYPE_FLOAT_VEC2, glu::PRECISION_LOWP), 0 /* no access */));
		block.addMember(BufferVar("v", VarType(glu::TYPE_UINT, glu::PRECISION_MEDIUMP), ACCESS_WRITE));
		block.addMember(BufferVar("s", VarType(VarType(&typeS), VarType::UNSIZED_ARRAY), ACCESS_READ|ACCESS_WRITE));
		block.setFlags(m_layoutFlags);

		if (m_numInstances > 0)
		{
			block.setInstanceName("block");
			block.setArraySize(m_numInstances);
		}

		{
			de::Random rnd(246);
			for (int ndx = 0; ndx < (m_numInstances ? m_numInstances : 1); ndx++)
			{
				const int lastArrayLen = rnd.getInt(1, 5);
				block.setLastUnsizedArraySize(ndx, lastArrayLen);
			}
		}

		init();
	}

private:
	deUint32	m_layoutFlags;
	int			m_numInstances;
};

class Block2LevelUnsizedStructArrayCase : public SSBOLayoutCase
{
public:
	Block2LevelUnsizedStructArrayCase (tcu::TestContext& testCtx, const char* name, const char* description, deUint32 layoutFlags, BufferMode bufferMode, int numInstances)
		: SSBOLayoutCase	(testCtx, name, description, bufferMode)
		, m_layoutFlags		(layoutFlags)
		, m_numInstances	(numInstances)
	{
		StructType& typeS = m_interface.allocStruct("S");
		typeS.addMember("a", VarType(glu::TYPE_INT_VEC3, glu::PRECISION_HIGHP));
		typeS.addMember("c", VarType(glu::TYPE_FLOAT_VEC4, glu::PRECISION_HIGHP));

		BufferBlock& block = m_interface.allocBlock("Block");
		block.addMember(BufferVar("u", VarType(glu::TYPE_UINT, glu::PRECISION_LOWP), 0 /* no access */));
		block.addMember(BufferVar("v", VarType(glu::TYPE_FLOAT_VEC4, glu::PRECISION_MEDIUMP), ACCESS_WRITE));
		block.addMember(BufferVar("s", VarType(VarType(VarType(&typeS), 2), VarType::UNSIZED_ARRAY), ACCESS_READ|ACCESS_WRITE));
		block.setFlags(m_layoutFlags);

		if (m_numInstances > 0)
		{
			block.setInstanceName("block");
			block.setArraySize(m_numInstances);
		}

		{
			de::Random rnd(2344);
			for (int ndx = 0; ndx < (m_numInstances ? m_numInstances : 1); ndx++)
			{
				const int lastArrayLen = rnd.getInt(1, 5);
				block.setLastUnsizedArraySize(ndx, lastArrayLen);
			}
		}

		init();
	}

private:
	deUint32	m_layoutFlags;
	int			m_numInstances;
};

class BlockUnsizedNestedStructArrayCase : public SSBOLayoutCase
{
public:
	BlockUnsizedNestedStructArrayCase (tcu::TestContext& testCtx, const char* name, const char* description, deUint32 layoutFlags, BufferMode bufferMode, int numInstances)
		: SSBOLayoutCase	(testCtx, name, description, bufferMode)
		, m_layoutFlags		(layoutFlags)
		, m_numInstances	(numInstances)
	{
		StructType& typeS = m_interface.allocStruct("S");
		typeS.addMember("a", VarType(glu::TYPE_UINT_VEC3, glu::PRECISION_HIGHP));
		typeS.addMember("b", VarType(VarType(glu::TYPE_FLOAT_VEC2, glu::PRECISION_MEDIUMP), 4));
		typeS.addMember("c", VarType(glu::TYPE_FLOAT_VEC4, glu::PRECISION_HIGHP)); // \todo [pyry] UNUSED

		StructType& typeT = m_interface.allocStruct("T");
		typeT.addMember("a", VarType(glu::TYPE_FLOAT_MAT4X3, glu::PRECISION_MEDIUMP));
		typeT.addMember("b", VarType(VarType(&typeS), 3));
		typeT.addMember("c", VarType(glu::TYPE_INT, glu::PRECISION_HIGHP));

		BufferBlock& block = m_interface.allocBlock("Block");
		block.addMember(BufferVar("s", VarType(&typeS), ACCESS_WRITE));
		block.addMember(BufferVar("v", VarType(glu::TYPE_FLOAT_VEC2, glu::PRECISION_LOWP), 0 /* no access */));
		block.addMember(BufferVar("u", VarType(glu::TYPE_UINT, glu::PRECISION_HIGHP), ACCESS_READ|ACCESS_WRITE));
		block.addMember(BufferVar("t", VarType(VarType(&typeT), VarType::UNSIZED_ARRAY), ACCESS_READ));
		block.setFlags(m_layoutFlags);

		if (m_numInstances > 0)
		{
			block.setInstanceName("block");
			block.setArraySize(m_numInstances);
		}

		{
			de::Random rnd(7921);
			for (int ndx = 0; ndx < (m_numInstances ? m_numInstances : 1); ndx++)
			{
				const int lastArrayLen = rnd.getInt(1, 5);
				block.setLastUnsizedArraySize(ndx, lastArrayLen);
			}
		}

		init();
	}

private:
	deUint32	m_layoutFlags;
	int			m_numInstances;
};

class BlockMultiBasicTypesCase : public SSBOLayoutCase
{
public:
	BlockMultiBasicTypesCase (tcu::TestContext& testCtx, const char* name, const char* description, deUint32 flagsA, deUint32 flagsB, BufferMode bufferMode, int numInstances)
		: SSBOLayoutCase	(testCtx, name, description, bufferMode)
		, m_flagsA			(flagsA)
		, m_flagsB			(flagsB)
		, m_numInstances	(numInstances)
	{
		BufferBlock& blockA = m_interface.allocBlock("BlockA");
		blockA.addMember(BufferVar("a", VarType(glu::TYPE_FLOAT, glu::PRECISION_HIGHP), ACCESS_READ|ACCESS_WRITE));
		blockA.addMember(BufferVar("b", VarType(glu::TYPE_UINT_VEC3, glu::PRECISION_LOWP), 0 /* no access */));
		blockA.addMember(BufferVar("c", VarType(glu::TYPE_FLOAT_MAT2, glu::PRECISION_MEDIUMP), ACCESS_READ));
		blockA.setInstanceName("blockA");
		blockA.setFlags(m_flagsA);

		BufferBlock& blockB = m_interface.allocBlock("BlockB");
		blockB.addMember(BufferVar("a", VarType(glu::TYPE_FLOAT_MAT3, glu::PRECISION_MEDIUMP), ACCESS_WRITE));
		blockB.addMember(BufferVar("b", VarType(glu::TYPE_INT_VEC2, glu::PRECISION_LOWP), ACCESS_READ));
		blockB.addMember(BufferVar("c", VarType(glu::TYPE_FLOAT_VEC4, glu::PRECISION_HIGHP), 0 /* no access */));
		blockB.addMember(BufferVar("d", VarType(glu::TYPE_BOOL, glu::PRECISION_LAST), ACCESS_READ|ACCESS_WRITE));
		blockB.setInstanceName("blockB");
		blockB.setFlags(m_flagsB);

		if (m_numInstances > 0)
		{
			blockA.setArraySize(m_numInstances);
			blockB.setArraySize(m_numInstances);
		}

		init();
	}

private:
	deUint32	m_flagsA;
	deUint32	m_flagsB;
	int			m_numInstances;
};

class BlockMultiNestedStructCase : public SSBOLayoutCase
{
public:
	BlockMultiNestedStructCase (tcu::TestContext& testCtx, const char* name, const char* description, deUint32 flagsA, deUint32 flagsB, BufferMode bufferMode, int numInstances)
		: SSBOLayoutCase	(testCtx, name, description, bufferMode)
		, m_flagsA			(flagsA)
		, m_flagsB			(flagsB)
		, m_numInstances	(numInstances)
	{
		StructType& typeS = m_interface.allocStruct("S");
		typeS.addMember("a", VarType(glu::TYPE_FLOAT_MAT3, glu::PRECISION_LOWP));
		typeS.addMember("b", VarType(VarType(glu::TYPE_INT_VEC2, glu::PRECISION_MEDIUMP), 4));
		typeS.addMember("c", VarType(glu::TYPE_FLOAT_VEC4, glu::PRECISION_HIGHP));

		StructType& typeT = m_interface.allocStruct("T");
		typeT.addMember("a", VarType(glu::TYPE_UINT, glu::PRECISION_MEDIUMP)); // \todo [pyry] UNUSED
		typeT.addMember("b", VarType(&typeS));
		typeT.addMember("c", VarType(glu::TYPE_BOOL_VEC4, glu::PRECISION_LAST));

		BufferBlock& blockA = m_interface.allocBlock("BlockA");
		blockA.addMember(BufferVar("a", VarType(glu::TYPE_FLOAT, glu::PRECISION_HIGHP), ACCESS_READ|ACCESS_WRITE));
		blockA.addMember(BufferVar("b", VarType(&typeS), ACCESS_WRITE));
		blockA.addMember(BufferVar("c", VarType(glu::TYPE_UINT_VEC3, glu::PRECISION_LOWP), 0 /* no access */));
		blockA.setInstanceName("blockA");
		blockA.setFlags(m_flagsA);

		BufferBlock& blockB = m_interface.allocBlock("BlockB");
		blockB.addMember(BufferVar("a", VarType(glu::TYPE_FLOAT_MAT2, glu::PRECISION_MEDIUMP), ACCESS_WRITE));
		blockB.addMember(BufferVar("b", VarType(&typeT), ACCESS_READ|ACCESS_WRITE));
		blockB.addMember(BufferVar("c", VarType(glu::TYPE_BOOL_VEC4, glu::PRECISION_LAST), 0 /* no access */));
		blockB.addMember(BufferVar("d", VarType(glu::TYPE_BOOL, glu::PRECISION_LAST), ACCESS_READ|ACCESS_WRITE));
		blockB.setInstanceName("blockB");
		blockB.setFlags(m_flagsB);

		if (m_numInstances > 0)
		{
			blockA.setArraySize(m_numInstances);
			blockB.setArraySize(m_numInstances);
		}

		init();
	}

private:
	deUint32	m_flagsA;
	deUint32	m_flagsB;
	int			m_numInstances;
};

class SSBOLayoutTests : public tcu::TestCaseGroup
{
public:
							SSBOLayoutTests		(tcu::TestContext& testCtx);
							~SSBOLayoutTests	(void);

	void					init				(void);

private:
							SSBOLayoutTests		(const SSBOLayoutTests& other);
	SSBOLayoutTests&		operator=			(const SSBOLayoutTests& other);
};


SSBOLayoutTests::SSBOLayoutTests (tcu::TestContext& testCtx)
	: TestCaseGroup(testCtx, "layout", "SSBO Layout Tests")
{
}

SSBOLayoutTests::~SSBOLayoutTests (void)
{
}

void SSBOLayoutTests::init (void)
{
	static const glu::DataType basicTypes[] =
	{
		glu::TYPE_FLOAT,
		glu::TYPE_FLOAT_VEC2,
		glu::TYPE_FLOAT_VEC3,
		glu::TYPE_FLOAT_VEC4,
		glu::TYPE_INT,
		glu::TYPE_INT_VEC2,
		glu::TYPE_INT_VEC3,
		glu::TYPE_INT_VEC4,
		glu::TYPE_UINT,
		glu::TYPE_UINT_VEC2,
		glu::TYPE_UINT_VEC3,
		glu::TYPE_UINT_VEC4,
		glu::TYPE_BOOL,
		glu::TYPE_BOOL_VEC2,
		glu::TYPE_BOOL_VEC3,
		glu::TYPE_BOOL_VEC4,
		glu::TYPE_FLOAT_MAT2,
		glu::TYPE_FLOAT_MAT3,
		glu::TYPE_FLOAT_MAT4,
		glu::TYPE_FLOAT_MAT2X3,
		glu::TYPE_FLOAT_MAT2X4,
		glu::TYPE_FLOAT_MAT3X2,
		glu::TYPE_FLOAT_MAT3X4,
		glu::TYPE_FLOAT_MAT4X2,
		glu::TYPE_FLOAT_MAT4X3
	};

	static const struct
	{
		const char*		name;
		deUint32		flags;
	} layoutFlags[] =
	{
		{ "std140",	LAYOUT_STD140 },
		{ "std430",	LAYOUT_STD430 },
	};

	static const struct
	{
		const char*		name;
		deUint32		flags;
	} matrixFlags[] =
	{
		{ "row_major",		LAYOUT_ROW_MAJOR	},
		{ "column_major",	LAYOUT_COLUMN_MAJOR }
	};

	static const struct
	{
		const char*							name;
		SSBOLayoutCase::BufferMode		mode;
	} bufferModes[] =
	{
		{ "per_block_buffer",	SSBOLayoutCase::BUFFERMODE_PER_BLOCK },
		{ "single_buffer",		SSBOLayoutCase::BUFFERMODE_SINGLE	}
	};

	// ubo.single_basic_type
	{
		tcu::TestCaseGroup* singleBasicTypeGroup = new tcu::TestCaseGroup(m_testCtx, "single_basic_type", "Single basic variable in single buffer");
		addChild(singleBasicTypeGroup);

		for (int layoutFlagNdx = 0; layoutFlagNdx < DE_LENGTH_OF_ARRAY(layoutFlags); layoutFlagNdx++)
		{
			tcu::TestCaseGroup* layoutGroup = new tcu::TestCaseGroup(m_testCtx, layoutFlags[layoutFlagNdx].name, "");
			singleBasicTypeGroup->addChild(layoutGroup);

			for (int basicTypeNdx = 0; basicTypeNdx < DE_LENGTH_OF_ARRAY(basicTypes); basicTypeNdx++)
			{
				glu::DataType	type		= basicTypes[basicTypeNdx];
				const char*		typeName	= glu::getDataTypeName(type);

				if (glu::isDataTypeBoolOrBVec(type))
					layoutGroup->addChild(new BlockBasicTypeCase(m_testCtx, typeName, "", VarType(type, glu::PRECISION_LAST), layoutFlags[layoutFlagNdx].flags, 0));
				else
				{
					for (int precNdx = 0; precNdx < glu::PRECISION_LAST; precNdx++)
					{
						const glu::Precision	precision	= glu::Precision(precNdx);
						const string			caseName	= string(glu::getPrecisionName(precision)) + "_" + typeName;

						layoutGroup->addChild(new BlockBasicTypeCase(m_testCtx, caseName.c_str(), "", VarType(type, precision), layoutFlags[layoutFlagNdx].flags, 0));
					}
				}

				if (glu::isDataTypeMatrix(type))
				{
					for (int matFlagNdx = 0; matFlagNdx < DE_LENGTH_OF_ARRAY(matrixFlags); matFlagNdx++)
					{
						for (int precNdx = 0; precNdx < glu::PRECISION_LAST; precNdx++)
						{
							const glu::Precision	precision	= glu::Precision(precNdx);
							const string			caseName	= string(matrixFlags[matFlagNdx].name) + "_" + string(glu::getPrecisionName(precision)) + "_" + typeName;

							layoutGroup->addChild(new BlockBasicTypeCase(m_testCtx, caseName.c_str(), "", glu::VarType(type, precision), layoutFlags[layoutFlagNdx].flags|matrixFlags[matFlagNdx].flags, 0));
						}
					}
				}
			}
		}
	}

	// ubo.single_basic_array
	{
		tcu::TestCaseGroup* singleBasicArrayGroup = new tcu::TestCaseGroup(m_testCtx, "single_basic_array", "Single basic array variable in single buffer");
		addChild(singleBasicArrayGroup);

		for (int layoutFlagNdx = 0; layoutFlagNdx < DE_LENGTH_OF_ARRAY(layoutFlags); layoutFlagNdx++)
		{
			tcu::TestCaseGroup* layoutGroup = new tcu::TestCaseGroup(m_testCtx, layoutFlags[layoutFlagNdx].name, "");
			singleBasicArrayGroup->addChild(layoutGroup);

			for (int basicTypeNdx = 0; basicTypeNdx < DE_LENGTH_OF_ARRAY(basicTypes); basicTypeNdx++)
			{
				glu::DataType	type		= basicTypes[basicTypeNdx];
				const char*		typeName	= glu::getDataTypeName(type);
				const int		arraySize	= 3;

				layoutGroup->addChild(new BlockBasicTypeCase(m_testCtx, typeName, "",
															 VarType(VarType(type, glu::isDataTypeBoolOrBVec(type) ? glu::PRECISION_LAST : glu::PRECISION_HIGHP), arraySize),
															 layoutFlags[layoutFlagNdx].flags, 0));

				if (glu::isDataTypeMatrix(type))
				{
					for (int matFlagNdx = 0; matFlagNdx < DE_LENGTH_OF_ARRAY(matrixFlags); matFlagNdx++)
						layoutGroup->addChild(new BlockBasicTypeCase(m_testCtx, (string(matrixFlags[matFlagNdx].name) + "_" + typeName).c_str(), "",
																	 VarType(VarType(type, glu::PRECISION_HIGHP), arraySize),
																	 layoutFlags[layoutFlagNdx].flags|matrixFlags[matFlagNdx].flags, 0));
				}
			}
		}
	}

	// ubo.basic_unsized_array
	{
		tcu::TestCaseGroup* basicUnsizedArray = new tcu::TestCaseGroup(m_testCtx, "basic_unsized_array", "Basic unsized array tests");
		addChild(basicUnsizedArray);

		for (int layoutFlagNdx = 0; layoutFlagNdx < DE_LENGTH_OF_ARRAY(layoutFlags); layoutFlagNdx++)
		{
			tcu::TestCaseGroup* layoutGroup = new tcu::TestCaseGroup(m_testCtx, layoutFlags[layoutFlagNdx].name, "");
			basicUnsizedArray->addChild(layoutGroup);

			for (int basicTypeNdx = 0; basicTypeNdx < DE_LENGTH_OF_ARRAY(basicTypes); basicTypeNdx++)
			{
				glu::DataType	type		= basicTypes[basicTypeNdx];
				const char*		typeName	= glu::getDataTypeName(type);
				const int		arraySize	= 19;

				layoutGroup->addChild(new BlockBasicUnsizedArrayCase(m_testCtx, typeName, "",
																	 VarType(type, glu::isDataTypeBoolOrBVec(type) ? glu::PRECISION_LAST : glu::PRECISION_HIGHP),
																	 arraySize, layoutFlags[layoutFlagNdx].flags));

				if (glu::isDataTypeMatrix(type))
				{
					for (int matFlagNdx = 0; matFlagNdx < DE_LENGTH_OF_ARRAY(matrixFlags); matFlagNdx++)
						layoutGroup->addChild(new BlockBasicUnsizedArrayCase(m_testCtx, (string(matrixFlags[matFlagNdx].name) + "_" + typeName).c_str(), "",
																			 VarType(type, glu::PRECISION_HIGHP), arraySize,
																			 layoutFlags[layoutFlagNdx].flags|matrixFlags[matFlagNdx].flags));
				}
			}
		}
	}

	// ubo.2_level_array
	{
		tcu::TestCaseGroup* nestedArrayGroup = new tcu::TestCaseGroup(m_testCtx, "2_level_array", "2-level nested array");
		addChild(nestedArrayGroup);

		for (int layoutFlagNdx = 0; layoutFlagNdx < DE_LENGTH_OF_ARRAY(layoutFlags); layoutFlagNdx++)
		{
			tcu::TestCaseGroup* layoutGroup = new tcu::TestCaseGroup(m_testCtx, layoutFlags[layoutFlagNdx].name, "");
			nestedArrayGroup->addChild(layoutGroup);

			for (int basicTypeNdx = 0; basicTypeNdx < DE_LENGTH_OF_ARRAY(basicTypes); basicTypeNdx++)
			{
				glu::DataType	type		= basicTypes[basicTypeNdx];
				const char*		typeName	= glu::getDataTypeName(type);
				const int		childSize	= 3;
				const int		parentSize	= 4;
				const VarType	childType	(VarType(type, glu::isDataTypeBoolOrBVec(type) ? glu::PRECISION_LAST : glu::PRECISION_HIGHP), childSize);
				const VarType	fullType	(childType, parentSize);

				layoutGroup->addChild(new BlockBasicTypeCase(m_testCtx, typeName, "", fullType, layoutFlags[layoutFlagNdx].flags, 0));

				if (glu::isDataTypeMatrix(type))
				{
					for (int matFlagNdx = 0; matFlagNdx < DE_LENGTH_OF_ARRAY(matrixFlags); matFlagNdx++)
						layoutGroup->addChild(new BlockBasicTypeCase(m_testCtx, (string(matrixFlags[matFlagNdx].name) + "_" + typeName).c_str(), "",
																	 fullType,
																	 layoutFlags[layoutFlagNdx].flags|matrixFlags[matFlagNdx].flags, 0));
				}
			}
		}
	}

	// ubo.3_level_array
	{
		tcu::TestCaseGroup* nestedArrayGroup = new tcu::TestCaseGroup(m_testCtx, "3_level_array", "3-level nested array");
		addChild(nestedArrayGroup);

		for (int layoutFlagNdx = 0; layoutFlagNdx < DE_LENGTH_OF_ARRAY(layoutFlags); layoutFlagNdx++)
		{
			tcu::TestCaseGroup* layoutGroup = new tcu::TestCaseGroup(m_testCtx, layoutFlags[layoutFlagNdx].name, "");
			nestedArrayGroup->addChild(layoutGroup);

			for (int basicTypeNdx = 0; basicTypeNdx < DE_LENGTH_OF_ARRAY(basicTypes); basicTypeNdx++)
			{
				glu::DataType	type		= basicTypes[basicTypeNdx];
				const char*		typeName	= glu::getDataTypeName(type);
				const int		childSize0	= 3;
				const int		childSize1	= 2;
				const int		parentSize	= 4;
				const VarType	childType0	(VarType(type, glu::isDataTypeBoolOrBVec(type) ? glu::PRECISION_LAST : glu::PRECISION_HIGHP), childSize0);
				const VarType	childType1	(childType0, childSize1);
				const VarType	fullType	(childType1, parentSize);

				layoutGroup->addChild(new BlockBasicTypeCase(m_testCtx, typeName, "", fullType, layoutFlags[layoutFlagNdx].flags, 0));

				if (glu::isDataTypeMatrix(type))
				{
					for (int matFlagNdx = 0; matFlagNdx < DE_LENGTH_OF_ARRAY(matrixFlags); matFlagNdx++)
						layoutGroup->addChild(new BlockBasicTypeCase(m_testCtx, (string(matrixFlags[matFlagNdx].name) + "_" + typeName).c_str(), "",
																	 fullType,
																	 layoutFlags[layoutFlagNdx].flags|matrixFlags[matFlagNdx].flags, 0));
				}
			}
		}
	}

	// ubo.3_level_unsized_array
	{
		tcu::TestCaseGroup* nestedArrayGroup = new tcu::TestCaseGroup(m_testCtx, "3_level_unsized_array", "3-level nested array, top-level array unsized");
		addChild(nestedArrayGroup);

		for (int layoutFlagNdx = 0; layoutFlagNdx < DE_LENGTH_OF_ARRAY(layoutFlags); layoutFlagNdx++)
		{
			tcu::TestCaseGroup* layoutGroup = new tcu::TestCaseGroup(m_testCtx, layoutFlags[layoutFlagNdx].name, "");
			nestedArrayGroup->addChild(layoutGroup);

			for (int basicTypeNdx = 0; basicTypeNdx < DE_LENGTH_OF_ARRAY(basicTypes); basicTypeNdx++)
			{
				glu::DataType	type		= basicTypes[basicTypeNdx];
				const char*		typeName	= glu::getDataTypeName(type);
				const int		childSize0	= 2;
				const int		childSize1	= 4;
				const int		parentSize	= 3;
				const VarType	childType0	(VarType(type, glu::isDataTypeBoolOrBVec(type) ? glu::PRECISION_LAST : glu::PRECISION_HIGHP), childSize0);
				const VarType	childType1	(childType0, childSize1);

				layoutGroup->addChild(new BlockBasicUnsizedArrayCase(m_testCtx, typeName, "", childType1, parentSize, layoutFlags[layoutFlagNdx].flags));

				if (glu::isDataTypeMatrix(type))
				{
					for (int matFlagNdx = 0; matFlagNdx < DE_LENGTH_OF_ARRAY(matrixFlags); matFlagNdx++)
						layoutGroup->addChild(new BlockBasicUnsizedArrayCase(m_testCtx, (string(matrixFlags[matFlagNdx].name) + "_" + typeName).c_str(), "",
																			 childType1, parentSize,
																			 layoutFlags[layoutFlagNdx].flags|matrixFlags[matFlagNdx].flags));
				}
			}
		}
	}

	// ubo.single_struct
	{
		tcu::TestCaseGroup* singleStructGroup = new tcu::TestCaseGroup(m_testCtx, "single_struct", "Single struct in uniform block");
		addChild(singleStructGroup);

		for (int modeNdx = 0; modeNdx < DE_LENGTH_OF_ARRAY(bufferModes); modeNdx++)
		{
			tcu::TestCaseGroup* modeGroup = new tcu::TestCaseGroup(m_testCtx, bufferModes[modeNdx].name, "");
			singleStructGroup->addChild(modeGroup);

			for (int layoutFlagNdx = 0; layoutFlagNdx < DE_LENGTH_OF_ARRAY(layoutFlags); layoutFlagNdx++)
			{
				for (int isArray = 0; isArray < 2; isArray++)
				{
					const deUint32	caseFlags	= layoutFlags[layoutFlagNdx].flags;
					string			caseName	= layoutFlags[layoutFlagNdx].name;

					if (bufferModes[modeNdx].mode == SSBOLayoutCase::BUFFERMODE_SINGLE && isArray == 0)
						continue; // Doesn't make sense to add this variant.

					if (isArray)
						caseName += "_instance_array";

					modeGroup->addChild(new BlockSingleStructCase(m_testCtx, caseName.c_str(), "", caseFlags, bufferModes[modeNdx].mode, isArray ? 3 : 0));
				}
			}
		}
	}

	// ubo.single_struct_array
	{
		tcu::TestCaseGroup* singleStructArrayGroup = new tcu::TestCaseGroup(m_testCtx, "single_struct_array", "Struct array in one uniform block");
		addChild(singleStructArrayGroup);

		for (int modeNdx = 0; modeNdx < DE_LENGTH_OF_ARRAY(bufferModes); modeNdx++)
		{
			tcu::TestCaseGroup* modeGroup = new tcu::TestCaseGroup(m_testCtx, bufferModes[modeNdx].name, "");
			singleStructArrayGroup->addChild(modeGroup);

			for (int layoutFlagNdx = 0; layoutFlagNdx < DE_LENGTH_OF_ARRAY(layoutFlags); layoutFlagNdx++)
			{
				for (int isArray = 0; isArray < 2; isArray++)
				{
					std::string	baseName	= layoutFlags[layoutFlagNdx].name;
					deUint32	baseFlags	= layoutFlags[layoutFlagNdx].flags;

					if (bufferModes[modeNdx].mode == SSBOLayoutCase::BUFFERMODE_SINGLE && isArray == 0)
						continue; // Doesn't make sense to add this variant.

					if (isArray)
						baseName += "_instance_array";

					modeGroup->addChild(new BlockSingleStructArrayCase(m_testCtx, baseName.c_str(),	"", baseFlags, bufferModes[modeNdx].mode, isArray ? 3 : 0));
				}
			}
		}
	}

	// ubo.single_nested_struct
	{
		tcu::TestCaseGroup* singleNestedStructGroup = new tcu::TestCaseGroup(m_testCtx, "single_nested_struct", "Nested struct in one uniform block");
		addChild(singleNestedStructGroup);

		for (int modeNdx = 0; modeNdx < DE_LENGTH_OF_ARRAY(bufferModes); modeNdx++)
		{
			tcu::TestCaseGroup* modeGroup = new tcu::TestCaseGroup(m_testCtx, bufferModes[modeNdx].name, "");
			singleNestedStructGroup->addChild(modeGroup);

			for (int layoutFlagNdx = 0; layoutFlagNdx < DE_LENGTH_OF_ARRAY(layoutFlags); layoutFlagNdx++)
			{
				for (int isArray = 0; isArray < 2; isArray++)
				{
					std::string	baseName	= layoutFlags[layoutFlagNdx].name;
					deUint32	baseFlags	= layoutFlags[layoutFlagNdx].flags;

					if (bufferModes[modeNdx].mode == SSBOLayoutCase::BUFFERMODE_SINGLE && isArray == 0)
						continue; // Doesn't make sense to add this variant.

					if (isArray)
						baseName += "_instance_array";

					modeGroup->addChild(new BlockSingleNestedStructCase(m_testCtx, baseName.c_str(), "", baseFlags, bufferModes[modeNdx].mode, isArray ? 3 : 0));
				}
			}
		}
	}

	// ubo.single_nested_struct_array
	{
		tcu::TestCaseGroup* singleNestedStructArrayGroup = new tcu::TestCaseGroup(m_testCtx, "single_nested_struct_array", "Nested struct array in one uniform block");
		addChild(singleNestedStructArrayGroup);

		for (int modeNdx = 0; modeNdx < DE_LENGTH_OF_ARRAY(bufferModes); modeNdx++)
		{
			tcu::TestCaseGroup* modeGroup = new tcu::TestCaseGroup(m_testCtx, bufferModes[modeNdx].name, "");
			singleNestedStructArrayGroup->addChild(modeGroup);

			for (int layoutFlagNdx = 0; layoutFlagNdx < DE_LENGTH_OF_ARRAY(layoutFlags); layoutFlagNdx++)
			{
				for (int isArray = 0; isArray < 2; isArray++)
				{
					std::string	baseName	= layoutFlags[layoutFlagNdx].name;
					deUint32	baseFlags	= layoutFlags[layoutFlagNdx].flags;

					if (bufferModes[modeNdx].mode == SSBOLayoutCase::BUFFERMODE_SINGLE && isArray == 0)
						continue; // Doesn't make sense to add this variant.

					if (isArray)
						baseName += "_instance_array";

					modeGroup->addChild(new BlockSingleNestedStructArrayCase(m_testCtx, baseName.c_str(), "", baseFlags, bufferModes[modeNdx].mode, isArray ? 3 : 0));
				}
			}
		}
	}

	// ubo.unsized_struct_array
	{
		tcu::TestCaseGroup* singleStructArrayGroup = new tcu::TestCaseGroup(m_testCtx, "unsized_struct_array", "Unsized struct array in one uniform block");
		addChild(singleStructArrayGroup);

		for (int modeNdx = 0; modeNdx < DE_LENGTH_OF_ARRAY(bufferModes); modeNdx++)
		{
			tcu::TestCaseGroup* modeGroup = new tcu::TestCaseGroup(m_testCtx, bufferModes[modeNdx].name, "");
			singleStructArrayGroup->addChild(modeGroup);

			for (int layoutFlagNdx = 0; layoutFlagNdx < DE_LENGTH_OF_ARRAY(layoutFlags); layoutFlagNdx++)
			{
				for (int isArray = 0; isArray < 2; isArray++)
				{
					std::string	baseName	= layoutFlags[layoutFlagNdx].name;
					deUint32	baseFlags	= layoutFlags[layoutFlagNdx].flags;

					if (bufferModes[modeNdx].mode == SSBOLayoutCase::BUFFERMODE_SINGLE && isArray == 0)
						continue; // Doesn't make sense to add this variant.

					if (isArray)
						baseName += "_instance_array";

					modeGroup->addChild(new BlockUnsizedStructArrayCase(m_testCtx, baseName.c_str(),	"", baseFlags, bufferModes[modeNdx].mode, isArray ? 3 : 0));
				}
			}
		}
	}

	// ubo.2_level_unsized_struct_array
	{
		tcu::TestCaseGroup* structArrayGroup = new tcu::TestCaseGroup(m_testCtx, "2_level_unsized_struct_array", "Unsized 2-level struct array in one uniform block");
		addChild(structArrayGroup);

		for (int modeNdx = 0; modeNdx < DE_LENGTH_OF_ARRAY(bufferModes); modeNdx++)
		{
			tcu::TestCaseGroup* modeGroup = new tcu::TestCaseGroup(m_testCtx, bufferModes[modeNdx].name, "");
			structArrayGroup->addChild(modeGroup);

			for (int layoutFlagNdx = 0; layoutFlagNdx < DE_LENGTH_OF_ARRAY(layoutFlags); layoutFlagNdx++)
			{
				for (int isArray = 0; isArray < 2; isArray++)
				{
					std::string	baseName	= layoutFlags[layoutFlagNdx].name;
					deUint32	baseFlags	= layoutFlags[layoutFlagNdx].flags;

					if (bufferModes[modeNdx].mode == SSBOLayoutCase::BUFFERMODE_SINGLE && isArray == 0)
						continue; // Doesn't make sense to add this variant.

					if (isArray)
						baseName += "_instance_array";

					modeGroup->addChild(new Block2LevelUnsizedStructArrayCase(m_testCtx, baseName.c_str(),	"", baseFlags, bufferModes[modeNdx].mode, isArray ? 3 : 0));
				}
			}
		}
	}

	// ubo.unsized_nested_struct_array
	{
		tcu::TestCaseGroup* singleNestedStructArrayGroup = new tcu::TestCaseGroup(m_testCtx, "unsized_nested_struct_array", "Unsized, nested struct array in one uniform block");
		addChild(singleNestedStructArrayGroup);

		for (int modeNdx = 0; modeNdx < DE_LENGTH_OF_ARRAY(bufferModes); modeNdx++)
		{
			tcu::TestCaseGroup* modeGroup = new tcu::TestCaseGroup(m_testCtx, bufferModes[modeNdx].name, "");
			singleNestedStructArrayGroup->addChild(modeGroup);

			for (int layoutFlagNdx = 0; layoutFlagNdx < DE_LENGTH_OF_ARRAY(layoutFlags); layoutFlagNdx++)
			{
				for (int isArray = 0; isArray < 2; isArray++)
				{
					std::string	baseName	= layoutFlags[layoutFlagNdx].name;
					deUint32	baseFlags	= layoutFlags[layoutFlagNdx].flags;

					if (bufferModes[modeNdx].mode == SSBOLayoutCase::BUFFERMODE_SINGLE && isArray == 0)
						continue; // Doesn't make sense to add this variant.

					if (isArray)
						baseName += "_instance_array";

					modeGroup->addChild(new BlockUnsizedNestedStructArrayCase(m_testCtx, baseName.c_str(), "", baseFlags, bufferModes[modeNdx].mode, isArray ? 3 : 0));
				}
			}
		}
	}

	// ubo.instance_array_basic_type
	{
		tcu::TestCaseGroup* instanceArrayBasicTypeGroup = new tcu::TestCaseGroup(m_testCtx, "instance_array_basic_type", "Single basic variable in instance array");
		addChild(instanceArrayBasicTypeGroup);

		for (int layoutFlagNdx = 0; layoutFlagNdx < DE_LENGTH_OF_ARRAY(layoutFlags); layoutFlagNdx++)
		{
			tcu::TestCaseGroup* layoutGroup = new tcu::TestCaseGroup(m_testCtx, layoutFlags[layoutFlagNdx].name, "");
			instanceArrayBasicTypeGroup->addChild(layoutGroup);

			for (int basicTypeNdx = 0; basicTypeNdx < DE_LENGTH_OF_ARRAY(basicTypes); basicTypeNdx++)
			{
				glu::DataType	type			= basicTypes[basicTypeNdx];
				const char*		typeName		= glu::getDataTypeName(type);
				const int		numInstances	= 3;

				layoutGroup->addChild(new BlockBasicTypeCase(m_testCtx, typeName, "",
															 VarType(type, glu::isDataTypeBoolOrBVec(type) ? glu::PRECISION_LAST : glu::PRECISION_HIGHP),
															 layoutFlags[layoutFlagNdx].flags, numInstances));

				if (glu::isDataTypeMatrix(type))
				{
					for (int matFlagNdx = 0; matFlagNdx < DE_LENGTH_OF_ARRAY(matrixFlags); matFlagNdx++)
						layoutGroup->addChild(new BlockBasicTypeCase(m_testCtx, (string(matrixFlags[matFlagNdx].name) + "_" + typeName).c_str(), "",
																	 VarType(type, glu::PRECISION_HIGHP), layoutFlags[layoutFlagNdx].flags|matrixFlags[matFlagNdx].flags,
																	 numInstances));
				}
			}
		}
	}

	// ubo.multi_basic_types
	{
		tcu::TestCaseGroup* multiBasicTypesGroup = new tcu::TestCaseGroup(m_testCtx, "multi_basic_types", "Multiple buffers with basic types");
		addChild(multiBasicTypesGroup);

		for (int modeNdx = 0; modeNdx < DE_LENGTH_OF_ARRAY(bufferModes); modeNdx++)
		{
			tcu::TestCaseGroup* modeGroup = new tcu::TestCaseGroup(m_testCtx, bufferModes[modeNdx].name, "");
			multiBasicTypesGroup->addChild(modeGroup);

			for (int layoutFlagNdx = 0; layoutFlagNdx < DE_LENGTH_OF_ARRAY(layoutFlags); layoutFlagNdx++)
			{
				for (int isArray = 0; isArray < 2; isArray++)
				{
					std::string	baseName	= layoutFlags[layoutFlagNdx].name;
					deUint32	baseFlags	= layoutFlags[layoutFlagNdx].flags;

					if (isArray)
						baseName += "_instance_array";

					modeGroup->addChild(new BlockMultiBasicTypesCase(m_testCtx, baseName.c_str(), "", baseFlags, baseFlags, bufferModes[modeNdx].mode, isArray ? 3 : 0));
				}
			}
		}
	}

	// ubo.multi_nested_struct
	{
		tcu::TestCaseGroup* multiNestedStructGroup = new tcu::TestCaseGroup(m_testCtx, "multi_nested_struct", "Multiple buffers with nested structs");
		addChild(multiNestedStructGroup);

		for (int modeNdx = 0; modeNdx < DE_LENGTH_OF_ARRAY(bufferModes); modeNdx++)
		{
			tcu::TestCaseGroup* modeGroup = new tcu::TestCaseGroup(m_testCtx, bufferModes[modeNdx].name, "");
			multiNestedStructGroup->addChild(modeGroup);

			for (int layoutFlagNdx = 0; layoutFlagNdx < DE_LENGTH_OF_ARRAY(layoutFlags); layoutFlagNdx++)
			{
				for (int isArray = 0; isArray < 2; isArray++)
				{
					std::string	baseName	= layoutFlags[layoutFlagNdx].name;
					deUint32	baseFlags	= layoutFlags[layoutFlagNdx].flags;

					if (isArray)
						baseName += "_instance_array";

					modeGroup->addChild(new BlockMultiNestedStructCase(m_testCtx, baseName.c_str(), "", baseFlags, baseFlags, bufferModes[modeNdx].mode, isArray ? 3 : 0));
				}
			}
		}
	}

	// ubo.random
	{
		const deUint32	allLayouts		= FEATURE_STD140_LAYOUT;
		const deUint32	allBasicTypes	= FEATURE_VECTORS|FEATURE_MATRICES;
		const deUint32	unused			= FEATURE_UNUSED_MEMBERS|FEATURE_UNUSED_VARS;
		const deUint32	unsized			= FEATURE_UNSIZED_ARRAYS;
		const deUint32	matFlags		= FEATURE_MATRIX_LAYOUT;

		tcu::TestCaseGroup* randomGroup = new tcu::TestCaseGroup(m_testCtx, "random", "Random Uniform Block cases");
		addChild(randomGroup);

		// Basic types.
		createRandomCaseGroup(randomGroup, m_testCtx, "scalar_types",		"Scalar types only, per-block buffers",				SSBOLayoutCase::BUFFERMODE_PER_BLOCK,	allLayouts|unused,																			25, 0);
		createRandomCaseGroup(randomGroup, m_testCtx, "vector_types",		"Scalar and vector types only, per-block buffers",	SSBOLayoutCase::BUFFERMODE_PER_BLOCK,	allLayouts|unused|FEATURE_VECTORS,															25, 25);
		createRandomCaseGroup(randomGroup, m_testCtx, "basic_types",		"All basic types, per-block buffers",				SSBOLayoutCase::BUFFERMODE_PER_BLOCK,	allLayouts|unused|allBasicTypes|matFlags,													25, 50);
		createRandomCaseGroup(randomGroup, m_testCtx, "basic_arrays",		"Arrays, per-block buffers",						SSBOLayoutCase::BUFFERMODE_PER_BLOCK,	allLayouts|unused|allBasicTypes|matFlags|FEATURE_ARRAYS,									25, 50);
		createRandomCaseGroup(randomGroup, m_testCtx, "unsized_arrays",		"Unsized arrays, per-block buffers",				SSBOLayoutCase::BUFFERMODE_PER_BLOCK,	allLayouts|unused|allBasicTypes|matFlags|unsized|FEATURE_ARRAYS,							25, 50);
		createRandomCaseGroup(randomGroup, m_testCtx, "arrays_of_arrays",	"Arrays of arrays, per-block buffers",				SSBOLayoutCase::BUFFERMODE_PER_BLOCK,	allLayouts|unused|allBasicTypes|matFlags|unsized|FEATURE_ARRAYS|FEATURE_ARRAYS_OF_ARRAYS,	25, 950);

		createRandomCaseGroup(randomGroup, m_testCtx, "basic_instance_arrays",					"Basic instance arrays, per-block buffers",				SSBOLayoutCase::BUFFERMODE_PER_BLOCK,	allLayouts|unused|allBasicTypes|matFlags|unsized|FEATURE_INSTANCE_ARRAYS,															25, 75);
		createRandomCaseGroup(randomGroup, m_testCtx, "nested_structs",							"Nested structs, per-block buffers",					SSBOLayoutCase::BUFFERMODE_PER_BLOCK,	allLayouts|unused|allBasicTypes|matFlags|unsized|FEATURE_STRUCTS,																	25, 100);
		createRandomCaseGroup(randomGroup, m_testCtx, "nested_structs_arrays",					"Nested structs, arrays, per-block buffers",			SSBOLayoutCase::BUFFERMODE_PER_BLOCK,	allLayouts|unused|allBasicTypes|matFlags|unsized|FEATURE_STRUCTS|FEATURE_ARRAYS|FEATURE_ARRAYS_OF_ARRAYS,							25, 150);
		createRandomCaseGroup(randomGroup, m_testCtx, "nested_structs_instance_arrays",			"Nested structs, instance arrays, per-block buffers",	SSBOLayoutCase::BUFFERMODE_PER_BLOCK,	allLayouts|unused|allBasicTypes|matFlags|unsized|FEATURE_STRUCTS|FEATURE_INSTANCE_ARRAYS,											25, 125);
		createRandomCaseGroup(randomGroup, m_testCtx, "nested_structs_arrays_instance_arrays",	"Nested structs, instance arrays, per-block buffers",	SSBOLayoutCase::BUFFERMODE_PER_BLOCK,	allLayouts|unused|allBasicTypes|matFlags|unsized|FEATURE_STRUCTS|FEATURE_ARRAYS|FEATURE_ARRAYS_OF_ARRAYS|FEATURE_INSTANCE_ARRAYS,	25, 175);

		createRandomCaseGroup(randomGroup, m_testCtx, "all_per_block_buffers",	"All random features, per-block buffers",	SSBOLayoutCase::BUFFERMODE_PER_BLOCK,	~0u,	50, 200);
		createRandomCaseGroup(randomGroup, m_testCtx, "all_shared_buffer",		"All random features, shared buffer",		SSBOLayoutCase::BUFFERMODE_SINGLE,		~0u,	50, 250);
	}
}

} // anonymous

tcu::TestCaseGroup* createTests (tcu::TestContext& testCtx)
{
	de::MovePtr<tcu::TestCaseGroup> ssboTestGroup (new tcu::TestCaseGroup(testCtx, "ssbo", "Shader Storage Buffer Object Tests"));

	ssboTestGroup->addChild(new SSBOLayoutTests(testCtx));

	return ssboTestGroup.release();
}

} // ssbo
} // vkt
