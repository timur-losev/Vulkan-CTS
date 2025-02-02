/*-------------------------------------------------------------------------
 * Vulkan Conformance Tests
 * ------------------------
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
 * \brief Device Initialization Tests
 *//*--------------------------------------------------------------------*/

#include "vktApiDeviceInitializationTests.hpp"
#include "vktTestCaseUtil.hpp"

#include "vkDefs.hpp"
#include "vkPlatform.hpp"
#include "vkStrUtil.hpp"
#include "vkRef.hpp"
#include "vkRefUtil.hpp"
#include "vkQueryUtil.hpp"
#include "vkMemUtil.hpp"
#include "vkDeviceUtil.hpp"
#include "vkApiVersion.hpp"

#include "tcuTestLog.hpp"
#include "tcuResultCollector.hpp"

#include "deUniquePtr.hpp"
#include "deStringUtil.hpp"

#include <vector>

namespace vkt
{
namespace api
{

namespace
{

using namespace vk;
using namespace std;
using std::vector;
using tcu::TestLog;

tcu::TestStatus createInstanceTest (Context& context)
{
	tcu::TestLog&				log						= context.getTestContext().getLog();
	tcu::ResultCollector		resultCollector			(log);
	const char*					appNames[]				= { "appName", DE_NULL, "",  "app, name", "app(\"name\"", "app~!@#$%^&*()_+name", "app\nName", "app\r\nName" };
	const char*					engineNames[]			= { "engineName", DE_NULL, "",  "engine. name", "engine\"(name)", "eng~!@#$%^&*()_+name", "engine\nName", "engine\r\nName" };
	const deUint32				appVersions[]			= { 0, 1, (deUint32)-1 };
	const deUint32				engineVersions[]		= { 0, 1, (deUint32)-1 };
	const PlatformInterface&	platformInterface		= context.getPlatformInterface();
	vector<VkApplicationInfo>	appInfos;

	// test over appName
	for (int appNameNdx = 0; appNameNdx < DE_LENGTH_OF_ARRAY(appNames); appNameNdx++)
	{
		const VkApplicationInfo appInfo =
		{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,		// VkStructureType				sType;
			DE_NULL,								// const void*					pNext;
			appNames[appNameNdx],					// const char*					pAppName;
			0u,										// deUint32						appVersion;
			"engineName",							// const char*					pEngineName;
			0u,										// deUint32						engineVersion;
			VK_API_VERSION,							// deUint32						apiVersion;
		};

		appInfos.push_back(appInfo);
	}

	// test over engineName
	for (int engineNameNdx = 0; engineNameNdx < DE_LENGTH_OF_ARRAY(engineNames); engineNameNdx++)
	{
		const VkApplicationInfo appInfo =
		{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,		// VkStructureType				sType;
			DE_NULL,								// const void*					pNext;
			"appName",								// const char*					pAppName;
			0u,										// deUint32						appVersion;
			engineNames[engineNameNdx],				// const char*					pEngineName;
			0u,										// deUint32						engineVersion;
			VK_API_VERSION,							// deUint32						apiVersion;
		};

		appInfos.push_back(appInfo);
	}

	// test over appVersion
	for (int appVersionNdx = 0; appVersionNdx < DE_LENGTH_OF_ARRAY(appVersions); appVersionNdx++)
	{
		const VkApplicationInfo appInfo =
		{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,		// VkStructureType				sType;
			DE_NULL,								// const void*					pNext;
			"appName",								// const char*					pAppName;
			appVersions[appVersionNdx],				// deUint32						appVersion;
			"engineName",							// const char*					pEngineName;
			0u,										// deUint32						engineVersion;
			VK_API_VERSION,							// deUint32						apiVersion;
		};

		appInfos.push_back(appInfo);
	}

	// test over engineVersion
	for (int engineVersionNdx = 0; engineVersionNdx < DE_LENGTH_OF_ARRAY(engineVersions); engineVersionNdx++)
	{
		const VkApplicationInfo appInfo =
		{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,		// VkStructureType				sType;
			DE_NULL,								// const void*					pNext;
			"appName",								// const char*					pAppName;
			0u,										// deUint32						appVersion;
			"engineName",							// const char*					pEngineName;
			engineVersions[engineVersionNdx],		// deUint32						engineVersion;
			VK_API_VERSION,							// deUint32						apiVersion;
		};

		appInfos.push_back(appInfo);
	}

	// run the tests!
	for (size_t appInfoNdx = 0; appInfoNdx < appInfos.size(); ++appInfoNdx)
	{
		const VkApplicationInfo&		appInfo					= appInfos[appInfoNdx];
		const VkInstanceCreateInfo		instanceCreateInfo		=
		{
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,	// VkStructureType				sType;
			DE_NULL,								// const void*					pNext;
			(VkInstanceCreateFlags)0u,				// VkInstanceCreateFlags		flags;
			&appInfo,								// const VkApplicationInfo*		pAppInfo;
			0u,										// deUint32						layerCount;
			DE_NULL,								// const char*const*			ppEnabledLayernames;
			0u,										// deUint32						extensionCount;
			DE_NULL,								// const char*const*			ppEnabledExtensionNames;
		};

		log << TestLog::Message << "Creating instance with appInfo: " << appInfo << TestLog::EndMessage;

		try
		{
			const Unique<VkInstance> instance(createInstance(platformInterface, &instanceCreateInfo));
			log << TestLog::Message << "Succeeded" << TestLog::EndMessage;
		}
		catch (const vk::Error& err)
		{
			resultCollector.fail("Failed, Error code: " + de::toString(err.getMessage()));
		}
	}

	return tcu::TestStatus(resultCollector.getResult(), resultCollector.getMessage());
}

tcu::TestStatus createInstanceWithInvalidApiVersionTest (Context& context)
{
	tcu::TestLog&				log					= context.getTestContext().getLog();
	tcu::ResultCollector		resultCollector		(log);
	const PlatformInterface&	platformInterface	= context.getPlatformInterface();
	const ApiVersion			apiVersion			= unpackVersion(VK_API_VERSION);
	const deUint32				invalidMajorVersion	= (1 << 10) - 1;
	const deUint32				invalidMinorVersion	= (1 << 10) - 1;
	const deUint32				invalidPatchNum		= (1 << 12) - 1;
	vector<ApiVersion>			invalidApiVersions;

	invalidApiVersions.push_back(ApiVersion(invalidMajorVersion, apiVersion.minorNum, apiVersion.patchNum));
	invalidApiVersions.push_back(ApiVersion(apiVersion.majorNum, invalidMinorVersion, apiVersion.patchNum));
	invalidApiVersions.push_back(ApiVersion(apiVersion.majorNum, apiVersion.minorNum, invalidPatchNum));

	for (size_t apiVersionNdx = 0; apiVersionNdx < invalidApiVersions.size(); apiVersionNdx++)
	{
		const VkApplicationInfo appInfo					=
		{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,			// VkStructureType				sType;
			DE_NULL,									// const void*					pNext;
			"appName",									// const char*					pAppName;
			0u,											// deUint32						appVersion;
			"engineName",								// const char*					pEngineName;
			0u,											// deUint32						engineVersion;
			pack(invalidApiVersions[apiVersionNdx]),	// deUint32						apiVersion;
		};
		const VkInstanceCreateInfo instanceCreateInfo	=
		{
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,		// VkStructureType				sType;
			DE_NULL,									// const void*					pNext;
			(VkInstanceCreateFlags)0u,					// VkInstanceCreateFlags		flags;
			&appInfo,									// const VkApplicationInfo*		pAppInfo;
			0u,											// deUint32						layerCount;
			DE_NULL,									// const char*const*			ppEnabledLayernames;
			0u,											// deUint32						extensionCount;
			DE_NULL,									// const char*const*			ppEnabledExtensionNames;
		};


		log << TestLog::Message
			<<"VK_API_VERSION defined in vulkan.h: " << apiVersion
			<< ", api version used to create instance: " << invalidApiVersions[apiVersionNdx]
			<< TestLog::EndMessage;

		{
			VkInstance		instance	= (VkInstance)0;
			const VkResult	result		= platformInterface.createInstance(&instanceCreateInfo, DE_NULL/*pAllocator*/, &instance);
			const bool		gotInstance	= !!instance;

			if (instance)
			{
				const InstanceDriver	instanceIface	(platformInterface, instance);
				instanceIface.destroyInstance(instance, DE_NULL/*pAllocator*/);
			}

			if (result == VK_ERROR_INCOMPATIBLE_DRIVER)
			{
				TCU_CHECK(!gotInstance);
				log << TestLog::Message << "Pass, instance creation with invalid apiVersion is rejected" << TestLog::EndMessage;
			}
			else
				resultCollector.fail("Fail, instance creation with invalid apiVersion is not rejected");
		}
	}

	return tcu::TestStatus(resultCollector.getResult(), resultCollector.getMessage());
}

tcu::TestStatus createInstanceWithUnsupportedExtensionsTest (Context& context)
{
	tcu::TestLog&						log						= context.getTestContext().getLog();
	const PlatformInterface&			platformInterface		= context.getPlatformInterface();
	const char*							enabledExtensions[]		= {"VK_UNSUPPORTED_EXTENSION", "THIS_IS_NOT_AN_EXTENSION"};
	const VkApplicationInfo				appInfo					=
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,						// VkStructureType				sType;
		DE_NULL,												// const void*					pNext;
		"appName",												// const char*					pAppName;
		0u,														// deUint32						appVersion;
		"engineName",											// const char*					pEngineName;
		0u,														// deUint32						engineVersion;
		VK_API_VERSION,											// deUint32						apiVersion;
	};
	const VkInstanceCreateInfo			instanceCreateInfo		=
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,					// VkStructureType				sType;
		DE_NULL,												// const void*					pNext;
		(VkInstanceCreateFlags)0u,								// VkInstanceCreateFlags		flags;
		&appInfo,												// const VkApplicationInfo*		pAppInfo;
		0u,														// deUint32						layerCount;
		DE_NULL,												// const char*const*			ppEnabledLayernames;
		DE_LENGTH_OF_ARRAY(enabledExtensions),					// deUint32						extensionCount;
		enabledExtensions,										// const char*const*			ppEnabledExtensionNames;
	};

