#ifndef _VKBINARYREGISTRY_HPP
#define _VKBINARYREGISTRY_HPP
/*-------------------------------------------------------------------------
 * Vulkan CTS Framework
 * --------------------
 *
 * Copyright (c) 2015 Google Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice(s) and this permission notice shall be
 * included in all copies or substantial portions of the Materials.
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
 * \brief Program binary registry.
 *//*--------------------------------------------------------------------*/

#include "vkDefs.hpp"
#include "vkPrograms.hpp"
#include "tcuResource.hpp"
#include "deMemPool.hpp"
#include "dePoolHash.h"
#include "deUniquePtr.hpp"

#include <map>
#include <vector>
#include <stdexcept>

namespace vk
{
namespace BinaryRegistryDetail
{

struct ProgramIdentifier
{
	std::string		testCasePath;
	std::string		programName;

	ProgramIdentifier (const std::string& testCasePath_, const std::string& programName_)
		: testCasePath	(testCasePath_)
		, programName	(programName_)
	{
	}
};

inline bool operator< (const ProgramIdentifier& a, const ProgramIdentifier& b)
{
	return (a.testCasePath < b.testCasePath) || ((a.testCasePath == b.testCasePath) && (a.programName < b.programName));
}

class ProgramNotFoundException : public tcu::ResourceError
{
public:
	ProgramNotFoundException (const ProgramIdentifier& id, const std::string& reason)
		: tcu::ResourceError("Program " + id.testCasePath + " / '" + id.programName + "' not found: " + reason)
	{
	}
};

// Program Binary Index
// --------------------
//
// When SPIR-V binaries are stored on disk, duplicate binaries are eliminated
// to save a significant amount of space. Many tests use identical binaries and
// just storing each compiled binary without de-duplication would be incredibly
// wasteful.
//
// To locate binary that corresponds given ProgramIdentifier, a program binary
// index is needed. Since that index is accessed every time a test requests shader
// binary, it must be fast to load (to reduce statup cost), and fast to access.
//
// Simple trie is used to store binary indices. It is laid out as an array of
// BinaryIndexNodes. Nodes store 4-byte pieces (words) of search string, rather
// than just a single character. This gives more regular memory layout in exchange
// of a little wasted storage.
//
// Search strings are created by splitting original string into 4-byte words and
// appending one or more terminating 0 bytes.
//
// For each node where word doesn't have trailing 0 bytes (not terminated), the
// index points into a offset of its child list. Children for each node are stored
// consecutively, and the list is terminated by child with word = 0.
//
// If word contains one or more trailing 0 bytes, index denotes the binary index
// instead of index of the child list.

struct BinaryIndexNode
{
	deUint32	word;		//!< 4 bytes of search string.
	deUint32	index;		//!< Binary index if word ends with 0 bytes, or index of first child node otherwise.
};

template<typename Element>
class LazyResource
{
public:
									LazyResource		(de::MovePtr<tcu::Resource> resource);

	const Element&					operator[]			(size_t ndx);
	size_t							size				(void) const { return m_elements.size();	}

private:
	enum
	{
		ELEMENTS_PER_PAGE_LOG2	= 10
	};

	inline size_t					getPageForElement	(size_t elemNdx) const { return elemNdx >> ELEMENTS_PER_PAGE_LOG2;	}
	inline bool						isPageResident		(size_t pageNdx) const { return m_isPageResident[pageNdx];			}

	void							makePageResident	(size_t pageNdx);

	de::UniquePtr<tcu::Resource>	m_resource;

	std::vector<Element>			m_elements;
	std::vector<bool>				m_isPageResident;
};

template<typename Element>
LazyResource<Element>::LazyResource (de::MovePtr<tcu::Resource> resource)
	: m_resource(resource)
{
	const size_t	resSize		= m_resource->getSize();
	const size_t	numElements	= resSize/sizeof(Element);
	const size_t	numPages	= (numElements >> ELEMENTS_PER_PAGE_LOG2) + ((numElements & ((1u<<ELEMENTS_PER_PAGE_LOG2)-1u)) == 0 ? 0 : 1);

	TCU_CHECK_INTERNAL(numElements*sizeof(Element) == resSize);

	m_elements.resize(numElements);
	m_isPageResident.resize(numPages, false);
}

template<typename Element>
const Element& LazyResource<Element>::operator[] (size_t ndx)
{
	const size_t pageNdx = getPageForElement(ndx);

	if (ndx >= m_elements.size())
		throw std::out_of_range("");

	if (!isPageResident(pageNdx))
		makePageResident(pageNdx);

	return m_elements[ndx];
}

template<typename Element>
void LazyResource<Element>::makePageResident (size_t pageNdx)
{
	const size_t	pageSize		= (size_t)(1<<ELEMENTS_PER_PAGE_LOG2)*sizeof(Element);
	const size_t	pageOffset		= pageNdx*pageSize;
	const size_t	numBytesToRead	= de::min(m_elements.size()*sizeof(Element) - pageOffset, pageSize);

	DE_ASSERT(!isPageResident(pageNdx));

	if ((size_t)m_resource->getPosition() != pageOffset)
		m_resource->setPosition((int)pageOffset);

	m_resource->read((deUint8*)&m_elements[pageNdx << ELEMENTS_PER_PAGE_LOG2], (int)numBytesToRead);
	m_isPageResident[pageNdx] = true;
}

typedef LazyResource<BinaryIndexNode> BinaryIndexAccess;

DE_DECLARE_POOL_HASH(BinaryHash, const ProgramBinary*, deUint32);

class BinaryRegistryReader
{
public:
							BinaryRegistryReader	(const tcu::Archive& archive, const std::string& srcPath);
							~BinaryRegistryReader	(void);

	ProgramBinary*			loadProgram				(const ProgramIdentifier& id) const;

private:
	typedef de::MovePtr<BinaryIndexAccess> BinaryIndexPtr;

	const tcu::Archive&		m_archive;
	const std::string		m_srcPath;

	mutable BinaryIndexPtr	m_binaryIndex;
};

class BinaryRegistryWriter
{
public:
						BinaryRegistryWriter	(const std::string& dstPath);
						~BinaryRegistryWriter	(void);

	void				storeProgram			(const ProgramIdentifier& id, const ProgramBinary& binary);
	void				writeIndex				(void) const;

private:
	struct BinaryIndex
	{
		ProgramIdentifier	id;
		deUint32			index;

		BinaryIndex (const ProgramIdentifier&	id_,
					 deUint32					index_)
			: id	(id_)
			, index	(index_)
		{}
	};

	typedef std::vector<ProgramBinary*>	BinaryVector;
	typedef std::vector<BinaryIndex>	BinaryIndexVector;

	const std::string&	m_dstPath;

	de::MemPool			m_memPool;
	BinaryHash*			m_binaryIndexMap;		//!< ProgramBinary -> slot in m_compactedBinaries
	BinaryVector		m_compactedBinaries;
	BinaryIndexVector	m_binaryIndices;		//!< ProgramIdentifier -> slot in m_compactedBinaries
};

} // BinaryRegistryDetail

using BinaryRegistryDetail::BinaryRegistryReader;
using BinaryRegistryDetail::BinaryRegistryWriter;
using BinaryRegistryDetail::ProgramIdentifier;
using BinaryRegistryDetail::ProgramNotFoundException;

} // vk

#endif // _VKBINARYREGISTRY_HPP
