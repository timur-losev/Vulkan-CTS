/*-------------------------------------------------------------------------
 * Vulkan Conformance Tests
 * ------------------------
 *
 * Copyright (c) 2015 The Khronos Group Inc.
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
 *//*--------------------------------------------------------------------*/

#include "vkDefs.hpp"
#include "vktTestCaseUtil.hpp"
#include "vkBuilderUtil.hpp"
#include "vkPlatform.hpp"
#include "vkRefUtil.hpp"
#include "vkQueryUtil.hpp"
#include "vkMemUtil.hpp"
#include "vkDeviceUtil.hpp"
#include "vkPrograms.hpp"
#include "vkTypeUtil.hpp"
#include "vkAllocationCallbackUtil.hpp"
#include "vktApiCommandBuffersTests.hpp"
#include "vktApiBufferComputeInstance.hpp"
#include "vktApiComputeInstanceResultBuffer.hpp"
#include "deSharedPtr.hpp"
#include <sstream>

namespace vkt
{
namespace api
{
namespace
{

using namespace vk;

typedef de::SharedPtr<vk::Unique<vk::VkEvent> >	VkEventSp;

// Global variables
const deUint64								INFINITE_TIMEOUT		= ~(deUint64)0u;

// Testcases
/********* 19.1. Command Pools (6.1 in VK 1.0 Spec) ***************************/
tcu::TestStatus createPoolNullParamsTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext;
		0u,															// flags;
		queueFamilyIndex,											// queueFamilyIndex;
	};

	createCommandPool(vk, vkDevice, &cmdPoolParams, DE_NULL);

	return tcu::TestStatus::pass("Command Pool allocated correctly.");
}

tcu::TestStatus createPoolNonNullAllocatorTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();
	const VkAllocationCallbacks*			allocationCallbacks		= getSystemAllocator();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext;
		0u,															// flags;
		queueFamilyIndex,											// queueFamilyIndex;
	};

	createCommandPool(vk, vkDevice, &cmdPoolParams, allocationCallbacks);

	return tcu::TestStatus::pass("Command Pool allocated correctly.");
}

tcu::TestStatus createPoolTransientBitTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext;
		VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,						// flags;
		queueFamilyIndex,											// queueFamilyIndex;
	};

	createCommandPool(vk, vkDevice, &cmdPoolParams, DE_NULL);

	return tcu::TestStatus::pass("Command Pool allocated correctly.");
}

tcu::TestStatus createPoolResetBitTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			// flags;
		queueFamilyIndex,											// queueFamilyIndex;
	};

	createCommandPool(vk, vkDevice, &cmdPoolParams, DE_NULL);

	return tcu::TestStatus::pass("Command Pool allocated correctly.");
}

tcu::TestStatus resetPoolReleaseResourcesBitTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext;
		0u,															// flags;
		queueFamilyIndex,											// queueFamilyIndex;
	};

	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams, DE_NULL));

	VK_CHECK(vk.resetCommandPool(vkDevice, *cmdPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT));

	return tcu::TestStatus::pass("Command Pool allocated correctly.");
}

tcu::TestStatus resetPoolNoFlagsTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext;
		0u,															// flags;
		queueFamilyIndex,											// queueFamilyIndex;
	};

	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams, DE_NULL));

	VK_CHECK(vk.resetCommandPool(vkDevice, *cmdPool, 0u));

	return tcu::TestStatus::pass("Command Pool allocated correctly.");
}

/******** 19.2. Command Buffer Lifetime (6.2 in VK 1.0 Spec) ******************/
tcu::TestStatus allocatePrimaryBufferTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			// flags;
		queueFamilyIndex,											// queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				// sType;
		DE_NULL,													// pNext;
		*cmdPool,													// commandPool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							// level;
		1u,															// bufferCount;
	};
	const Unique<VkCommandBuffer>			cmdBuf					(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));

	return tcu::TestStatus::pass("Buffer was created correctly.");
}

tcu::TestStatus allocateManyPrimaryBuffersTest(Context& context)
{

	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			//	VkCommandPoolCreateFlags	flags;
		queueFamilyIndex,											//	deUint32					queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// \todo Determining the minimum number of command buffers should be a function of available system memory and driver capabilities.
#if (DE_PTR_SIZE == 4)
	const unsigned minCommandBuffer = 1024;
#else
	const unsigned minCommandBuffer = 10000;
#endif

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							//	VkCommandBufferLevel		level;
		minCommandBuffer,											//	uint32_t					bufferCount;
	};

	// do not keep the handles to buffers, as they will be freed with command pool

	// allocate the minimum required amount of buffers
	VkCommandBuffer cmdBuffers[minCommandBuffer];
	VK_CHECK(vk.allocateCommandBuffers(vkDevice, &cmdBufParams, cmdBuffers));

	std::ostringstream out;
	out << "allocateManyPrimaryBuffersTest succeded: created " << minCommandBuffer << " command buffers";

	return tcu::TestStatus::pass(out.str());
}

tcu::TestStatus allocateZeroPrimaryBuffersTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			// flags;
		queueFamilyIndex,											// queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				// sType;
		DE_NULL,													// pNext;
		*cmdPool,													// commandPool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							// level;
		0u,															// bufferCount;
	};

	VkCommandBuffer cmdBuffer;
	VK_CHECK(vk.allocateCommandBuffers(vkDevice, &cmdBufParams, &cmdBuffer));

	return tcu::TestStatus::pass("allocateZeroPrimaryBuffersTest passed.");
}

tcu::TestStatus allocateSecondaryBufferTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			// flags;
		queueFamilyIndex,											// queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				// sType;
		DE_NULL,													// pNext;
		*cmdPool,													// commandPool;
		VK_COMMAND_BUFFER_LEVEL_SECONDARY,							// level;
		1u,															// bufferCount;
	};
	const Unique<VkCommandBuffer>			cmdBuf					(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));

	return tcu::TestStatus::pass("Buffer was created correctly.");
}

tcu::TestStatus allocateManySecondaryBuffersTest(Context& context)
{

	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			//	VkCommandPoolCreateFlags	flags;
		queueFamilyIndex,											//	deUint32					queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// \todo Determining the minimum number of command buffers should be a function of available system memory and driver capabilities.
#if (DE_PTR_SIZE == 4)
	const unsigned minCommandBuffer = 1024;
#else
	const unsigned minCommandBuffer = 10000;
#endif

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_SECONDARY,							//	VkCommandBufferLevel		level;
		minCommandBuffer,											//	uint32_t					bufferCount;
	};

	// do not keep the handles to buffers, as they will be freed with command pool

	// allocate the minimum required amount of buffers
	VkCommandBuffer cmdBuffers[minCommandBuffer];
	VK_CHECK(vk.allocateCommandBuffers(vkDevice, &cmdBufParams, cmdBuffers));

	std::ostringstream out;
	out << "allocateManySecondaryBuffersTest succeded: created " << minCommandBuffer << " command buffers";

	return tcu::TestStatus::pass(out.str());
}

tcu::TestStatus allocateZeroSecondaryBuffersTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			// flags;
		queueFamilyIndex,											// queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				// sType;
		DE_NULL,													// pNext;
		*cmdPool,													// commandPool;
		VK_COMMAND_BUFFER_LEVEL_SECONDARY,							// level;
		0u,															// bufferCount;
	};

	VkCommandBuffer cmdBuffer;
	VK_CHECK(vk.allocateCommandBuffers(vkDevice, &cmdBufParams, &cmdBuffer));

	return tcu::TestStatus::pass("allocateZeroSecondaryBuffersTest passed.");
}

tcu::TestStatus executePrimaryBufferTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const VkQueue							queue					= context.getUniversalQueue();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			//	VkCommandPoolCreateFlags	flags;
		queueFamilyIndex,											//	deUint32					queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							//	VkCommandBufferLevel		level;
		1u,															//	uint32_t					bufferCount;
	};
	const Unique<VkCommandBuffer>			primCmdBuf				(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));
	const VkCommandBufferBeginInfo			primCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		DE_NULL,
		0,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	// Fill create info struct for event
	const VkEventCreateInfo					eventCreateInfo			=
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		DE_NULL,
		0u,
	};

	// create event that will be used to check if secondary command buffer has been executed
	const Unique<VkEvent>					event					(createEvent(vk, vkDevice, &eventCreateInfo));

	// reset event
	VK_CHECK(vk.resetEvent(vkDevice, *event));

	// record primary command buffer
	VK_CHECK(vk.beginCommandBuffer(*primCmdBuf, &primCmdBufBeginInfo));
	{
		// allow execution of event during every stage of pipeline
		VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		// record setting event
		vk.cmdSetEvent(*primCmdBuf, *event,stageMask);
	}
	VK_CHECK(vk.endCommandBuffer(*primCmdBuf));

	const VkFenceCreateInfo					fenceCreateInfo			=
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		DE_NULL,
		0u,															// flags
	};

	// create fence to wait for execution of queue
	const Unique<VkFence>					fence					(createFence(vk, vkDevice, &fenceCreateInfo));

	const VkSubmitInfo						submitInfo				=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1,															// commandBufferCount
		&primCmdBuf.get(),											// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	// Submit the command buffer to the queue
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo, *fence));

	// wait for end of execution of queue
	VK_CHECK(vk.waitForFences(vkDevice, 1, &fence.get(), 0u, INFINITE_TIMEOUT));

	// check if buffer has been executed
	VkResult result = vk.getEventStatus(vkDevice,*event);
	if (result == VK_EVENT_SET)
		return tcu::TestStatus::pass("Execute Primary Command Buffer succeeded");

	return tcu::TestStatus::fail("Execute Primary Command Buffer FAILED");
}

tcu::TestStatus executeLargePrimaryBufferTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const VkQueue							queue					= context.getUniversalQueue();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();
	const deUint32							LARGE_BUFFER_SIZE		= 10000;

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			//	VkCommandPoolCreateFlags	flags;
		queueFamilyIndex,											//	deUint32					queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							//	VkCommandBufferLevel		level;
		1u,															//	uint32_t					bufferCount;
	};
	const Unique<VkCommandBuffer>			primCmdBuf				(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));
	const VkCommandBufferBeginInfo			primCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		DE_NULL,
		0,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	// Fill create info struct for event
	const VkEventCreateInfo					eventCreateInfo			=
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		DE_NULL,
		0u,
	};

	std::vector<VkEventSp>					events;
	for (deUint32 ndx = 0; ndx < LARGE_BUFFER_SIZE; ++ndx)
		events.push_back(VkEventSp(new vk::Unique<VkEvent>(createEvent(vk, vkDevice, &eventCreateInfo, DE_NULL))));

	// record primary command buffer
	VK_CHECK(vk.beginCommandBuffer(*primCmdBuf, &primCmdBufBeginInfo));
	{
		// set all the events
		for (deUint32 ndx = 0; ndx < LARGE_BUFFER_SIZE; ++ndx)
		{
			vk.cmdSetEvent(*primCmdBuf, events[ndx]->get(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
		}
	}
	VK_CHECK(vk.endCommandBuffer(*primCmdBuf));

	const VkFenceCreateInfo					fenceCreateInfo			=
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		DE_NULL,
		0u,															// flags
	};

	// create fence to wait for execution of queue
	const Unique<VkFence>					fence					(createFence(vk, vkDevice, &fenceCreateInfo));

	const VkSubmitInfo						submitInfo				=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1,															// commandBufferCount
		&primCmdBuf.get(),											// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	// Submit the command buffer to the queue
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo, *fence));

	// wait for end of execution of queue
	VK_CHECK(vk.waitForFences(vkDevice, 1, &fence.get(), 0u, INFINITE_TIMEOUT));

	// check if the buffer was executed correctly - all events had their status
	// changed
	tcu::TestStatus testResult = tcu::TestStatus::incomplete();

	for (deUint32 ndx = 0; ndx < LARGE_BUFFER_SIZE; ++ndx)
	{
		if (vk.getEventStatus(vkDevice, events[ndx]->get()) != VK_EVENT_SET)
		{
			testResult = tcu::TestStatus::fail("An event was not set.");
			break;
		}
	}

	if (!testResult.isComplete())
		testResult = tcu::TestStatus::pass("All events set correctly.");

	return testResult;
}

