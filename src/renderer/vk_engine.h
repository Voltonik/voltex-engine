#pragma once

#include <vk_types.h>
#include <vk_descriptors.h>
#include <camera.h>


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

	VkSwapchainKHR _swapchain;
	VkFormat _swapchainImageFormat;

	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;
	VkExtent2D _swapchainExtent;

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

	AllocatedImage _drawImage;
	VkExtent2D _drawExtent;

	DescriptorAllocator globalDescriptorAllocator;

	VkDescriptorSet _drawImageDescriptors;
	VkDescriptorSetLayout _drawImageDescriptorLayout;

	std::vector<ComputeEffect> backgroundEffects;
	int currentBackgroundEffect = 0;

	GPUSceneData sceneData;
	Camera mainCamera;

	float renderScale = 1.f;

	int currentView = 0;

	static VulkanEngine& Get();

	void init();
	void cleanup();
	void draw();
	void run();

	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

	void update_scene();

private:
	bool resize_requested = false;

	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_sync_structures();
	void init_pipelines();
	void init_background_pipelines();
	void init_imgui();

	void create_swapchain(uint32_t width, uint32_t height);
	void resize_swapchain();
	void destroy_swapchain();

	void init_descriptors();

	VkPipelineLayout _trianglePipelineLayout;
	VkPipeline _trianglePipeline;

	void init_triangle_pipeline();

	void draw_background(VkCommandBuffer cmd);
	void draw_geometry(VkCommandBuffer cmd);
	void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);
};