	log << TestLog::Message << "Enabled extensions are: " << TestLog::EndMessage;

	for (int ndx = 0; ndx < DE_LENGTH_OF_ARRAY(enabledExtensions); ndx++)
		log << TestLog::Message << enabledExtensions[ndx] <<  TestLog::EndMessage;

	{
		VkInstance		instance	= (VkInstance)0;
		const VkResult	result		= platformInterface.createInstance(&instanceCreateInfo, DE_NULL/*pAllocator*/, &instance);
		const bool		gotInstance	= !!instance;

		if (instance)
		{
			const InstanceDriver	instanceIface	(platformInterface, instance);
			instanceIface.destroyInstance(instance, DE_NULL/*pAllocator*/);
		}

		if (result == VK_ERROR_EXTENSION_NOT_PRESENT)
		{
			TCU_CHECK(!gotInstance);
			return tcu::TestStatus::pass("Pass, creating instance with unsupported extension was rejected.");
		}
		else
			return tcu::TestStatus::fail("Fail, creating instance with unsupported extensions succeeded.");
	}
}

tcu::TestStatus createDeviceTest (Context& context)
{
	const PlatformInterface&		platformInterface		= context.getPlatformInterface();
	const Unique<VkInstance>		instance				(createDefaultInstance(platformInterface));
	const InstanceDriver			instanceDriver			(platformInterface, instance.get());
	const VkPhysicalDevice			physicalDevice			= chooseDevice(instanceDriver, instance.get(), context.getTestContext().getCommandLine());
	const deUint32					queueFamilyIndex		= 0;
	const deUint32					queueCount				= 1;
	const deUint32					queueIndex				= 0;
	const float						queuePriority			= 1.0f;
	const VkDeviceQueueCreateInfo	deviceQueueCreateInfo	=
	{
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		DE_NULL,
		(VkDeviceQueueCreateFlags)0u,
		queueFamilyIndex,						//queueFamilyIndex;
		queueCount,								//queueCount;
		&queuePriority,							//pQueuePriorities;
	};
	const VkDeviceCreateInfo		deviceCreateInfo	=
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,	//sType;
		DE_NULL,								//pNext;
		(VkDeviceCreateFlags)0u,
		1,										//queueRecordCount;
		&deviceQueueCreateInfo,					//pRequestedQueues;
		0,										//layerCount;
		DE_NULL,								//ppEnabledLayerNames;
		0,										//extensionCount;
		DE_NULL,								//ppEnabledExtensionNames;
		DE_NULL,								//pEnabledFeatures;
	};