tcu::TestStatus resetBufferImplicitlyTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const VkQueue							queue					= context.getUniversalQueue();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			// flags;
		queueFamilyIndex,											// queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				// sType;
		DE_NULL,													// pNext;
		*cmdPool,													// pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							// level;
		1u,															// bufferCount;
	};
	const Unique<VkCommandBuffer>			cmdBuf						(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));

	const VkCommandBufferBeginInfo			cmdBufBeginInfo			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,				// sType
		DE_NULL,													// pNext
		0u,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	const VkEventCreateInfo					eventCreateInfo			=
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,						// sType;
		DE_NULL,													// pNext;
		0u,															// flags;
	};
	const Unique<VkEvent>					event					(createEvent(vk, vkDevice, &eventCreateInfo));

	// Put the command buffer in recording state.
	VK_CHECK(vk.beginCommandBuffer(*cmdBuf, &cmdBufBeginInfo));
	{
		// Set the event
		vk.cmdSetEvent(*cmdBuf, *event, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
	}
	VK_CHECK(vk.endCommandBuffer(*cmdBuf));

	// We'll use a fence to wait for the execution of the queue
	const VkFenceCreateInfo					fenceCreateInfo			=
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,						// sType;
		DE_NULL,													// pNext;
		0u,															// flags
	};
	const Unique<VkFence>					fence					(createFence(vk, vkDevice, &fenceCreateInfo));

	const VkSubmitInfo						submitInfo				=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1u,															// commandBufferCount
		&cmdBuf.get(),												// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	// Submitting the command buffer that sets the event to the queue
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo, fence.get()));

	// Waiting for the queue to finish executing
	VK_CHECK(vk.waitForFences(vkDevice, 1u, &fence.get(), 0u, INFINITE_TIMEOUT));
	// Reset the fence so that we can reuse it
	VK_CHECK(vk.resetFences(vkDevice, 1u, &fence.get()));

	// Check if the buffer was executed
	if (vk.getEventStatus(vkDevice, *event) != VK_EVENT_SET)
		return tcu::TestStatus::fail("Failed to set the event.");

	// Reset the event
	vk.resetEvent(vkDevice, *event);
	if(vk.getEventStatus(vkDevice, *event) != VK_EVENT_RESET)
		return tcu::TestStatus::fail("Failed to reset the event.");

	// Reset the command buffer by putting it in recording state again. This
	// should empty the command buffer.
	VK_CHECK(vk.beginCommandBuffer(*cmdBuf, &cmdBufBeginInfo));
	VK_CHECK(vk.endCommandBuffer(*cmdBuf));

	// Submit the command buffer after resetting. It should have no commands
	// recorded, so the event should remain unsignaled.
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo, fence.get()));
	// Waiting for the queue to finish executing
	VK_CHECK(vk.waitForFences(vkDevice, 1u, &fence.get(), 0u, INFINITE_TIMEOUT));

	// Check if the event remained unset.
	if(vk.getEventStatus(vkDevice, *event) == VK_EVENT_RESET)
		return tcu::TestStatus::pass("Buffer was reset correctly.");
	else
		return tcu::TestStatus::fail("Buffer was not reset correctly.");
}

/******** 19.3. Command Buffer Recording (6.3 in VK 1.0 Spec) *****************/
tcu::TestStatus recordSinglePrimaryBufferTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			//	VkCommandPoolCreateFlags	flags;
		queueFamilyIndex,											//	deUint32					queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							//	VkCommandBufferLevel		level;
		1u,															//	uint32_t					bufferCount;
	};
	const Unique<VkCommandBuffer>			primCmdBuf				(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));

	const VkCommandBufferBeginInfo			primCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		DE_NULL,
		0,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	// Fill create info struct for event
	const VkEventCreateInfo					eventCreateInfo			=
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		DE_NULL,
		0u,
	};

	// create event that will be used to check if secondary command buffer has been executed
	const Unique<VkEvent>					event					(createEvent(vk, vkDevice, &eventCreateInfo));

	// record primary command buffer
	VK_CHECK(vk.beginCommandBuffer(*primCmdBuf, &primCmdBufBeginInfo));
	{
		// record setting event
		vk.cmdSetEvent(*primCmdBuf, *event, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
	}
	VK_CHECK(vk.endCommandBuffer(*primCmdBuf));

	return tcu::TestStatus::pass("Primary buffer recorded successfully.");
}

tcu::TestStatus recordLargePrimaryBufferTest(Context &context)
{

	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const VkQueue							queue					= context.getUniversalQueue();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			//	VkCommandPoolCreateFlags	flags;
		queueFamilyIndex,											//	deUint32					queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							//	VkCommandBufferLevel		level;
		1u,															//	uint32_t					bufferCount;
	};
	const Unique<VkCommandBuffer>			primCmdBuf				(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));
	const VkCommandBufferBeginInfo			primCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		DE_NULL,
		0,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	// Fill create info struct for event
	const VkEventCreateInfo					eventCreateInfo			=
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		DE_NULL,
		0u,
	};

	// create event that will be used to check if secondary command buffer has been executed
	const Unique<VkEvent>					event					(createEvent(vk, vkDevice, &eventCreateInfo));

	// reset event
	VK_CHECK(vk.resetEvent(vkDevice, *event));

	// record primary command buffer
	VK_CHECK(vk.beginCommandBuffer(*primCmdBuf, &primCmdBufBeginInfo));
	{
		// allow execution of event during every stage of pipeline
		VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		// define minimal amount of commands to accept
		const long long unsigned minNumCommands = 10000llu;

		for ( long long unsigned currentCommands = 0; currentCommands < minNumCommands / 2; ++currentCommands )
		{
			// record setting event
			vk.cmdSetEvent(*primCmdBuf, *event,stageMask);

			// record resetting event
			vk.cmdResetEvent(*primCmdBuf, *event,stageMask);
		};

	}
	VK_CHECK(vk.endCommandBuffer(*primCmdBuf));

	const VkFenceCreateInfo					fenceCreateInfo			=
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		DE_NULL,
		0u,															// flags
	};

	// create fence to wait for execution of queue
	const Unique<VkFence>					fence					(createFence(vk, vkDevice, &fenceCreateInfo));

	const VkSubmitInfo						submitInfo				=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1,															// commandBufferCount
		&primCmdBuf.get(),											// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	// Submit the command buffer to the queue
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo, *fence));

	// wait for end of execution of queue
	VK_CHECK(vk.waitForFences(vkDevice, 1, &fence.get(), 0u, INFINITE_TIMEOUT));

	return tcu::TestStatus::pass("hugeTest succeeded");
}

tcu::TestStatus recordSingleSecondaryBufferTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			//	VkCommandPoolCreateFlags	flags;
		queueFamilyIndex,											//	deUint32					queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_SECONDARY,							//	VkCommandBufferLevel		level;
		1u,															//	uint32_t					bufferCount;
	};
	const Unique<VkCommandBuffer>			secCmdBuf				(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));

	const VkCommandBufferInheritanceInfo	secCmdBufInheritInfo	=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
		DE_NULL,
		(VkRenderPass)0u,											// renderPass
		0u,															// subpass
		(VkFramebuffer)0u,											// framebuffer
		VK_FALSE,													// occlusionQueryEnable
		(VkQueryControlFlags)0u,									// queryFlags
		(VkQueryPipelineStatisticFlags)0u,							// pipelineStatistics
	};
	const VkCommandBufferBeginInfo			secCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		DE_NULL,
		0,															// flags
		&secCmdBufInheritInfo,
	};

	// Fill create info struct for event
	const VkEventCreateInfo					eventCreateInfo			=
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		DE_NULL,
		0u,
	};

	// create event that will be used to check if secondary command buffer has been executed
	const Unique<VkEvent>					event					(createEvent(vk, vkDevice, &eventCreateInfo));

	// record primary command buffer
	VK_CHECK(vk.beginCommandBuffer(*secCmdBuf, &secCmdBufBeginInfo));
	{
		// record setting event
		vk.cmdSetEvent(*secCmdBuf, *event, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
	}
	VK_CHECK(vk.endCommandBuffer(*secCmdBuf));

	return tcu::TestStatus::pass("Secondary buffer recorded successfully.");
}

tcu::TestStatus recordLargeSecondaryBufferTest(Context &context)
{

	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const VkQueue							queue					= context.getUniversalQueue();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			//	VkCommandPoolCreateFlags	flags;
		queueFamilyIndex,											//	deUint32					queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							//	VkCommandBufferLevel		level;
		1u,															//	uint32_t					bufferCount;
	};
	const Unique<VkCommandBuffer>			primCmdBuf				(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));
	const VkCommandBufferBeginInfo			primCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		DE_NULL,
		0,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	// Fill create info struct for event
	const VkEventCreateInfo					eventCreateInfo			=
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		DE_NULL,
		0u,
	};

	// create event that will be used to check if secondary command buffer has been executed
	const Unique<VkEvent>					event					(createEvent(vk, vkDevice, &eventCreateInfo));

	// reset event
	VK_CHECK(vk.resetEvent(vkDevice, *event));

	// record primary command buffer
	VK_CHECK(vk.beginCommandBuffer(*primCmdBuf, &primCmdBufBeginInfo));
	{
		// allow execution of event during every stage of pipeline
		VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		// define minimal amount of commands to accept
		const long long unsigned minNumCommands = 10000llu;

		for ( long long unsigned currentCommands = 0; currentCommands < minNumCommands / 2; ++currentCommands )
		{
			// record setting event
			vk.cmdSetEvent(*primCmdBuf, *event,stageMask);

			// record resetting event
			vk.cmdResetEvent(*primCmdBuf, *event,stageMask);
		};


	}
	VK_CHECK(vk.endCommandBuffer(*primCmdBuf));

	const VkFenceCreateInfo					fenceCreateInfo			=
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		DE_NULL,
		0u,															// flags
	};

	// create fence to wait for execution of queue
	const Unique<VkFence>					fence					(createFence(vk, vkDevice, &fenceCreateInfo));

	const VkSubmitInfo						submitInfo				=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1,															// commandBufferCount
		&primCmdBuf.get(),											// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	// Submit the command buffer to the queue
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo, *fence));

	// wait for end of execution of queue
	VK_CHECK(vk.waitForFences(vkDevice, 1, &fence.get(), 0u, INFINITE_TIMEOUT));

	return tcu::TestStatus::pass("hugeTest succeeded");
}

