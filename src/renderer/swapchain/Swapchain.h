#include "vk_types.h"
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <VkBootstrap.h>
#include <vk_initializers.h>

class Swapchain {
public:
    Swapchain(VmaAllocator allocator, VkDevice device, SDL_Window* window, VkExtent2D& windowExtent);
    ~Swapchain();

    void Create(uint32_t width, uint32_t height, VkPhysicalDevice chosenGPU, VkSurfaceKHR surface);
    void Destroy();
    void Resize(VkPhysicalDevice chosenGPU, VkSurfaceKHR surface);

    VkFormat Format;
    VkSwapchainKHR SwapchainKHR;
    std::vector<VkImage> Images;
    std::vector<VkImageView> ImageViews;
    VkExtent2D Extent;

    AllocatedImage _drawImage;
    AllocatedImage _depthImage;
    VkExtent2D _drawExtent;

private:
    VkDevice m_Device;
    VmaAllocator m_Allocator;
    SDL_Window* m_Window;
    VkExtent2D& m_WindowExtent;

    void CreateDrawImage(uint32_t width, uint32_t height);
};