	const Unique<VkDevice>			device					(createDevice(instanceDriver, physicalDevice, &deviceCreateInfo));
	const DeviceDriver				deviceDriver			(instanceDriver, device.get());
	VkQueue							queue;

	deviceDriver.getDeviceQueue(device.get(), queueFamilyIndex, queueIndex, &queue);
	VK_CHECK(deviceDriver.queueWaitIdle(queue));

	return tcu::TestStatus::pass("Pass");
}

tcu::TestStatus createMultipleDevicesTest (Context& context)
{
	tcu::TestLog&										log						= context.getTestContext().getLog();
	tcu::ResultCollector								resultCollector			(log);
	const int											numDevices				= 5;
	const PlatformInterface&							platformInterface		= context.getPlatformInterface();
	const Unique<VkInstance>							instance				(createDefaultInstance(platformInterface));
	const InstanceDriver								instanceDriver			(platformInterface, instance.get());
	const VkPhysicalDevice								physicalDevice			= chooseDevice(instanceDriver, instance.get(), context.getTestContext().getCommandLine());
	const deUint32										queueFamilyIndex		= 0;
	const deUint32										queueCount				= 1;
	const deUint32										queueIndex				= 0;
	const float											queuePriority			= 1.0f;
	const VkDeviceQueueCreateInfo						deviceQueueCreateInfo	=
	{
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		DE_NULL,
		(VkDeviceQueueCreateFlags)0u,					//flags;
		queueFamilyIndex,								//queueFamilyIndex;
		queueCount,										//queueCount;
		&queuePriority,									//pQueuePriorities;
	};
	const VkDeviceCreateInfo							deviceCreateInfo		=
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,			//sType;
		DE_NULL,										//pNext;
		(VkDeviceCreateFlags)0u,
		1,												//queueRecordCount;
		&deviceQueueCreateInfo,							//pRequestedQueues;
		0,												//layerCount;
		DE_NULL,										//ppEnabledLayerNames;
		0,												//extensionCount;
		DE_NULL,										//ppEnabledExtensionNames;
		DE_NULL,										//pEnabledFeatures;
	};
	vector<VkDevice>									devices(numDevices, (VkDevice)DE_NULL);

	try
	{
		for (int deviceNdx = 0; deviceNdx < numDevices; deviceNdx++)
		{
			const VkResult result = instanceDriver.createDevice(physicalDevice, &deviceCreateInfo, DE_NULL/*pAllocator*/, &devices[deviceNdx]);

			if (result != VK_SUCCESS)
			{
				resultCollector.fail("Failed to create Device No." + de::toString(deviceNdx) + ", Error Code: " + de::toString(result));
				break;
			}

			{
				const DeviceDriver	deviceDriver(instanceDriver, devices[deviceNdx]);
				VkQueue				queue;

				DE_ASSERT(queueIndex < queueCount);
				deviceDriver.getDeviceQueue(devices[deviceNdx], queueFamilyIndex, queueIndex, &queue);
				VK_CHECK(deviceDriver.queueWaitIdle(queue));
			}
		}
	}
	catch (const vk::Error& error)
	{
		resultCollector.fail(de::toString(error.getError()));
	}
	catch (...)
	{
		for (int deviceNdx = (int)devices.size()-1; deviceNdx >= 0; deviceNdx--)
		{
			if (devices[deviceNdx] != (VkDevice)DE_NULL)
			{
				DeviceDriver deviceDriver(instanceDriver, devices[deviceNdx]);
				deviceDriver.destroyDevice(devices[deviceNdx], DE_NULL/*pAllocator*/);
			}
		}

		throw;
	}

	for (int deviceNdx = (int)devices.size()-1; deviceNdx >= 0; deviceNdx--)
	{
		if (devices[deviceNdx] != (VkDevice)DE_NULL)
		{
			DeviceDriver deviceDriver(instanceDriver, devices[deviceNdx]);
			deviceDriver.destroyDevice(devices[deviceNdx], DE_NULL/*pAllocator*/);
		}
	}

	return tcu::TestStatus(resultCollector.getResult(), resultCollector.getMessage());
}