tcu::TestStatus submitPrimaryBufferTwiceTest(Context& context)
{

	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const VkQueue							queue					= context.getUniversalQueue();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			//	VkCommandPoolCreateFlags	flags;
		queueFamilyIndex,											//	deUint32					queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType			sType;
		DE_NULL,													//	const void*				pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							//	VkCommandBufferLevel		level;
		1u,															//	uint32_t					bufferCount;
	};
	const Unique<VkCommandBuffer>			primCmdBuf				(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));
	const VkCommandBufferBeginInfo			primCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		DE_NULL,
		0,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	// Fill create info struct for event
	const VkEventCreateInfo					eventCreateInfo			=
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		DE_NULL,
		0u,
	};

	// create event that will be used to check if secondary command buffer has been executed
	const Unique<VkEvent>					event					(createEvent(vk, vkDevice, &eventCreateInfo));

	// reset event
	VK_CHECK(vk.resetEvent(vkDevice, *event));

	// record primary command buffer
	VK_CHECK(vk.beginCommandBuffer(*primCmdBuf, &primCmdBufBeginInfo));
	{
		// allow execution of event during every stage of pipeline
		VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		// record setting event
		vk.cmdSetEvent(*primCmdBuf, *event,stageMask);
	}
	VK_CHECK(vk.endCommandBuffer(*primCmdBuf));

	const VkFenceCreateInfo					fenceCreateInfo			=
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		DE_NULL,
		0u,															// flags
	};

	// create fence to wait for execution of queue
	const Unique<VkFence>					fence					(createFence(vk, vkDevice, &fenceCreateInfo));

	const VkSubmitInfo						submitInfo				=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1,															// commandBufferCount
		&primCmdBuf.get(),											// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	// submit primary buffer
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo, *fence));

	// wait for end of execution of queue
	VK_CHECK(vk.waitForFences(vkDevice, 1, &fence.get(), 0u, INFINITE_TIMEOUT));
	VK_CHECK(vk.resetFences(vkDevice, 1u, &fence.get()));
	// check if buffer has been executed
	VkResult result = vk.getEventStatus(vkDevice,*event);
	if (result != VK_EVENT_SET)
		return tcu::TestStatus::fail("Submit Twice Test FAILED");

	// reset event
	VK_CHECK(vk.resetEvent(vkDevice, *event));

	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo, *fence));

	// wait for end of execution of queue
	VK_CHECK(vk.waitForFences(vkDevice, 1, &fence.get(), 0u, INFINITE_TIMEOUT));

	// check if buffer has been executed
	result = vk.getEventStatus(vkDevice,*event);
	if (result != VK_EVENT_SET)
		return tcu::TestStatus::fail("Submit Twice Test FAILED");
	else
		return tcu::TestStatus::pass("Submit Twice Test succeeded");
}

tcu::TestStatus submitSecondaryBufferTwiceTest(Context& context)
{

	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const VkQueue							queue					= context.getUniversalQueue();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			//	VkCommandPoolCreateFlags	flags;
		queueFamilyIndex,											//	deUint32					queueFamilyIndex;
	};

	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType			sType;
		DE_NULL,													//	const void*				pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							//	VkCommandBufferLevel		level;
		1u,															//	uint32_t					bufferCount;
	};

	const Unique<VkCommandBuffer>			primCmdBuf1				(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));
	const Unique<VkCommandBuffer>			primCmdBuf2				(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));

	// Secondary Command buffer
	const VkCommandBufferAllocateInfo		secCmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType			sType;
		DE_NULL,													//	const void*				pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_SECONDARY,							//	VkCommandBufferLevel		level;
		1u,															//	uint32_t					bufferCount;
	};
	const Unique<VkCommandBuffer>			secCmdBuf				(allocateCommandBuffer(vk, vkDevice, &secCmdBufParams));

	const VkCommandBufferBeginInfo			primCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		DE_NULL,
		0,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	const VkCommandBufferInheritanceInfo	secCmdBufInheritInfo	=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
		DE_NULL,
		(VkRenderPass)0u,											// renderPass
		0u,															// subpass
		(VkFramebuffer)0u,											// framebuffer
		VK_FALSE,													// occlusionQueryEnable
		(VkQueryControlFlags)0u,									// queryFlags
		(VkQueryPipelineStatisticFlags)0u,							// pipelineStatistics
	};
	const VkCommandBufferBeginInfo			secCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		DE_NULL,
		0u,															// flags
		&secCmdBufInheritInfo,
	};

	// Fill create info struct for event
	const VkEventCreateInfo					eventCreateInfo			=
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		DE_NULL,
		0u,
	};

	// create event that will be used to check if secondary command buffer has been executed
	const Unique<VkEvent>					event					(createEvent(vk, vkDevice, &eventCreateInfo));

	// reset event
	VK_CHECK(vk.resetEvent(vkDevice, *event));

	// record first primary command buffer
	VK_CHECK(vk.beginCommandBuffer(*primCmdBuf1, &primCmdBufBeginInfo));
	{
		// record secondary command buffer
		VK_CHECK(vk.beginCommandBuffer(*secCmdBuf, &secCmdBufBeginInfo));
		{
			// allow execution of event during every stage of pipeline
			VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

			// record setting event
			vk.cmdSetEvent(*secCmdBuf, *event,stageMask);
		}

		// end recording of secondary buffers
		VK_CHECK(vk.endCommandBuffer(*secCmdBuf));

		// execute secondary buffer
		vk.cmdExecuteCommands(*primCmdBuf1, 1, &secCmdBuf.get());
	}
	VK_CHECK(vk.endCommandBuffer(*primCmdBuf1));

	const VkFenceCreateInfo					fenceCreateInfo			=
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		DE_NULL,
		0u,															// flags
	};

	// create fence to wait for execution of queue
	const Unique<VkFence>					fence					(createFence(vk, vkDevice, &fenceCreateInfo));

	const VkSubmitInfo						submitInfo1				=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1,															// commandBufferCount
		&primCmdBuf1.get(),											// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo1, *fence));

	// wait for end of execution of queue
	VK_CHECK(vk.waitForFences(vkDevice, 1, &fence.get(), 0u, INFINITE_TIMEOUT));
	VK_CHECK(vk.resetFences(vkDevice, 1u, &fence.get()));

	// check if secondary buffer has been executed
	VkResult result = vk.getEventStatus(vkDevice,*event);
	if (result != VK_EVENT_SET)
		return tcu::TestStatus::fail("Submit Twice Secondary Command Buffer FAILED");

	// reset first primary buffer
	vk.resetCommandBuffer( *primCmdBuf1, 0u);

	// reset event to allow receiving it again
	VK_CHECK(vk.resetEvent(vkDevice, *event));

	// record second primary command buffer
	VK_CHECK(vk.beginCommandBuffer(*primCmdBuf2, &primCmdBufBeginInfo));
	{
		// execute secondary buffer
		vk.cmdExecuteCommands(*primCmdBuf2, 1, &secCmdBuf.get());
	}
	// end recording
	VK_CHECK(vk.endCommandBuffer(*primCmdBuf2));

	// submit second primary buffer, the secondary should be executed too
	const VkSubmitInfo						submitInfo2				=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1,															// commandBufferCount
		&primCmdBuf2.get(),											// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo2, *fence));

	// wait for end of execution of queue
	VK_CHECK(vk.waitForFences(vkDevice, 1, &fence.get(), 0u, INFINITE_TIMEOUT));

	// check if secondary buffer has been executed
	result = vk.getEventStatus(vkDevice,*event);
	if (result != VK_EVENT_SET)
		return tcu::TestStatus::fail("Submit Twice Secondary Command Buffer FAILED");
	else
		return tcu::TestStatus::pass("Submit Twice Secondary Command Buffer succeeded");
}

tcu::TestStatus oneTimeSubmitFlagPrimaryBufferTest(Context& context)
{

	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const VkQueue							queue					= context.getUniversalQueue();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			//	VkCommandPoolCreateFlags	flags;
		queueFamilyIndex,											//	deUint32					queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							//	VkCommandBufferLevel		level;
		1u,															//	uint32_t					bufferCount;
	};
	const Unique<VkCommandBuffer>			primCmdBuf				(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));
	const VkCommandBufferBeginInfo			primCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		DE_NULL,
		VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,				// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	// Fill create info struct for event
	const VkEventCreateInfo					eventCreateInfo			=
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		DE_NULL,
		0u,
	};

	// create event that will be used to check if secondary command buffer has been executed
	const Unique<VkEvent>					event					(createEvent(vk, vkDevice, &eventCreateInfo));

	// reset event
	VK_CHECK(vk.resetEvent(vkDevice, *event));

	// record primary command buffer
	VK_CHECK(vk.beginCommandBuffer(*primCmdBuf, &primCmdBufBeginInfo));
	{
		// allow execution of event during every stage of pipeline
		VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		// record setting event
		vk.cmdSetEvent(*primCmdBuf, *event,stageMask);
	}
	VK_CHECK(vk.endCommandBuffer(*primCmdBuf));

	const VkFenceCreateInfo					fenceCreateInfo			=
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		DE_NULL,
		0u,															// flags
	};

	// create fence to wait for execution of queue
	const Unique<VkFence>					fence					(createFence(vk, vkDevice, &fenceCreateInfo));

	const VkSubmitInfo						submitInfo				=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1,															// commandBufferCount
		&primCmdBuf.get(),											// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	// submit primary buffer
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo, *fence));

	// wait for end of execution of queue
	VK_CHECK(vk.waitForFences(vkDevice, 1, &fence.get(), 0u, INFINITE_TIMEOUT));
	VK_CHECK(vk.resetFences(vkDevice, 1u, &fence.get()));

	// check if buffer has been executed
	VkResult result = vk.getEventStatus(vkDevice,*event);
	if (result != VK_EVENT_SET)
		return tcu::TestStatus::fail("oneTimeSubmitFlagPrimaryBufferTest FAILED");

	// record primary command buffer again - implicit reset because of VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
	VK_CHECK(vk.beginCommandBuffer(*primCmdBuf, &primCmdBufBeginInfo));
	{
		// allow execution of event during every stage of pipeline
		VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		// record setting event
		vk.cmdSetEvent(*primCmdBuf, *event,stageMask);
	}
	VK_CHECK(vk.endCommandBuffer(*primCmdBuf));

	// reset event
	VK_CHECK(vk.resetEvent(vkDevice, *event));

	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo, *fence));

	// wait for end of execution of queue
	VK_CHECK(vk.waitForFences(vkDevice, 1, &fence.get(), 0u, INFINITE_TIMEOUT));

	// check if buffer has been executed
	result = vk.getEventStatus(vkDevice,*event);
	if (result != VK_EVENT_SET)
		return tcu::TestStatus::fail("oneTimeSubmitFlagPrimaryBufferTest FAILED");
	else
		return tcu::TestStatus::pass("oneTimeSubmitFlagPrimaryBufferTest succeeded");
}

