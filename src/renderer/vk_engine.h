#pragma once

#include <vk_types.h>
#include <vk_descriptors.h>

#include <Swapchain.h>

#include <camera.h>
#include "vk_loader.h"


#ifdef NODEBUG
constexpr bool bUseValidationLayers = false;
#else
constexpr bool bUseValidationLayers = true;
#endif

constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanEngine {
public:
	bool _isInitialized{ false };
	int _frameNumber{ 0 };
	bool stop_rendering{ false };

	struct SDL_Window* _window{ nullptr };
	VkExtent2D _windowExtent{ 1700 , 900 };

	VkInstance _instance;
	VkDebugUtilsMessengerEXT _debugMessenger;
	VkPhysicalDevice _chosenGPU;
	VkDevice _device;
	VkSurfaceKHR _surface;

	// immediate submit structures
	VkFence _immFence;
	VkCommandBuffer _immCommandBuffer;
	VkCommandPool _immCommandPool;

	FrameData _frames[FRAME_OVERLAP];
	FrameData& getCurrentFrame() { return _frames[_frameNumber % FRAME_OVERLAP]; }

	VkQueue _graphicsQueue;
	uint32_t _graphicsQueueFamily;

	DeletionQueue _mainDeletionQueue;

	VmaAllocator _allocator;

	DescriptorAllocator globalDescriptorAllocator;

	VkDescriptorSet _drawImageDescriptors;
	VkDescriptorSetLayout _drawImageDescriptorLayout;

	std::vector<ComputeEffect> backgroundEffects;
	int currentBackgroundEffect = 0;

	GPUSceneData sceneData;
	std::vector<std::shared_ptr<MeshAsset>> testMeshes;
	Camera mainCamera;

	float renderScale = 1.f;
	int currentView = 0;

	static VulkanEngine& Get();

	void init();
	void cleanup();
	void draw();
	void run();

	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

	AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
	void destroy_buffer(const AllocatedBuffer& buffer);
	GPUMeshBuffers uploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices);

	void update_scene();

private:
	Swapchain* m_Swapchain = nullptr;
	bool resize_requested = false;

	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_sync_structures();
	void init_pipelines();
	void init_background_pipelines();
	void init_imgui();

	void init_descriptors();
	void update_descriptors();

	VkPipelineLayout _meshPipelineLayout;
	VkPipeline _meshPipeline;

	void init_mesh_pipeline();
	void init_default_data();

	void draw_background(VkCommandBuffer cmd);
	void draw_geometry(VkCommandBuffer cmd);
	void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);
};