tcu::TestStatus createDeviceWithUnsupportedExtensionsTest (Context& context)
{
	tcu::TestLog&					log						= context.getTestContext().getLog();
	const PlatformInterface&		platformInterface		= context.getPlatformInterface();
	const Unique<VkInstance>		instance				(createDefaultInstance(platformInterface));
	const InstanceDriver			instanceDriver			(platformInterface, instance.get());
	const char*						enabledExtensions[]		= {"VK_UNSUPPORTED_EXTENSION", "THIS_IS_NOT_AN_EXTENSION", "VK_DONT_SUPPORT_ME"};
	const VkPhysicalDevice			physicalDevice			= chooseDevice(instanceDriver, instance.get(), context.getTestContext().getCommandLine());
	const float						queuePriority			= 1.0f;
	const VkDeviceQueueCreateInfo	deviceQueueCreateInfo	=
	{
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		DE_NULL,
		(VkDeviceQueueCreateFlags)0u,
		0,										//queueFamiliIndex;
		1,										//queueCount;
		&queuePriority,							//pQueuePriorities;
	};
	const VkDeviceCreateInfo		deviceCreateInfo		=
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,	//sType;
		DE_NULL,								//pNext;
		(VkDeviceCreateFlags)0u,
		1,										//queueRecordCount;
		&deviceQueueCreateInfo,					//pRequestedQueues;
		0,										//layerCount;
		DE_NULL,								//ppEnabledLayerNames;
		DE_LENGTH_OF_ARRAY(enabledExtensions),	//extensionCount;
		enabledExtensions,						//ppEnabledExtensionNames;
		DE_NULL,								//pEnabledFeatures;
	};

	log << TestLog::Message << "Enabled extensions are: " << TestLog::EndMessage;

	for (int ndx = 0; ndx < DE_LENGTH_OF_ARRAY(enabledExtensions); ndx++)
		log << TestLog::Message << enabledExtensions[ndx] <<  TestLog::EndMessage;

	{
		VkDevice		device		= (VkDevice)0;
		const VkResult	result		= instanceDriver.createDevice(physicalDevice, &deviceCreateInfo, DE_NULL/*pAllocator*/, &device);
		const bool		gotDevice	= !!device;

		if (device)
		{
			const DeviceDriver	deviceIface	(instanceDriver, device);
			deviceIface.destroyDevice(device, DE_NULL/*pAllocator*/);
		}

		if (result == VK_ERROR_EXTENSION_NOT_PRESENT)
		{
			TCU_CHECK(!gotDevice);
			return tcu::TestStatus::pass("Pass, create device with unsupported extension is rejected.");
		}
		else
			return tcu::TestStatus::fail("Fail, create device with unsupported extension but succeed.");
	}
}