tcu::TestStatus oneTimeSubmitFlagSecondaryBufferTest(Context& context)
{

	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const VkQueue							queue					= context.getUniversalQueue();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			//	VkCommandPoolCreateFlags	flags;
		queueFamilyIndex,											//	deUint32					queueFamilyIndex;
	};

	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType			sType;
		DE_NULL,													//	const void*				pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							//	VkCommandBufferLevel		level;
		1u,															//	uint32_t					bufferCount;
	};

	const Unique<VkCommandBuffer>			primCmdBuf1				(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));
	const Unique<VkCommandBuffer>			primCmdBuf2				(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));

	// Secondary Command buffer
	const VkCommandBufferAllocateInfo		secCmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType			sType;
		DE_NULL,													//	const void*				pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_SECONDARY,							//	VkCommandBufferLevel		level;
		1u,															//	uint32_t					bufferCount;
	};
	const Unique<VkCommandBuffer>			secCmdBuf				(allocateCommandBuffer(vk, vkDevice, &secCmdBufParams));

	const VkCommandBufferBeginInfo			primCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		DE_NULL,
		0,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	const VkCommandBufferInheritanceInfo	secCmdBufInheritInfo	=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
		DE_NULL,
		(VkRenderPass)0u,											// renderPass
		0u,															// subpass
		(VkFramebuffer)0u,											// framebuffer
		VK_FALSE,													// occlusionQueryEnable
		(VkQueryControlFlags)0u,									// queryFlags
		(VkQueryPipelineStatisticFlags)0u,							// pipelineStatistics
	};
	const VkCommandBufferBeginInfo			secCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		DE_NULL,
		VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,				// flags
		&secCmdBufInheritInfo,
	};

	// Fill create info struct for event
	const VkEventCreateInfo					eventCreateInfo			=
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		DE_NULL,
		0u,
	};

	// create event that will be used to check if secondary command buffer has been executed
	const Unique<VkEvent>					event					(createEvent(vk, vkDevice, &eventCreateInfo));

	// reset event
	VK_CHECK(vk.resetEvent(vkDevice, *event));

	// record first primary command buffer
	VK_CHECK(vk.beginCommandBuffer(*primCmdBuf1, &primCmdBufBeginInfo));
	{
		// record secondary command buffer
		VK_CHECK(vk.beginCommandBuffer(*secCmdBuf, &secCmdBufBeginInfo));
		{
			// allow execution of event during every stage of pipeline
			VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

			// record setting event
			vk.cmdSetEvent(*secCmdBuf, *event,stageMask);
		}

		// end recording of secondary buffers
		VK_CHECK(vk.endCommandBuffer(*secCmdBuf));

		// execute secondary buffer
		vk.cmdExecuteCommands(*primCmdBuf1, 1, &secCmdBuf.get());
	}
	VK_CHECK(vk.endCommandBuffer(*primCmdBuf1));

	const VkFenceCreateInfo					fenceCreateInfo			=
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		DE_NULL,
		0u,															// flags
	};

	// create fence to wait for execution of queue
	const Unique<VkFence>					fence					(createFence(vk, vkDevice, &fenceCreateInfo));

	const VkSubmitInfo						submitInfo1				=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1,															// commandBufferCount
		&primCmdBuf1.get(),											// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo1, *fence));

	// wait for end of execution of queue
	VK_CHECK(vk.waitForFences(vkDevice, 1, &fence.get(), 0u, INFINITE_TIMEOUT));
	VK_CHECK(vk.resetFences(vkDevice, 1u, &fence.get()));

	// check if secondary buffer has been executed
	VkResult result = vk.getEventStatus(vkDevice,*event);
	if (result != VK_EVENT_SET)
		return tcu::TestStatus::fail("Submit Twice Secondary Command Buffer FAILED");

	// reset first primary buffer
	vk.resetCommandBuffer( *primCmdBuf1, 0u);

	// reset event to allow receiving it again
	VK_CHECK(vk.resetEvent(vkDevice, *event));

	// record secondary command buffer again
	VK_CHECK(vk.beginCommandBuffer(*secCmdBuf, &secCmdBufBeginInfo));
	{
		// allow execution of event during every stage of pipeline
		VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		// record setting event
		vk.cmdSetEvent(*secCmdBuf, *event,stageMask);
	}
	// end recording of secondary buffers
	VK_CHECK(vk.endCommandBuffer(*secCmdBuf));

	// record second primary command buffer
	VK_CHECK(vk.beginCommandBuffer(*primCmdBuf2, &primCmdBufBeginInfo));
	{
		// execute secondary buffer
		vk.cmdExecuteCommands(*primCmdBuf2, 1, &secCmdBuf.get());
	}
	// end recording
	VK_CHECK(vk.endCommandBuffer(*primCmdBuf2));

	// submit second primary buffer, the secondary should be executed too
	const VkSubmitInfo						submitInfo2				=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1,															// commandBufferCount
		&primCmdBuf2.get(),											// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo2, *fence));

	// wait for end of execution of queue
	VK_CHECK(vk.waitForFences(vkDevice, 1, &fence.get(), 0u, INFINITE_TIMEOUT));

	// check if secondary buffer has been executed
	result = vk.getEventStatus(vkDevice,*event);
	if (result != VK_EVENT_SET)
		return tcu::TestStatus::fail("oneTimeSubmitFlagSecondaryBufferTest FAILED");
	else
		return tcu::TestStatus::pass("oneTimeSubmitFlagSecondaryBufferTest succeeded");
}

tcu::TestStatus simultaneousUsePrimaryBufferTest(Context& context)
{

	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const VkQueue							queue					= context.getUniversalQueue();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			//	VkCommandPoolCreateFlags	flags;
		queueFamilyIndex,											//	deUint32					queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							//	VkCommandBufferLevel		level;
		1u,															//	uint32_t					bufferCount;
	};
	const Unique<VkCommandBuffer>			primCmdBuf				(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));
	const VkCommandBufferBeginInfo			primCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		DE_NULL,
		VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,				// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	// Fill create info struct for event
	const VkEventCreateInfo					eventCreateInfo			=
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		DE_NULL,
		0u,
	};

	// create event that will be used to check if secondary command buffer has been executed
	const Unique<VkEvent>					eventOne				(createEvent(vk, vkDevice, &eventCreateInfo));
	const Unique<VkEvent>					eventTwo				(createEvent(vk, vkDevice, &eventCreateInfo));

	// reset event
	VK_CHECK(vk.resetEvent(vkDevice, *eventOne));

	// record primary command buffer
	VK_CHECK(vk.beginCommandBuffer(*primCmdBuf, &primCmdBufBeginInfo));
	{
		// allow execution of event during every stage of pipeline
		VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		// wait for event
		vk.cmdWaitEvents(*primCmdBuf, 1u, &eventOne.get(), stageMask, stageMask, 0u, DE_NULL, 0u, DE_NULL, 0u, DE_NULL);

		// Set the second event
		vk.cmdSetEvent(*primCmdBuf, eventTwo.get(), stageMask);
	}
	VK_CHECK(vk.endCommandBuffer(*primCmdBuf));

	const VkFenceCreateInfo					fenceCreateInfo			=
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		DE_NULL,
		0u,															// flags
	};

	// create fence to wait for execution of queue
	const Unique<VkFence>					fence1					(createFence(vk, vkDevice, &fenceCreateInfo));
	const Unique<VkFence>					fence2					(createFence(vk, vkDevice, &fenceCreateInfo));


	const VkSubmitInfo						submitInfo				=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1,															// commandBufferCount
		&primCmdBuf.get(),											// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	// submit first buffer
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo, *fence1));

	// submit second buffer
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo, *fence2));

	// wait for both buffer to stop at event for 100 microseconds
	vk.waitForFences(vkDevice, 1, &fence1.get(), 0u, 100000);
	vk.waitForFences(vkDevice, 1, &fence2.get(), 0u, 100000);

	// set event
	VK_CHECK(vk.setEvent(vkDevice, *eventOne));

	// wait for end of execution of the first buffer
	VK_CHECK(vk.waitForFences(vkDevice, 1, &fence1.get(), 0u, INFINITE_TIMEOUT));
	// wait for end of execution of the second buffer
	VK_CHECK(vk.waitForFences(vkDevice, 1, &fence2.get(), 0u, INFINITE_TIMEOUT));

	// TODO: this will be true if the command buffer was executed only once
	// TODO: add some test that will say if it was executed twice

	// check if buffer has been executed
	VkResult result = vk.getEventStatus(vkDevice, *eventTwo);
	if (result == VK_EVENT_SET)
		return tcu::TestStatus::pass("simultaneous use - primary buffers test succeeded");
	else
		return tcu::TestStatus::fail("simultaneous use - primary buffers test FAILED");
}

tcu::TestStatus simultaneousUseSecondaryBufferTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const VkQueue							queue					= context.getUniversalQueue();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			//	VkCommandPoolCreateFlags	flags;
		queueFamilyIndex,											//	deUint32					queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType			sType;
		DE_NULL,													//	const void*				pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							//	VkCommandBufferLevel		level;
		1u,															//	uint32_t					bufferCount;
	};
	const Unique<VkCommandBuffer>			primCmdBuf				(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));

	// Secondary Command buffer params
	const VkCommandBufferAllocateInfo		secCmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType			sType;
		DE_NULL,													//	const void*				pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_SECONDARY,							//	VkCommandBufferLevel		level;
		1u,															//	uint32_t					bufferCount;
	};
	const Unique<VkCommandBuffer>			secCmdBuf				(allocateCommandBuffer(vk, vkDevice, &secCmdBufParams));

	const VkCommandBufferBeginInfo			primCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		DE_NULL,
		0,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	const VkCommandBufferInheritanceInfo	secCmdBufInheritInfo	=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
		DE_NULL,
		(VkRenderPass)0u,											// renderPass
		0u,															// subpass
		(VkFramebuffer)0u,											// framebuffer
		VK_FALSE,													// occlusionQueryEnable
		(VkQueryControlFlags)0u,									// queryFlags
		(VkQueryPipelineStatisticFlags)0u,							// pipelineStatistics
	};
	const VkCommandBufferBeginInfo			secCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		DE_NULL,
		VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,				// flags
		&secCmdBufInheritInfo,
	};

	// Fill create info struct for event
	const VkEventCreateInfo					eventCreateInfo			=
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		DE_NULL,
		0u,
	};

	// create event that will be used to check if secondary command buffer has been executed
	const Unique<VkEvent>					eventOne				(createEvent(vk, vkDevice, &eventCreateInfo));
	const Unique<VkEvent>					eventTwo				(createEvent(vk, vkDevice, &eventCreateInfo));

	// reset event
	VK_CHECK(vk.resetEvent(vkDevice, *eventOne));
	VK_CHECK(vk.resetEvent(vkDevice, *eventTwo));

	// record secondary command buffer
	VK_CHECK(vk.beginCommandBuffer(*secCmdBuf, &secCmdBufBeginInfo));
	{
		// allow execution of event during every stage of pipeline
		VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		// wait for event
		vk.cmdWaitEvents(*secCmdBuf, 1, &eventOne.get(), stageMask, stageMask, 0, DE_NULL, 0u, DE_NULL, 0u, DE_NULL);

		// reset event
		vk.cmdSetEvent(*secCmdBuf, *eventTwo, stageMask);
	}
	// end recording of secondary buffers
	VK_CHECK(vk.endCommandBuffer(*secCmdBuf));

	// record primary command buffer
	VK_CHECK(vk.beginCommandBuffer(*primCmdBuf, &primCmdBufBeginInfo));
	{
		// execute secondary buffer
		vk.cmdExecuteCommands(*primCmdBuf, 1, &secCmdBuf.get());
	}
	VK_CHECK(vk.endCommandBuffer(*primCmdBuf));

	const VkFenceCreateInfo					fenceCreateInfo			=
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		DE_NULL,
		0u,															// flags
	};

	// create fence to wait for execution of queue
	const Unique<VkFence>					fence					(createFence(vk, vkDevice, &fenceCreateInfo));

	const VkSubmitInfo						submitInfo				=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1,															// commandBufferCount
		&primCmdBuf.get(),											// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	// submit primary buffer, the secondary should be executed too
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo, *fence));

	// wait for both buffers to stop at event for 100 microseconds
	vk.waitForFences(vkDevice, 1, &fence.get(), 0u, 100000);

	// set event
	VK_CHECK(vk.setEvent(vkDevice, *eventOne));

	// wait for end of execution of queue
	VK_CHECK(vk.waitForFences(vkDevice, 1, &fence.get(), 0u, INFINITE_TIMEOUT));

	// TODO: this will be true if the command buffer was executed only once
	// TODO: add some test that will say if it was executed twice

	// check if secondary buffer has been executed
	VkResult result = vk.getEventStatus(vkDevice,*eventTwo);
	if (result == VK_EVENT_SET)
		return tcu::TestStatus::pass("Simulatous Secondary Command Buffer Execution succeeded");
	else
		return tcu::TestStatus::fail("Simulatous Secondary Command Buffer Execution FAILED");
}

