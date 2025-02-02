/* WARNING: This is auto-generated file. Do not modify, since changes will
 * be lost! Modify the generating script instead.
 */
virtual void		destroyDevice									(VkDevice device, const VkAllocationCallbacks* pAllocator) const;
virtual void		getDeviceQueue									(VkDevice device, deUint32 queueFamilyIndex, deUint32 queueIndex, VkQueue* pQueue) const;
virtual VkResult	queueSubmit										(VkQueue queue, deUint32 submitCount, const VkSubmitInfo* pSubmits, VkFence fence) const;
virtual VkResult	queueWaitIdle									(VkQueue queue) const;
virtual VkResult	deviceWaitIdle									(VkDevice device) const;
virtual VkResult	allocateMemory									(VkDevice device, const VkMemoryAllocateInfo* pAllocateInfo, const VkAllocationCallbacks* pAllocator, VkDeviceMemory* pMemory) const;
virtual void		freeMemory										(VkDevice device, VkDeviceMemory memory, const VkAllocationCallbacks* pAllocator) const;
virtual VkResult	mapMemory										(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData) const;
virtual void		unmapMemory										(VkDevice device, VkDeviceMemory memory) const;
virtual VkResult	flushMappedMemoryRanges							(VkDevice device, deUint32 memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges) const;
virtual VkResult	invalidateMappedMemoryRanges					(VkDevice device, deUint32 memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges) const;
virtual void		getDeviceMemoryCommitment						(VkDevice device, VkDeviceMemory memory, VkDeviceSize* pCommittedMemoryInBytes) const;
virtual VkResult	bindBufferMemory								(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset) const;
virtual VkResult	bindImageMemory									(VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset) const;
virtual void		getBufferMemoryRequirements						(VkDevice device, VkBuffer buffer, VkMemoryRequirements* pMemoryRequirements) const;
virtual void		getImageMemoryRequirements						(VkDevice device, VkImage image, VkMemoryRequirements* pMemoryRequirements) const;
virtual void		getImageSparseMemoryRequirements				(VkDevice device, VkImage image, deUint32* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements* pSparseMemoryRequirements) const;
virtual void		getPhysicalDeviceSparseImageFormatProperties	(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling, deUint32* pPropertyCount, VkSparseImageFormatProperties* pProperties) const;
virtual VkResult	queueBindSparse									(VkQueue queue, deUint32 bindInfoCount, const VkBindSparseInfo* pBindInfo, VkFence fence) const;
virtual VkResult	createFence										(VkDevice device, const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence) const;
virtual void		destroyFence									(VkDevice device, VkFence fence, const VkAllocationCallbacks* pAllocator) const;
virtual VkResult	resetFences										(VkDevice device, deUint32 fenceCount, const VkFence* pFences) const;
virtual VkResult	getFenceStatus									(VkDevice device, VkFence fence) const;
virtual VkResult	waitForFences									(VkDevice device, deUint32 fenceCount, const VkFence* pFences, VkBool32 waitAll, deUint64 timeout) const;
virtual VkResult	createSemaphore									(VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore) const;
virtual void		destroySemaphore								(VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks* pAllocator) const;
virtual VkResult	createEvent										(VkDevice device, const VkEventCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkEvent* pEvent) const;
virtual void		destroyEvent									(VkDevice device, VkEvent event, const VkAllocationCallbacks* pAllocator) const;
virtual VkResult	getEventStatus									(VkDevice device, VkEvent event) const;
virtual VkResult	setEvent										(VkDevice device, VkEvent event) const;
virtual VkResult	resetEvent										(VkDevice device, VkEvent event) const;
virtual VkResult	createQueryPool									(VkDevice device, const VkQueryPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkQueryPool* pQueryPool) const;
virtual void		destroyQueryPool								(VkDevice device, VkQueryPool queryPool, const VkAllocationCallbacks* pAllocator) const;
virtual VkResult	getQueryPoolResults								(VkDevice device, VkQueryPool queryPool, deUint32 firstQuery, deUint32 queryCount, deUintptr dataSize, void* pData, VkDeviceSize stride, VkQueryResultFlags flags) const;
virtual VkResult	createBuffer									(VkDevice device, const VkBufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer) const;
virtual void		destroyBuffer									(VkDevice device, VkBuffer buffer, const VkAllocationCallbacks* pAllocator) const;
virtual VkResult	createBufferView								(VkDevice device, const VkBufferViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBufferView* pView) const;
virtual void		destroyBufferView								(VkDevice device, VkBufferView bufferView, const VkAllocationCallbacks* pAllocator) const;
virtual VkResult	createImage										(VkDevice device, const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImage* pImage) const;
virtual void		destroyImage									(VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator) const;
virtual void		getImageSubresourceLayout						(VkDevice device, VkImage image, const VkImageSubresource* pSubresource, VkSubresourceLayout* pLayout) const;
virtual VkResult	createImageView									(VkDevice device, const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImageView* pView) const;
virtual void		destroyImageView								(VkDevice device, VkImageView imageView, const VkAllocationCallbacks* pAllocator) const;
virtual VkResult	createShaderModule								(VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkShaderModule* pShaderModule) const;
virtual void		destroyShaderModule								(VkDevice device, VkShaderModule shaderModule, const VkAllocationCallbacks* pAllocator) const;
virtual VkResult	createPipelineCache								(VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineCache* pPipelineCache) const;
virtual void		destroyPipelineCache							(VkDevice device, VkPipelineCache pipelineCache, const VkAllocationCallbacks* pAllocator) const;
virtual VkResult	getPipelineCacheData							(VkDevice device, VkPipelineCache pipelineCache, deUintptr* pDataSize, void* pData) const;
virtual VkResult	mergePipelineCaches								(VkDevice device, VkPipelineCache dstCache, deUint32 srcCacheCount, const VkPipelineCache* pSrcCaches) const;
virtual VkResult	createGraphicsPipelines							(VkDevice device, VkPipelineCache pipelineCache, deUint32 createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) const;
virtual VkResult	createComputePipelines							(VkDevice device, VkPipelineCache pipelineCache, deUint32 createInfoCount, const VkComputePipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) const;
virtual void		destroyPipeline									(VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks* pAllocator) const;
virtual VkResult	createPipelineLayout							(VkDevice device, const VkPipelineLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineLayout* pPipelineLayout) const;
virtual void		destroyPipelineLayout							(VkDevice device, VkPipelineLayout pipelineLayout, const VkAllocationCallbacks* pAllocator) const;
virtual VkResult	createSampler									(VkDevice device, const VkSamplerCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSampler* pSampler) const;
virtual void		destroySampler									(VkDevice device, VkSampler sampler, const VkAllocationCallbacks* pAllocator) const;
virtual VkResult	createDescriptorSetLayout						(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorSetLayout* pSetLayout) const;
virtual void		destroyDescriptorSetLayout						(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks* pAllocator) const;
virtual VkResult	createDescriptorPool							(VkDevice device, const VkDescriptorPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorPool* pDescriptorPool) const;
virtual void		destroyDescriptorPool							(VkDevice device, VkDescriptorPool descriptorPool, const VkAllocationCallbacks* pAllocator) const;
virtual VkResult	resetDescriptorPool								(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags) const;
virtual VkResult	allocateDescriptorSets							(VkDevice device, const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets) const;
virtual VkResult	freeDescriptorSets								(VkDevice device, VkDescriptorPool descriptorPool, deUint32 descriptorSetCount, const VkDescriptorSet* pDescriptorSets) const;
virtual void		updateDescriptorSets							(VkDevice device, deUint32 descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites, deUint32 descriptorCopyCount, const VkCopyDescriptorSet* pDescriptorCopies) const;
virtual VkResult	createFramebuffer								(VkDevice device, const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFramebuffer* pFramebuffer) const;
virtual void		destroyFramebuffer								(VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* pAllocator) const;
virtual VkResult	createRenderPass								(VkDevice device, const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass) const;
virtual void		destroyRenderPass								(VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks* pAllocator) const;
virtual void		getRenderAreaGranularity						(VkDevice device, VkRenderPass renderPass, VkExtent2D* pGranularity) const;
virtual VkResult	createCommandPool								(VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool) const;
virtual void		destroyCommandPool								(VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks* pAllocator) const;
virtual VkResult	resetCommandPool								(VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags) const;
virtual VkResult	allocateCommandBuffers							(VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers) const;
virtual void		freeCommandBuffers								(VkDevice device, VkCommandPool commandPool, deUint32 commandBufferCount, const VkCommandBuffer* pCommandBuffers) const;
virtual VkResult	beginCommandBuffer								(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo) const;
virtual VkResult	endCommandBuffer								(VkCommandBuffer commandBuffer) const;
virtual VkResult	resetCommandBuffer								(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags) const;
virtual void		cmdBindPipeline									(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) const;
virtual void		cmdSetViewport									(VkCommandBuffer commandBuffer, deUint32 firstViewport, deUint32 viewportCount, const VkViewport* pViewports) const;
virtual void		cmdSetScissor									(VkCommandBuffer commandBuffer, deUint32 firstScissor, deUint32 scissorCount, const VkRect2D* pScissors) const;
virtual void		cmdSetLineWidth									(VkCommandBuffer commandBuffer, float lineWidth) const;
virtual void		cmdSetDepthBias									(VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) const;
virtual void		cmdSetBlendConstants							(VkCommandBuffer commandBuffer, const float blendConstants[4]) const;
virtual void		cmdSetDepthBounds								(VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds) const;
virtual void		cmdSetStencilCompareMask						(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, deUint32 compareMask) const;
virtual void		cmdSetStencilWriteMask							(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, deUint32 writeMask) const;
virtual void		cmdSetStencilReference							(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, deUint32 reference) const;
virtual void		cmdBindDescriptorSets							(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, deUint32 firstSet, deUint32 descriptorSetCount, const VkDescriptorSet* pDescriptorSets, deUint32 dynamicOffsetCount, const deUint32* pDynamicOffsets) const;
virtual void		cmdBindIndexBuffer								(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) const;
virtual void		cmdBindVertexBuffers							(VkCommandBuffer commandBuffer, deUint32 firstBinding, deUint32 bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets) const;
virtual void		cmdDraw											(VkCommandBuffer commandBuffer, deUint32 vertexCount, deUint32 instanceCount, deUint32 firstVertex, deUint32 firstInstance) const;
virtual void		cmdDrawIndexed									(VkCommandBuffer commandBuffer, deUint32 indexCount, deUint32 instanceCount, deUint32 firstIndex, deInt32 vertexOffset, deUint32 firstInstance) const;
virtual void		cmdDrawIndirect									(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, deUint32 drawCount, deUint32 stride) const;
virtual void		cmdDrawIndexedIndirect							(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, deUint32 drawCount, deUint32 stride) const;
virtual void		cmdDispatch										(VkCommandBuffer commandBuffer, deUint32 x, deUint32 y, deUint32 z) const;
virtual void		cmdDispatchIndirect								(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset) const;
virtual void		cmdCopyBuffer									(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, deUint32 regionCount, const VkBufferCopy* pRegions) const;
virtual void		cmdCopyImage									(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, deUint32 regionCount, const VkImageCopy* pRegions) const;
virtual void		cmdBlitImage									(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, deUint32 regionCount, const VkImageBlit* pRegions, VkFilter filter) const;
virtual void		cmdCopyBufferToImage							(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, deUint32 regionCount, const VkBufferImageCopy* pRegions) const;
virtual void		cmdCopyImageToBuffer							(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, deUint32 regionCount, const VkBufferImageCopy* pRegions) const;
virtual void		cmdUpdateBuffer									(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const deUint32* pData) const;
virtual void		cmdFillBuffer									(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, deUint32 data) const;
virtual void		cmdClearColorImage								(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, deUint32 rangeCount, const VkImageSubresourceRange* pRanges) const;
virtual void		cmdClearDepthStencilImage						(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, deUint32 rangeCount, const VkImageSubresourceRange* pRanges) const;
virtual void		cmdClearAttachments								(VkCommandBuffer commandBuffer, deUint32 attachmentCount, const VkClearAttachment* pAttachments, deUint32 rectCount, const VkClearRect* pRects) const;
virtual void		cmdResolveImage									(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, deUint32 regionCount, const VkImageResolve* pRegions) const;
virtual void		cmdSetEvent										(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask) const;
virtual void		cmdResetEvent									(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask) const;
virtual void		cmdWaitEvents									(VkCommandBuffer commandBuffer, deUint32 eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, deUint32 memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, deUint32 bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, deUint32 imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) const;
virtual void		cmdPipelineBarrier								(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, deUint32 memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, deUint32 bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, deUint32 imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) const;
virtual void		cmdBeginQuery									(VkCommandBuffer commandBuffer, VkQueryPool queryPool, deUint32 query, VkQueryControlFlags flags) const;
virtual void		cmdEndQuery										(VkCommandBuffer commandBuffer, VkQueryPool queryPool, deUint32 query) const;
virtual void		cmdResetQueryPool								(VkCommandBuffer commandBuffer, VkQueryPool queryPool, deUint32 firstQuery, deUint32 queryCount) const;
virtual void		cmdWriteTimestamp								(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, deUint32 query) const;
virtual void		cmdCopyQueryPoolResults							(VkCommandBuffer commandBuffer, VkQueryPool queryPool, deUint32 firstQuery, deUint32 queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags) const;
virtual void		cmdPushConstants								(VkCommandBuffer commandBuffer, VkPipelineLayout layout, VkShaderStageFlags stageFlags, deUint32 offset, deUint32 size, const void* pValues) const;
virtual void		cmdBeginRenderPass								(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents) const;
virtual void		cmdNextSubpass									(VkCommandBuffer commandBuffer, VkSubpassContents contents) const;
virtual void		cmdEndRenderPass								(VkCommandBuffer commandBuffer) const;
virtual void		cmdExecuteCommands								(VkCommandBuffer commandBuffer, deUint32 commandBufferCount, const VkCommandBuffer* pCommandBuffers) const;