deUint32 getGlobalMaxQueueCount(const vector<VkQueueFamilyProperties>& queueFamilyProperties)
{
	deUint32 maxQueueCount = 0;

	for (deUint32 queueFamilyNdx = 0; queueFamilyNdx < (deUint32)queueFamilyProperties.size(); queueFamilyNdx++)
	{
		maxQueueCount = de::max(maxQueueCount, queueFamilyProperties[queueFamilyNdx].queueCount);
	}

	return maxQueueCount;
}

tcu::TestStatus createDeviceWithVariousQueueCountsTest (Context& context)
{
	tcu::TestLog&							log						= context.getTestContext().getLog();
	const int								queueCountDiff			= 1;
	const PlatformInterface&				platformInterface		= context.getPlatformInterface();
	const Unique<VkInstance>				instance				(createDefaultInstance(platformInterface));
	const InstanceDriver					instanceDriver			(platformInterface, instance.get());
	const VkPhysicalDevice					physicalDevice			= chooseDevice(instanceDriver, instance.get(), context.getTestContext().getCommandLine());
	const vector<VkQueueFamilyProperties>	queueFamilyProperties	= getPhysicalDeviceQueueFamilyProperties(instanceDriver, physicalDevice);
	const vector<float>						queuePriorities			(getGlobalMaxQueueCount(queueFamilyProperties), 1.0f);
	vector<VkDeviceQueueCreateInfo>			deviceQueueCreateInfos;

	for (deUint32 queueFamilyNdx = 0; queueFamilyNdx < (deUint32)queueFamilyProperties.size(); queueFamilyNdx++)
	{
		const deUint32 maxQueueCount = queueFamilyProperties[queueFamilyNdx].queueCount;

		for (deUint32 queueCount = 1; queueCount <= maxQueueCount; queueCount += queueCountDiff)
		{
			const VkDeviceQueueCreateInfo queueCreateInfo =
			{
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				DE_NULL,
				(VkDeviceQueueCreateFlags)0u,
				queueFamilyNdx,
				queueCount,
				queuePriorities.data()
			};

			deviceQueueCreateInfos.push_back(queueCreateInfo);
		}
	}

	for (size_t testNdx = 0; testNdx < deviceQueueCreateInfos.size(); testNdx++)
	{
		const VkDeviceQueueCreateInfo&	queueCreateInfo		= deviceQueueCreateInfos[testNdx];
		const VkDeviceCreateInfo		deviceCreateInfo	=
		{
			VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,	//sType;
			DE_NULL,								//pNext;
			(VkDeviceCreateFlags)0u,
			1,										//queueRecordCount;
			&queueCreateInfo,						//pRequestedQueues;
			0,										//layerCount;
			DE_NULL,								//ppEnabledLayerNames;
			0,										//extensionCount;
			DE_NULL,								//ppEnabledExtensionNames;
			DE_NULL,								//pEnabledFeatures;
		};
		const Unique<VkDevice>			device				(createDevice(instanceDriver, physicalDevice, &deviceCreateInfo));
		const DeviceDriver				deviceDriver		(instanceDriver, device.get());
		const deUint32					queueFamilyIndex	= deviceCreateInfo.pQueueCreateInfos->queueFamilyIndex;
		const deUint32					queueCount			= deviceCreateInfo.pQueueCreateInfos->queueCount;

		for (deUint32 queueIndex = 0; queueIndex < queueCount; queueIndex++)
		{
			VkQueue		queue;
			VkResult	result;

			deviceDriver.getDeviceQueue(device.get(), queueFamilyIndex, queueIndex, &queue);
			TCU_CHECK(!!queue);

			result = deviceDriver.queueWaitIdle(queue);
			if (result != VK_SUCCESS)
			{
				log << TestLog::Message
					<< "vkQueueWaitIdle failed"
					<< ",  queueIndex = " << queueIndex
					<< ", queueCreateInfo " << queueCreateInfo
					<< ", Error Code: " << result
					<< TestLog::EndMessage;
				return tcu::TestStatus::fail("Fail");
			}
		}
	}
	return tcu::TestStatus::pass("Pass");
}

} // anonymous

tcu::TestCaseGroup* createDeviceInitializationTests (tcu::TestContext& testCtx)
{
	de::MovePtr<tcu::TestCaseGroup>	deviceInitializationTests (new tcu::TestCaseGroup(testCtx, "device_init", "Device Initialization Tests"));

	addFunctionCase(deviceInitializationTests.get(), "create_instance_name_version",			"", createInstanceTest);
	addFunctionCase(deviceInitializationTests.get(), "create_instance_invalid_api_version",		"", createInstanceWithInvalidApiVersionTest);
	addFunctionCase(deviceInitializationTests.get(), "create_instance_unsupported_extensions",	"", createInstanceWithUnsupportedExtensionsTest);
	addFunctionCase(deviceInitializationTests.get(), "create_device",							"", createDeviceTest);
	addFunctionCase(deviceInitializationTests.get(), "create_multiple_devices",					"", createMultipleDevicesTest);
	addFunctionCase(deviceInitializationTests.get(), "create_device_unsupported_extensions",	"", createDeviceWithUnsupportedExtensionsTest);
	addFunctionCase(deviceInitializationTests.get(), "create_device_various_queue_counts",		"", createDeviceWithVariousQueueCountsTest);

	return deviceInitializationTests.release();
}

} // api
} // vkt