tcu::TestStatus recordBufferQueryPreciseWithFlagTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			// flags;
		queueFamilyIndex,											// queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		primCmdBufParams		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				// sType;
		DE_NULL,													// pNext;
		*cmdPool,													// pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							// level;
		1u,															// flags;
	};
	const Unique<VkCommandBuffer>			primCmdBuf				(allocateCommandBuffer(vk, vkDevice, &primCmdBufParams));

	// Secondary Command buffer params
	const VkCommandBufferAllocateInfo		secCmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				// sType;
		DE_NULL,													// pNext;
		*cmdPool,													// pool;
		VK_COMMAND_BUFFER_LEVEL_SECONDARY,							// level;
		1u,															// flags;
	};
	const Unique<VkCommandBuffer>			secCmdBuf				(allocateCommandBuffer(vk, vkDevice, &secCmdBufParams));

	const VkCommandBufferBeginInfo			primBufferBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,				// sType
		DE_NULL,													// pNext
		0u,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	const VkCommandBufferInheritanceInfo	secBufferInheritInfo	=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
		DE_NULL,
		0u,															// renderPass
		0u,															// subpass
		0u,															// framebuffer
		VK_TRUE,													// occlusionQueryEnable
		VK_QUERY_CONTROL_PRECISE_BIT,								// queryFlags
		(VkQueryPipelineStatisticFlags)0u,							// pipelineStatistics
	};
	const VkCommandBufferBeginInfo			secBufferBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,				// sType
		DE_NULL,													// pNext
		0u,															// flags
		&secBufferInheritInfo,
	};

	// Create an occlusion query with VK_QUERY_CONTROL_PRECISE_BIT set
	const VkQueryPoolCreateInfo				queryPoolCreateInfo		=
	{
		VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,					// sType
		DE_NULL,													// pNext
		VK_QUERY_CONTROL_PRECISE_BIT,								// flags
		VK_QUERY_TYPE_OCCLUSION,									// queryType
		1u,															// entryCount
		0u,															// pipelineStatistics
	};
	Unique<VkQueryPool>						queryPool				(createQueryPool(vk, vkDevice, &queryPoolCreateInfo));

	VK_CHECK(vk.beginCommandBuffer(secCmdBuf.get(), &secBufferBeginInfo));
	VK_CHECK(vk.endCommandBuffer(secCmdBuf.get()));

	VK_CHECK(vk.beginCommandBuffer(primCmdBuf.get(), &primBufferBeginInfo));
	{
		vk.cmdResetQueryPool(primCmdBuf.get(), queryPool.get(), 0u, 1u);
		vk.cmdBeginQuery(primCmdBuf.get(), queryPool.get(), 0u, VK_QUERY_CONTROL_PRECISE_BIT);
		{
			vk.cmdExecuteCommands(primCmdBuf.get(), 1u, &secCmdBuf.get());
		}
		vk.cmdEndQuery(primCmdBuf.get(), queryPool.get(), 0u);
	}
	VK_CHECK(vk.endCommandBuffer(primCmdBuf.get()));

	return tcu::TestStatus::pass("Successfully recorded a secondary command buffer allowing a precise occlusion query.");
}

tcu::TestStatus recordBufferQueryImpreciseWithFlagTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			// flags;
		queueFamilyIndex,											// queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		primCmdBufParams		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				// sType;
		DE_NULL,													// pNext;
		*cmdPool,													// pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							// level;
		1u,															// flags;
	};
	const Unique<VkCommandBuffer>			primCmdBuf				(allocateCommandBuffer(vk, vkDevice, &primCmdBufParams));

	// Secondary Command buffer params
	const VkCommandBufferAllocateInfo		secCmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				// sType;
		DE_NULL,													// pNext;
		*cmdPool,													// pool;
		VK_COMMAND_BUFFER_LEVEL_SECONDARY,							// level;
		1u,															// flags;
	};
	const Unique<VkCommandBuffer>			secCmdBuf				(allocateCommandBuffer(vk, vkDevice, &secCmdBufParams));

	const VkCommandBufferBeginInfo			primBufferBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,				// sType
		DE_NULL,													// pNext
		0u,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	const VkCommandBufferInheritanceInfo	secBufferInheritInfo	=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
		DE_NULL,
		0u,															// renderPass
		0u,															// subpass
		0u,															// framebuffer
		VK_TRUE,													// occlusionQueryEnable
		VK_QUERY_CONTROL_PRECISE_BIT,								// queryFlags
		(VkQueryPipelineStatisticFlags)0u,							// pipelineStatistics
	};
	const VkCommandBufferBeginInfo			secBufferBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,				// sType
		DE_NULL,													// pNext
		0u,															// flags
		&secBufferInheritInfo,
	};

	// Create an occlusion query with VK_QUERY_CONTROL_PRECISE_BIT set
	const VkQueryPoolCreateInfo				queryPoolCreateInfo		=
	{
		VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,					// sType
		DE_NULL,													// pNext
		0u,															// flags
		VK_QUERY_TYPE_OCCLUSION,									// queryType
		1u,															// entryCount
		0u,															// pipelineStatistics
	};
	Unique<VkQueryPool>						queryPool				(createQueryPool(vk, vkDevice, &queryPoolCreateInfo));

	VK_CHECK(vk.beginCommandBuffer(secCmdBuf.get(), &secBufferBeginInfo));
	VK_CHECK(vk.endCommandBuffer(secCmdBuf.get()));

	VK_CHECK(vk.beginCommandBuffer(primCmdBuf.get(), &primBufferBeginInfo));
	{
		vk.cmdResetQueryPool(primCmdBuf.get(), queryPool.get(), 0u, 1u);
		vk.cmdBeginQuery(primCmdBuf.get(), queryPool.get(), 0u, VK_QUERY_CONTROL_PRECISE_BIT);
		{
			vk.cmdExecuteCommands(primCmdBuf.get(), 1u, &secCmdBuf.get());
		}
		vk.cmdEndQuery(primCmdBuf.get(), queryPool.get(), 0u);
	}
	VK_CHECK(vk.endCommandBuffer(primCmdBuf.get()));

	return tcu::TestStatus::pass("Successfully recorded a secondary command buffer allowing a precise occlusion query.");
}

tcu::TestStatus recordBufferQueryImpreciseWithoutFlagTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			// flags;
		queueFamilyIndex,											// queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		primCmdBufParams		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				// sType;
		DE_NULL,													// pNext;
		*cmdPool,													// pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							// level;
		1u,															// flags;
	};
	const Unique<VkCommandBuffer>			primCmdBuf				(allocateCommandBuffer(vk, vkDevice, &primCmdBufParams));

	// Secondary Command buffer params
	const VkCommandBufferAllocateInfo		secCmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				// sType;
		DE_NULL,													// pNext;
		*cmdPool,													// pool;
		VK_COMMAND_BUFFER_LEVEL_SECONDARY,							// level;
		1u,															// flags;
	};
	const Unique<VkCommandBuffer>			secCmdBuf				(allocateCommandBuffer(vk, vkDevice, &secCmdBufParams));

	const VkCommandBufferBeginInfo			primBufferBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,				// sType
		DE_NULL,													// pNext
		0u,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	const VkCommandBufferInheritanceInfo	secBufferInheritInfo	=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
		DE_NULL,
		0u,															// renderPass
		0u,															// subpass
		0u,															// framebuffer
		VK_TRUE,													// occlusionQueryEnable
		0u,															// queryFlags
		(VkQueryPipelineStatisticFlags)0u,							// pipelineStatistics
	};
	const VkCommandBufferBeginInfo			secBufferBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,				// sType
		DE_NULL,													// pNext
		0u,															// flags
		&secBufferInheritInfo,
	};

	// Create an occlusion query with VK_QUERY_CONTROL_PRECISE_BIT set
	const VkQueryPoolCreateInfo				queryPoolCreateInfo		=
	{
		VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,					// sType
		DE_NULL,													// pNext
		(VkQueryPoolCreateFlags)0,
		VK_QUERY_TYPE_OCCLUSION,
		1u,
		0u,
	};
	Unique<VkQueryPool>						queryPool				(createQueryPool(vk, vkDevice, &queryPoolCreateInfo));

	VK_CHECK(vk.beginCommandBuffer(secCmdBuf.get(), &secBufferBeginInfo));
	VK_CHECK(vk.endCommandBuffer(secCmdBuf.get()));

	VK_CHECK(vk.beginCommandBuffer(primCmdBuf.get(), &primBufferBeginInfo));
	{
		vk.cmdResetQueryPool(primCmdBuf.get(), queryPool.get(), 0u, 1u);
		vk.cmdBeginQuery(primCmdBuf.get(), queryPool.get(), 0u, VK_QUERY_CONTROL_PRECISE_BIT);
		{
			vk.cmdExecuteCommands(primCmdBuf.get(), 1u, &secCmdBuf.get());
		}
		vk.cmdEndQuery(primCmdBuf.get(), queryPool.get(), 0u);
	}
	VK_CHECK(vk.endCommandBuffer(primCmdBuf.get()));

	return tcu::TestStatus::pass("Successfully recorded a secondary command buffer allowing a precise occlusion query.");
}

/******** 19.4. Command Buffer Submission (6.4 in VK 1.0 Spec) ****************/
tcu::TestStatus submitBufferCountNonZero(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const VkQueue							queue					= context.getUniversalQueue();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const deUint32							BUFFER_COUNT			= 5u;

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext;
		0u,															// flags;
		queueFamilyIndex,											// queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				// sType;
		DE_NULL,													// pNext;
		*cmdPool,													// pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							// level;
		BUFFER_COUNT,												// bufferCount;
	};
	VkCommandBuffer cmdBuffers[BUFFER_COUNT];
	VK_CHECK(vk.allocateCommandBuffers(vkDevice, &cmdBufParams, cmdBuffers));

	const VkCommandBufferBeginInfo			cmdBufBeginInfo			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,				// sType
		DE_NULL,													// pNext
		0u,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	const VkEventCreateInfo					eventCreateInfo			=
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,						// sType;
		DE_NULL,													// pNext;
		0u,															// flags;
	};

	std::vector<VkEventSp>					events;
	for (deUint32 ndx = 0; ndx < BUFFER_COUNT; ++ndx)
	{
		events.push_back(VkEventSp(new vk::Unique<VkEvent>(createEvent(vk, vkDevice, &eventCreateInfo, DE_NULL))));
	}

	// Record the command buffers
	for (deUint32 ndx = 0; ndx < BUFFER_COUNT; ++ndx)
	{
		VK_CHECK(vk.beginCommandBuffer(cmdBuffers[ndx], &cmdBufBeginInfo));
		{
			vk.cmdSetEvent(cmdBuffers[ndx], events[ndx]->get(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
		}
		VK_CHECK(vk.endCommandBuffer(cmdBuffers[ndx]));
	}

	// We'll use a fence to wait for the execution of the queue
	const VkFenceCreateInfo					fenceCreateInfo			=
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,						// sType;
		DE_NULL,													// pNext;
		0u,															// flags
	};
	const Unique<VkFence>					fence					(createFence(vk, vkDevice, &fenceCreateInfo));

	const VkSubmitInfo						submitInfo				=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		BUFFER_COUNT,												// commandBufferCount
		cmdBuffers,													// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	// Submit the alpha command buffer to the queue
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo, fence.get()));
	// Wait for the queue
	VK_CHECK(vk.waitForFences(vkDevice, 1u, &fence.get(), VK_TRUE, INFINITE_TIMEOUT));

	// Check if the buffers were executed
	tcu::TestStatus testResult = tcu::TestStatus::incomplete();

	for (deUint32 ndx = 0; ndx < BUFFER_COUNT; ++ndx)
	{
		if (vk.getEventStatus(vkDevice, events[ndx]->get()) != VK_EVENT_SET)
		{
			testResult = tcu::TestStatus::fail("Failed to set the event.");
			break;
		}
	}

	if (!testResult.isComplete())
		testResult = tcu::TestStatus::pass("All buffers were submitted and executed correctly.");

	return testResult;
}

