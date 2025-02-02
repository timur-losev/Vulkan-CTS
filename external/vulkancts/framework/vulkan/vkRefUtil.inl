/* WARNING: This is auto-generated file. Do not modify, since changes will
 * be lost! Modify the generating script instead.
 */
Move<VkInstance>			createInstance				(const PlatformInterface& vk, const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkDevice>				createDevice				(const InstanceInterface& vk, VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkDeviceMemory>		allocateMemory				(const DeviceInterface& vk, VkDevice device, const VkMemoryAllocateInfo* pAllocateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkFence>				createFence					(const DeviceInterface& vk, VkDevice device, const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkSemaphore>			createSemaphore				(const DeviceInterface& vk, VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkEvent>				createEvent					(const DeviceInterface& vk, VkDevice device, const VkEventCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkQueryPool>			createQueryPool				(const DeviceInterface& vk, VkDevice device, const VkQueryPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkBuffer>				createBuffer				(const DeviceInterface& vk, VkDevice device, const VkBufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkBufferView>			createBufferView			(const DeviceInterface& vk, VkDevice device, const VkBufferViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkImage>				createImage					(const DeviceInterface& vk, VkDevice device, const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkImageView>			createImageView				(const DeviceInterface& vk, VkDevice device, const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkShaderModule>		createShaderModule			(const DeviceInterface& vk, VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkPipelineCache>		createPipelineCache			(const DeviceInterface& vk, VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkPipeline>			createGraphicsPipelines		(const DeviceInterface& vk, VkDevice device, VkPipelineCache pipelineCache, deUint32 createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkPipeline>			createComputePipelines		(const DeviceInterface& vk, VkDevice device, VkPipelineCache pipelineCache, deUint32 createInfoCount, const VkComputePipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkPipelineLayout>		createPipelineLayout		(const DeviceInterface& vk, VkDevice device, const VkPipelineLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkSampler>				createSampler				(const DeviceInterface& vk, VkDevice device, const VkSamplerCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkDescriptorSetLayout>	createDescriptorSetLayout	(const DeviceInterface& vk, VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkDescriptorPool>		createDescriptorPool		(const DeviceInterface& vk, VkDevice device, const VkDescriptorPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkFramebuffer>			createFramebuffer			(const DeviceInterface& vk, VkDevice device, const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkRenderPass>			createRenderPass			(const DeviceInterface& vk, VkDevice device, const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
Move<VkCommandPool>			createCommandPool			(const DeviceInterface& vk, VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = DE_NULL);