tcu::TestStatus submitBufferCountEqualZero(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const VkQueue							queue					= context.getUniversalQueue();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const deUint32							BUFFER_COUNT			= 2u;

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext;
		0u,															// flags;
		queueFamilyIndex,											// queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				// sType;
		DE_NULL,													// pNext;
		*cmdPool,													// pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							// level;
		BUFFER_COUNT,												// bufferCount;
	};
	VkCommandBuffer cmdBuffers[BUFFER_COUNT];
	VK_CHECK(vk.allocateCommandBuffers(vkDevice, &cmdBufParams, cmdBuffers));

	const VkCommandBufferBeginInfo			cmdBufBeginInfo			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,				// sType
		DE_NULL,													// pNext
		0u,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	const VkEventCreateInfo					eventCreateInfo			=
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,						// sType;
		DE_NULL,													// pNext;
		0u,															// flags;
	};

	std::vector<VkEventSp>					events;
	for (deUint32 ndx = 0; ndx < BUFFER_COUNT; ++ndx)
		events.push_back(VkEventSp(new vk::Unique<VkEvent>(createEvent(vk, vkDevice, &eventCreateInfo, DE_NULL))));

	// Record the command buffers
	for (deUint32 ndx = 0; ndx < BUFFER_COUNT; ++ndx)
	{
		VK_CHECK(vk.beginCommandBuffer(cmdBuffers[ndx], &cmdBufBeginInfo));
		{
			vk.cmdSetEvent(cmdBuffers[ndx], events[ndx]->get(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
		}
		VK_CHECK(vk.endCommandBuffer(cmdBuffers[ndx]));
	}

	// We'll use a fence to wait for the execution of the queue
	const VkFenceCreateInfo					fenceCreateInfo			=
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,						// sType;
		DE_NULL,													// pNext;
		0u,															// flags
	};
	const Unique<VkFence>					fenceZero				(createFence(vk, vkDevice, &fenceCreateInfo));
	const Unique<VkFence>					fenceOne				(createFence(vk, vkDevice, &fenceCreateInfo));

	const VkSubmitInfo						submitInfoCountZero		=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1u,															// commandBufferCount
		&cmdBuffers[0],												// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	const VkSubmitInfo						submitInfoCountOne		=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1u,															// commandBufferCount
		&cmdBuffers[1],												// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	// Submit the command buffers to the queue
	// We're performing two submits to make sure that the first one has
	// a chance to be processed before we check the event's status
	VK_CHECK(vk.queueSubmit(queue, 0, &submitInfoCountZero, fenceZero.get()));
	VK_CHECK(vk.queueSubmit(queue, 1, &submitInfoCountOne, fenceOne.get()));

	const VkFence							fences[]				=
	{
		fenceZero.get(),
		fenceOne.get(),
	};

	// Wait for the queue
	VK_CHECK(vk.waitForFences(vkDevice, (deUint32)DE_LENGTH_OF_ARRAY(fences), fences, VK_TRUE, INFINITE_TIMEOUT));

	// Check if the first buffer was executed
	tcu::TestStatus testResult = tcu::TestStatus::incomplete();

	if (vk.getEventStatus(vkDevice, events[0]->get()) == VK_EVENT_SET)
		testResult = tcu::TestStatus::fail("The first event was signaled.");
	else
		testResult = tcu::TestStatus::pass("The first submission was ignored.");

	return testResult;
}

tcu::TestStatus submitBufferNullFence(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const VkQueue							queue					= context.getUniversalQueue();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const short								BUFFER_COUNT			= 2;

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext;
		0u,															// flags;
		queueFamilyIndex,											// queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				// sType;
		DE_NULL,													// pNext;
		*cmdPool,													// pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							// level;
		1u,															// bufferCount;
	};
	VkCommandBuffer cmdBuffers[BUFFER_COUNT];
	for (deUint32 ndx = 0; ndx < BUFFER_COUNT; ++ndx)
		VK_CHECK(vk.allocateCommandBuffers(vkDevice, &cmdBufParams, &cmdBuffers[ndx]));

	const VkCommandBufferBeginInfo			cmdBufBeginInfo			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,				// sType
		DE_NULL,													// pNext
		0u,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	const VkEventCreateInfo					eventCreateInfo			=
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,						// sType;
		DE_NULL,													// pNext;
		0u,															// flags;
	};

	std::vector<VkEventSp>					events;
	for (deUint32 ndx = 0; ndx < BUFFER_COUNT; ++ndx)
		events.push_back(VkEventSp(new vk::Unique<VkEvent>(createEvent(vk, vkDevice, &eventCreateInfo, DE_NULL))));

	// Record the command buffers
	for (deUint32 ndx = 0; ndx < BUFFER_COUNT; ++ndx)
	{
		VK_CHECK(vk.beginCommandBuffer(cmdBuffers[ndx], &cmdBufBeginInfo));
		{
			vk.cmdSetEvent(cmdBuffers[ndx], events[ndx]->get(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
		}
		VK_CHECK(vk.endCommandBuffer(cmdBuffers[ndx]));
	}

	// We'll use a fence to wait for the execution of the queue
	const VkFenceCreateInfo					fenceCreateInfo			=
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,						// sType;
		DE_NULL,													// pNext;
		0u,															// flags
	};
	const Unique<VkFence>					fence					(createFence(vk, vkDevice, &fenceCreateInfo));

	const VkSubmitInfo						submitInfoNullFence		=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1u,															// commandBufferCount
		&cmdBuffers[0],												// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	const VkSubmitInfo						submitInfoNonNullFence	=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1u,															// commandBufferCount
		&cmdBuffers[1],												// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	// Perform two submissions - one with no fence, the other one with a valid
	// fence Hoping submitting the other buffer will give the first one time to
	// execute
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfoNullFence, DE_NULL));
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfoNonNullFence, fence.get()));

	// Wait for the queue
	VK_CHECK(vk.waitForFences(vkDevice, 1u, &fence.get(), VK_TRUE, INFINITE_TIMEOUT));

	tcu::TestStatus testResult = tcu::TestStatus::incomplete();

	if (vk.getEventStatus(vkDevice, events[0]->get()) != VK_EVENT_SET)
		testResult = tcu::TestStatus::fail("The first event was not signaled -> the buffer was not executed.");
	else
		testResult = tcu::TestStatus::pass("The first event was signaled -> the buffer with null fence submitted and executed correctly.");

	return testResult;
}

/******** 19.5. Secondary Command Buffer Execution (6.6 in VK 1.0 Spec) *******/
tcu::TestStatus executeSecondaryBufferTest(Context& context)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const VkQueue							queue					= context.getUniversalQueue();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			// flags;
		queueFamilyIndex,											// queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				// sType;
		DE_NULL,													// pNext;
		*cmdPool,													// commandPool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							// level;
		1u,															// bufferCount;
	};
	const Unique<VkCommandBuffer>			primCmdBuf				(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));

	// Secondary Command buffer
	const VkCommandBufferAllocateInfo		secCmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				// sType;
		DE_NULL,													// pNext;
		*cmdPool,													// commandPool;
		VK_COMMAND_BUFFER_LEVEL_SECONDARY,							// level;
		1u,															// bufferCount;
	};
	const Unique<VkCommandBuffer>			secCmdBuf				(allocateCommandBuffer(vk, vkDevice, &secCmdBufParams));

	const VkCommandBufferBeginInfo			primCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,				// sType
		DE_NULL,													// pNext
		0u,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	const VkCommandBufferInheritanceInfo	secCmdBufInheritInfo	=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
		DE_NULL,
		DE_NULL,													// renderPass
		0u,															// subpass
		DE_NULL,													// framebuffer
		VK_FALSE,													// occlusionQueryEnable
		(VkQueryControlFlags)0u,									// queryFlags
		(VkQueryPipelineStatisticFlags)0u,							// pipelineStatistics
	};
	const VkCommandBufferBeginInfo			secCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,				// sType
		DE_NULL,													// pNext
		0u,															// flags
		&secCmdBufInheritInfo,
	};

	// Fill create info struct for event
	const VkEventCreateInfo					eventCreateInfo			=
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		DE_NULL,
		0u,
	};

	// create event that will be used to check if secondary command buffer has been executed
	const Unique<VkEvent>					event					(createEvent(vk, vkDevice, &eventCreateInfo));

	// reset event
	VK_CHECK(vk.resetEvent(vkDevice, *event));

	// record secondary command buffer
	VK_CHECK(vk.beginCommandBuffer(*secCmdBuf, &secCmdBufBeginInfo));
	{
		// allow execution of event during every stage of pipeline
		VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		// record setting event
		vk.cmdSetEvent(*secCmdBuf, *event, stageMask);
	}
	// end recording of the secondary buffer
	VK_CHECK(vk.endCommandBuffer(*secCmdBuf));

	// record primary command buffer
	VK_CHECK(vk.beginCommandBuffer(*primCmdBuf, &primCmdBufBeginInfo));
	{
		// execute secondary buffer
		vk.cmdExecuteCommands(*primCmdBuf, 1u, &secCmdBuf.get());
	}
	VK_CHECK(vk.endCommandBuffer(*primCmdBuf));

	const VkFenceCreateInfo					fenceCreateInfo			=
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		DE_NULL,
		0u,															// flags
	};

	// create fence to wait for execution of queue
	const Unique<VkFence>					fence					(createFence(vk, vkDevice, &fenceCreateInfo));
	const VkSubmitInfo						submitInfo				=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1u,															// commandBufferCount
		&primCmdBuf.get(),											// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	// submit primary buffer, the secondary should be executed too
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo, fence.get()));

	// wait for end of execution of queue
	VK_CHECK(vk.waitForFences(vkDevice, 1, &fence.get(), 0u, INFINITE_TIMEOUT));

	// check if secondary buffer has been executed
	VkResult result = vk.getEventStatus(vkDevice, *event);
	if (result == VK_EVENT_SET)
		return tcu::TestStatus::pass("executeSecondaryBufferTest succeeded");

	return tcu::TestStatus::fail("executeSecondaryBufferTest FAILED");
}

tcu::TestStatus executeSecondaryBufferTwiceTest(Context& context)
{
	const deUint32							BUFFER_COUNT			= 10u;
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const VkQueue							queue					= context.getUniversalQueue();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();

	const VkCommandPoolCreateInfo			cmdPoolParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					//	VkStructureType				sType;
		DE_NULL,													//	const void*					pNext;
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			//	VkCommandPoolCreateFlags	flags;
		queueFamilyIndex,											//	deUint32					queueFamilyIndex;
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType			sType;
		DE_NULL,													//	const void*				pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							//	VkCommandBufferLevel		level;
		1u,															//	uint32_t					bufferCount;
	};
	const Unique<VkCommandBuffer>			primCmdBufOne			(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));
	const Unique<VkCommandBuffer>			primCmdBufTwo			(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));

	// Secondary Command buffers params
	const VkCommandBufferAllocateInfo		secCmdBufParams			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				//	VkStructureType			sType;
		DE_NULL,													//	const void*				pNext;
		*cmdPool,													//	VkCommandPool				pool;
		VK_COMMAND_BUFFER_LEVEL_SECONDARY,							//	VkCommandBufferLevel		level;
		BUFFER_COUNT,												//	uint32_t					bufferCount;
	};
	VkCommandBuffer cmdBuffers[BUFFER_COUNT];
	VK_CHECK(vk.allocateCommandBuffers(vkDevice, &secCmdBufParams, cmdBuffers));

	const VkCommandBufferBeginInfo			primCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		DE_NULL,
		0,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	const VkCommandBufferInheritanceInfo	secCmdBufInheritInfo	=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
		DE_NULL,
		(VkRenderPass)0u,											// renderPass
		0u,															// subpass
		(VkFramebuffer)0u,											// framebuffer
		VK_FALSE,													// occlusionQueryEnable
		(VkQueryControlFlags)0u,									// queryFlags
		(VkQueryPipelineStatisticFlags)0u,							// pipelineStatistics
	};
	const VkCommandBufferBeginInfo			secCmdBufBeginInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		DE_NULL,
		VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,				// flags
		&secCmdBufInheritInfo,
	};

	// Fill create info struct for event
	const VkEventCreateInfo					eventCreateInfo			=
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		DE_NULL,
		0u,
	};

	// create event that will be used to check if secondary command buffer has been executed
	const Unique<VkEvent>					eventOne				(createEvent(vk, vkDevice, &eventCreateInfo));

	// reset event
	VK_CHECK(vk.resetEvent(vkDevice, *eventOne));

	for (deUint32 ndx = 0; ndx < BUFFER_COUNT; ++ndx)
	{
		// record secondary command buffer
		VK_CHECK(vk.beginCommandBuffer(cmdBuffers[ndx], &secCmdBufBeginInfo));
		{
			// allow execution of event during every stage of pipeline
			VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

			// wait for event
			vk.cmdWaitEvents(cmdBuffers[ndx], 1, &eventOne.get(), stageMask, stageMask, 0, DE_NULL, 0u, DE_NULL, 0u, DE_NULL);
		}
		// end recording of secondary buffers
		VK_CHECK(vk.endCommandBuffer(cmdBuffers[ndx]));
	};

	// record primary command buffer one
	VK_CHECK(vk.beginCommandBuffer(*primCmdBufOne, &primCmdBufBeginInfo));
	{
		// execute one secondary buffer
		vk.cmdExecuteCommands(*primCmdBufOne, 1, cmdBuffers );
	}
	VK_CHECK(vk.endCommandBuffer(*primCmdBufOne));

	// record primary command buffer two
	VK_CHECK(vk.beginCommandBuffer(*primCmdBufTwo, &primCmdBufBeginInfo));
	{
		// execute one secondary buffer with all buffers
		vk.cmdExecuteCommands(*primCmdBufTwo, BUFFER_COUNT, cmdBuffers );
	}
	VK_CHECK(vk.endCommandBuffer(*primCmdBufTwo));

	const VkFenceCreateInfo					fenceCreateInfo			=
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		DE_NULL,
		0u,															// flags
	};

	// create fence to wait for execution of queue
	const Unique<VkFence>					fenceOne				(createFence(vk, vkDevice, &fenceCreateInfo));
	const Unique<VkFence>					fenceTwo				(createFence(vk, vkDevice, &fenceCreateInfo));

	const VkSubmitInfo						submitInfoOne			=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1,															// commandBufferCount
		&primCmdBufOne.get(),										// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	// submit primary buffer, the secondary should be executed too
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfoOne, *fenceOne));

	// wait for buffer to stop at event for 100 microseconds
	vk.waitForFences(vkDevice, 1, &fenceOne.get(), 0u, 100000);

	const VkSubmitInfo						submitInfoTwo			=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1,															// commandBufferCount
		&primCmdBufTwo.get(),										// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};

	// submit second primary buffer, the secondary should be executed too
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfoTwo, *fenceTwo));

	// wait for all buffers to stop at event for 100 microseconds
	vk.waitForFences(vkDevice, 1, &fenceOne.get(), 0u, 100000);

	// now all buffers are waiting at eventOne
	// set event eventOne
	VK_CHECK(vk.setEvent(vkDevice, *eventOne));

	// wait for end of execution of fenceOne
	VK_CHECK(vk.waitForFences(vkDevice, 1, &fenceOne.get(), 0u, INFINITE_TIMEOUT));

	// wait for end of execution of second queue
	VK_CHECK(vk.waitForFences(vkDevice, 1, &fenceTwo.get(), 0u, INFINITE_TIMEOUT));

	return tcu::TestStatus::pass("executeSecondaryBufferTwiceTest succeeded");
}

/******** 19.6. Commands Allowed Inside Command Buffers (6.7 in VK 1.0 Spec) **/
tcu::TestStatus orderBindPipelineTest(Context& context)
{
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const VkDevice							device					= context.getDevice();
	const VkQueue							queue					= context.getUniversalQueue();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();
	Allocator&								allocator				= context.getDefaultAllocator();
	const ComputeInstanceResultBuffer		result					(vk, device, allocator);

	enum
	{
		ADDRESSABLE_SIZE = 256, // allocate a lot more than required
	};

	const tcu::Vec4							colorA1					= tcu::Vec4(0.0f, 1.0f, 0.0f, 1.0f);
	const tcu::Vec4							colorA2					= tcu::Vec4(1.0f, 1.0f, 0.0f, 1.0f);
	const tcu::Vec4							colorB1					= tcu::Vec4(1.0f, 0.0f, 0.0f, 1.0f);
	const tcu::Vec4							colorB2					= tcu::Vec4(0.0f, 0.0f, 1.0f, 1.0f);

	const deUint32							dataOffsetA				= (0u);
	const deUint32							dataOffsetB				= (0u);
	const deUint32							viewOffsetA				= (0u);
	const deUint32							viewOffsetB				= (0u);
	const deUint32							bufferSizeA				= dataOffsetA + ADDRESSABLE_SIZE;
	const deUint32							bufferSizeB				= dataOffsetB + ADDRESSABLE_SIZE;

	de::MovePtr<Allocation>					bufferMemA;
	const Unique<VkBuffer>					bufferA					(createColorDataBuffer(dataOffsetA, bufferSizeA, colorA1, colorA2, &bufferMemA, context));

	de::MovePtr<Allocation>					bufferMemB;
	const Unique<VkBuffer>					bufferB					(createColorDataBuffer(dataOffsetB, bufferSizeB, colorB1, colorB2, &bufferMemB, context));

	const Unique<VkDescriptorSetLayout>		descriptorSetLayout		(createDescriptorSetLayout(context));
	const Unique<VkDescriptorPool>			descriptorPool			(createDescriptorPool(context));
	const Unique<VkDescriptorSet>			descriptorSet			(createDescriptorSet(*descriptorPool, *descriptorSetLayout, *bufferA, viewOffsetA, *bufferB, viewOffsetB, result.getBuffer(), context));
	const VkDescriptorSet					descriptorSets[]		= { *descriptorSet };
	const int								numDescriptorSets		= DE_LENGTH_OF_ARRAY(descriptorSets);

	const VkPipelineLayoutCreateInfo layoutCreateInfo =
	{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,				// sType
		DE_NULL,													// pNext
		(VkPipelineLayoutCreateFlags)0,
		numDescriptorSets,											// setLayoutCount
		&descriptorSetLayout.get(),									// pSetLayouts
		0u,															// pushConstantRangeCount
		DE_NULL,													// pPushConstantRanges
	};
	Unique<VkPipelineLayout>				pipelineLayout			(createPipelineLayout(vk, device, &layoutCreateInfo));

	const Unique<VkShaderModule>			computeModuleGood		(createShaderModule(vk, device, context.getBinaryCollection().get("compute_good"), (VkShaderModuleCreateFlags)0u));
	const Unique<VkShaderModule>			computeModuleBad		(createShaderModule(vk, device, context.getBinaryCollection().get("compute_bad"),  (VkShaderModuleCreateFlags)0u));

	const VkPipelineShaderStageCreateInfo	shaderCreateInfoGood	=
	{
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		DE_NULL,
		(VkPipelineShaderStageCreateFlags)0,
		VK_SHADER_STAGE_COMPUTE_BIT,								// stage
		*computeModuleGood,											// shader
		"main",
		DE_NULL,													// pSpecializationInfo
	};

	const VkPipelineShaderStageCreateInfo	shaderCreateInfoBad	=
	{
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		DE_NULL,
		(vk::VkPipelineShaderStageCreateFlags)0,
		vk::VK_SHADER_STAGE_COMPUTE_BIT,							// stage
		*computeModuleBad,											// shader
		"main",
		DE_NULL,													// pSpecializationInfo
	};

	const VkComputePipelineCreateInfo		createInfoGood			=
	{
		VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
		DE_NULL,
		0u,															// flags
		shaderCreateInfoGood,										// cs
		*pipelineLayout,											// layout
		(vk::VkPipeline)0,											// basePipelineHandle
		0u,															// basePipelineIndex
	};

	const VkComputePipelineCreateInfo		createInfoBad			=
	{
		VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
		DE_NULL,
		0u,															// flags
		shaderCreateInfoBad,										// cs
		*pipelineLayout,											// descriptorSetLayout.get()
		(VkPipeline)0,												// basePipelineHandle
		0u,															// basePipelineIndex
	};

	const Unique<VkPipeline>				pipelineGood			(createComputePipeline(vk, device, (VkPipelineCache)0u, &createInfoGood));
	const Unique<VkPipeline>				pipelineBad				(createComputePipeline(vk, device, (VkPipelineCache)0u, &createInfoBad));

	const VkAccessFlags						inputBit				= (VK_ACCESS_UNIFORM_READ_BIT);
	const VkBufferMemoryBarrier				bufferBarriers[]		=
	{
		{
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
			DE_NULL,
			VK_ACCESS_HOST_WRITE_BIT,									// outputMask
			inputBit,													// inputMask
			VK_QUEUE_FAMILY_IGNORED,									// srcQueueFamilyIndex
			VK_QUEUE_FAMILY_IGNORED,									// destQueueFamilyIndex
			*bufferA,													// buffer
			(VkDeviceSize)0u,											// offset
			(VkDeviceSize)bufferSizeA,									// size
		},
		{
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
			DE_NULL,
			VK_ACCESS_HOST_WRITE_BIT,									// outputMask
			inputBit,													// inputMask
			VK_QUEUE_FAMILY_IGNORED,									// srcQueueFamilyIndex
			VK_QUEUE_FAMILY_IGNORED,									// destQueueFamilyIndex
			*bufferB,													// buffer
			(VkDeviceSize)0u,											// offset
			(VkDeviceSize)bufferSizeB,									// size
		}
	};

	const deUint32							numSrcBuffers			= 1u;

	const deUint32* const					dynamicOffsets			= (DE_NULL);
	const deUint32							numDynamicOffsets		= (0);
	const int								numPreBarriers			= numSrcBuffers;
	const vk::VkBufferMemoryBarrier* const	postBarriers			= result.getResultReadBarrier();
	const int								numPostBarriers			= 1;
	const tcu::Vec4							refQuadrantValue14		= (colorA2);
	const tcu::Vec4							refQuadrantValue23		= (colorA1);
	const tcu::Vec4							references[4]			=
	{
		refQuadrantValue14,
		refQuadrantValue23,
		refQuadrantValue23,
		refQuadrantValue14,
	};
	tcu::Vec4								results[4];

	// submit and wait begin

	const tcu::UVec3 numWorkGroups = tcu::UVec3(4, 1u, 1);

	const VkCommandPoolCreateInfo			cmdPoolCreateInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType;
		DE_NULL,													// pNext
		VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,						// flags
		queueFamilyIndex,											// queueFamilyIndex
	};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, device, &cmdPoolCreateInfo));

	const VkFenceCreateInfo					fenceCreateInfo			=
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		DE_NULL,
		0u,			// flags
	};

	const VkCommandBufferAllocateInfo		cmdBufCreateInfo		=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,				// sType
		DE_NULL,													// pNext
		*cmdPool,													// commandPool
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,							// level
		1u,															// bufferCount;
	};

	const VkCommandBufferBeginInfo			cmdBufBeginInfo			=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,				// sType
		DE_NULL,													// pNext
		0u,															// flags
		(const VkCommandBufferInheritanceInfo*)DE_NULL,
	};

	const Unique<VkFence>					cmdCompleteFence		(createFence(vk, device, &fenceCreateInfo));
	const Unique<VkCommandBuffer>			cmd						(allocateCommandBuffer(vk, device, &cmdBufCreateInfo));

	VK_CHECK(vk.beginCommandBuffer(*cmd, &cmdBufBeginInfo));

	vk.cmdBindPipeline(*cmd, VK_PIPELINE_BIND_POINT_COMPUTE, *pipelineBad);
	vk.cmdBindPipeline(*cmd, VK_PIPELINE_BIND_POINT_COMPUTE, *pipelineGood);
	vk.cmdBindDescriptorSets(*cmd, VK_PIPELINE_BIND_POINT_COMPUTE, *pipelineLayout, 0, numDescriptorSets, descriptorSets, numDynamicOffsets, dynamicOffsets);

	if (numPreBarriers)
		vk.cmdPipelineBarrier(*cmd, 0u, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, (VkDependencyFlags)0,
							  0, (const VkMemoryBarrier*)DE_NULL,
							  numPreBarriers, bufferBarriers,
							  0, (const VkImageMemoryBarrier*)DE_NULL);

	vk.cmdDispatch(*cmd, numWorkGroups.x(), numWorkGroups.y(), numWorkGroups.z());
	vk.cmdPipelineBarrier(*cmd, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, (VkDependencyFlags)0,
						  0, (const VkMemoryBarrier*)DE_NULL,
						  numPostBarriers, postBarriers,
						  0, (const VkImageMemoryBarrier*)DE_NULL);
	VK_CHECK(vk.endCommandBuffer(*cmd));

	// run
	// submit second primary buffer, the secondary should be executed too
	const VkSubmitInfo						submitInfo				=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,								// sType
		DE_NULL,													// pNext
		0u,															// waitSemaphoreCount
		DE_NULL,													// pWaitSemaphores
		(const VkPipelineStageFlags*)DE_NULL,						// pWaitDstStageMask
		1,															// commandBufferCount
		&cmd.get(),													// pCommandBuffers
		0u,															// signalSemaphoreCount
		DE_NULL,													// pSignalSemaphores
	};
	VK_CHECK(vk.queueSubmit(queue, 1u, &submitInfo, *cmdCompleteFence));

	VK_CHECK(vk.waitForFences(device, 1u, &cmdCompleteFence.get(), 0u, INFINITE_TIMEOUT)); // \note: timeout is failure
	VK_CHECK(vk.resetFences(device, 1u, &cmdCompleteFence.get()));

	// submit and wait end
	result.readResultContentsTo(&results);

	// verify
	if (results[0] == references[0] &&
		results[1] == references[1] &&
		results[2] == references[2] &&
		results[3] == references[3])
	{
		return tcu::TestStatus::pass("Pass");
	}
	else if (results[0] == tcu::Vec4(-1.0f) &&
			 results[1] == tcu::Vec4(-1.0f) &&
			 results[2] == tcu::Vec4(-1.0f) &&
			 results[3] == tcu::Vec4(-1.0f))
	{
		context.getTestContext().getLog()
		<< tcu::TestLog::Message
		<< "Result buffer was not written to."
		<< tcu::TestLog::EndMessage;
		return tcu::TestStatus::fail("Result buffer was not written to");
	}
	else
	{
		context.getTestContext().getLog()
		<< tcu::TestLog::Message
		<< "Error expected ["
		<< references[0] << ", "
		<< references[1] << ", "
		<< references[2] << ", "
		<< references[3] << "], got ["
		<< results[0] << ", "
		<< results[1] << ", "
		<< results[2] << ", "
		<< results[3] << "]"
		<< tcu::TestLog::EndMessage;
		return tcu::TestStatus::fail("Invalid result values");
	}
}

// Shaders
void genComputeSource (SourceCollections& programCollection)
{
	const char* const						versionDecl				= glu::getGLSLVersionDeclaration(glu::GLSL_VERSION_310_ES);
	std::ostringstream						buf_good;

	buf_good << versionDecl << "\n"
	<< ""
	<< "layout(local_size_x = 1u, local_size_y = 1u, local_size_z = 1) in;\n"
	<< "layout(set = 0, binding = 1u, std140) uniform BufferName\n"
	<< "{\n"
	<< "	highp vec4 colorA;\n"
	<< "	highp vec4 colorB;\n"
	<< "} b_instance;\n"
	<< "layout(set = 0, binding = 0, std140) writeonly buffer OutBuf\n"
	<< "{\n"
	<< "	highp vec4 read_colors[4];\n"
	<< "} b_out;\n"
	<< "void main(void)\n"
	<< "{\n"
	<< "	highp int quadrant_id = int(gl_WorkGroupID.x);\n"
	<< "	highp vec4 result_color;\n"
	<< "	if (quadrant_id == 1 || quadrant_id == 2)\n"
	<< "		result_color = b_instance.colorA;\n"
	<< "	else\n"
	<< "		result_color = b_instance.colorB;\n"
	<< "	b_out.read_colors[gl_WorkGroupID.x] = result_color;\n"
	<< "}\n";

	programCollection.glslSources.add("compute_good") << glu::ComputeSource(buf_good.str());

	std::ostringstream	buf_bad;

	buf_bad	<< versionDecl << "\n"
	<< ""
	<< "layout(local_size_x = 1u, local_size_y = 1u, local_size_z = 1) in;\n"
	<< "layout(set = 0, binding = 1u, std140) uniform BufferName\n"
	<< "{\n"
	<< "	highp vec4 colorA;\n"
	<< "	highp vec4 colorB;\n"
	<< "} b_instance;\n"
	<< "layout(set = 0, binding = 0, std140) writeonly buffer OutBuf\n"
	<< "{\n"
	<< "	highp vec4 read_colors[4];\n"
	<< "} b_out;\n"
	<< "void main(void)\n"
	<< "{\n"
	<< "	highp int quadrant_id = int(gl_WorkGroupID.x);\n"
	<< "	highp vec4 result_color;\n"
	<< "	if (quadrant_id == 1 || quadrant_id == 2)\n"
	<< "		result_color = b_instance.colorA;\n"
	<< "	else\n"
	<< "		result_color = b_instance.colorB;\n"
	<< "	b_out.read_colors[gl_WorkGroupID.x] = vec4(0.0, 0.0, 0.0, 0.0);\n"
	<< "}\n";

	programCollection.glslSources.add("compute_bad") << glu::ComputeSource(buf_bad.str());
}

} // anonymous

tcu::TestCaseGroup* createCommandBuffersTests (tcu::TestContext& testCtx)
{
	de::MovePtr<tcu::TestCaseGroup>	commandBuffersTests	(new tcu::TestCaseGroup(testCtx, "command_buffers", "Command Buffers Tests"));

	/* 19.1. Command Pools (6.1 in VK 1.0 Spec) */
	addFunctionCase				(commandBuffersTests.get(), "pool_create_null_params",			"",	createPoolNullParamsTest);
	addFunctionCase				(commandBuffersTests.get(), "pool_create_non_null_allocator",	"",	createPoolNonNullAllocatorTest);
	addFunctionCase				(commandBuffersTests.get(), "pool_create_transient_bit",		"",	createPoolTransientBitTest);
	addFunctionCase				(commandBuffersTests.get(), "pool_create_reset_bit",			"",	createPoolResetBitTest);
	addFunctionCase				(commandBuffersTests.get(), "pool_reset_release_res",			"",	resetPoolReleaseResourcesBitTest);
	addFunctionCase				(commandBuffersTests.get(), "pool_reset_no_flags_res",			"",	resetPoolNoFlagsTest);
	/* 19.2. Command Buffer Lifetime (6.2 in VK 1.0 Spec) */
	addFunctionCase				(commandBuffersTests.get(), "allocate_single_primary",			"", allocatePrimaryBufferTest);
	addFunctionCase				(commandBuffersTests.get(), "allocate_many_primary",			"",	allocateManyPrimaryBuffersTest);
	addFunctionCase				(commandBuffersTests.get(), "allocate_zero_primary",			"", allocateZeroPrimaryBuffersTest);
	addFunctionCase				(commandBuffersTests.get(), "allocate_single_secondary",		"", allocateSecondaryBufferTest);
	addFunctionCase				(commandBuffersTests.get(), "allocate_many_secondary",			"", allocateManySecondaryBuffersTest);
	addFunctionCase				(commandBuffersTests.get(), "allocate_zero_secondary",			"", allocateZeroSecondaryBuffersTest);
	addFunctionCase				(commandBuffersTests.get(), "execute_small_primary",			"",	executePrimaryBufferTest);
	addFunctionCase				(commandBuffersTests.get(), "execute_large_primary",			"",	executeLargePrimaryBufferTest);
	addFunctionCase				(commandBuffersTests.get(), "reset_implicit",					"", resetBufferImplicitlyTest);
	/* 19.3. Command Buffer Recording (6.3 in VK 1.0 Spec) */
	addFunctionCase				(commandBuffersTests.get(), "record_single_primary",			"",	recordSinglePrimaryBufferTest);
	addFunctionCase				(commandBuffersTests.get(), "record_many_primary",				"", recordLargePrimaryBufferTest);
	addFunctionCase				(commandBuffersTests.get(), "record_single_secondary",			"",	recordSingleSecondaryBufferTest);
	addFunctionCase				(commandBuffersTests.get(), "record_many_secondary",			"", recordLargeSecondaryBufferTest);
	addFunctionCase				(commandBuffersTests.get(), "submit_twice_primary",				"",	submitPrimaryBufferTwiceTest);
	addFunctionCase				(commandBuffersTests.get(), "submit_twice_secondary",			"",	submitSecondaryBufferTwiceTest);
	addFunctionCase				(commandBuffersTests.get(), "record_one_time_submit_primary",	"",	oneTimeSubmitFlagPrimaryBufferTest);
	addFunctionCase				(commandBuffersTests.get(), "record_one_time_submit_secondary",	"",	oneTimeSubmitFlagSecondaryBufferTest);
	addFunctionCase				(commandBuffersTests.get(), "record_simul_use_primary",			"",	simultaneousUsePrimaryBufferTest);
	addFunctionCase				(commandBuffersTests.get(), "record_simul_use_secondary",		"",	simultaneousUseSecondaryBufferTest);
	addFunctionCase				(commandBuffersTests.get(), "record_query_precise_w_flag",		"",	recordBufferQueryPreciseWithFlagTest);
	addFunctionCase				(commandBuffersTests.get(), "record_query_imprecise_w_flag",	"",	recordBufferQueryImpreciseWithFlagTest);
	addFunctionCase				(commandBuffersTests.get(), "record_query_imprecise_wo_flag",	"",	recordBufferQueryImpreciseWithoutFlagTest);
	/* 19.4. Command Buffer Submission (6.4 in VK 1.0 Spec) */
	addFunctionCase				(commandBuffersTests.get(), "submit_count_non_zero",			"", submitBufferCountNonZero);
	addFunctionCase				(commandBuffersTests.get(), "submit_count_equal_zero",			"", submitBufferCountEqualZero);
	addFunctionCase				(commandBuffersTests.get(), "submit_null_fence",				"", submitBufferNullFence);
	/* 19.5. Secondary Command Buffer Execution (6.6 in VK 1.0 Spec) */
	addFunctionCase				(commandBuffersTests.get(), "secondary_execute",				"",	executeSecondaryBufferTest);
	addFunctionCase				(commandBuffersTests.get(), "secondary_execute_twice",			"",	executeSecondaryBufferTwiceTest);
	/* 19.6. Commands Allowed Inside Command Buffers (6.7 in VK 1.0 Spec) */
	addFunctionCaseWithPrograms (commandBuffersTests.get(), "order_bind_pipeline",				"", genComputeSource, orderBindPipelineTest);

	return commandBuffersTests.release();
}

} // api
} // vkt

