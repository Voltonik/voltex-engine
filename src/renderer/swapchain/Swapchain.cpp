#include "Swapchain.h"

Swapchain::Swapchain(VmaAllocator allocator, VkDevice device, SDL_Window* window, VkExtent2D& windowExtent)
    : m_Allocator(allocator), m_Device(device), m_Window(window), m_WindowExtent(windowExtent) {
}

Swapchain::~Swapchain() {
    Destroy();
}

void Swapchain::Create(uint32_t width, uint32_t height, VkPhysicalDevice chosenGPU, VkSurfaceKHR surface) {
    vkb::SwapchainBuilder builder{ chosenGPU, m_Device, surface };

    Format = VK_FORMAT_B8G8R8A8_UNORM;

    vkb::Swapchain vkbSwapchain = builder
        .set_desired_format(VkSurfaceFormatKHR{ .format = Format, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        .set_desired_extent(width, height)
        .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        .build()
        .value();


    Extent = vkbSwapchain.extent;
    SwapchainKHR = vkbSwapchain.swapchain;
    Images = vkbSwapchain.get_images().value();
    ImageViews = vkbSwapchain.get_image_views().value();


    CreateDrawImage(width, height);
}

void Swapchain::CreateDrawImage(uint32_t width, uint32_t height) {
    // Draw image size will match the window
    VkExtent3D drawImageExtent = { width, height, 1 };

    // Hardcoding the draw format to 32-bit float
    _drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    _drawImage.imageExtent = drawImageExtent;

    VkImageUsageFlags drawImageUsages{};
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkImageCreateInfo rimg_info = vkinit::image_create_info(_drawImage.imageFormat, drawImageUsages, drawImageExtent);

    // For the draw image, we want to allocate it from GPU local memory
    VmaAllocationCreateInfo rimg_allocinfo = {};
    rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    rimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // Allocate and create the image
    vmaCreateImage(m_Allocator, &rimg_info, &rimg_allocinfo, &_drawImage.image, &_drawImage.allocation, nullptr);

    // Build an image-view for the draw image to use for rendering
    VkImageViewCreateInfo rview_info = vkinit::imageview_create_info(_drawImage.imageFormat, _drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);

    VK_CHECK(vkCreateImageView(m_Device, &rview_info, nullptr, &_drawImage.imageView));

    _depthImage.imageFormat = VK_FORMAT_D32_SFLOAT;
    _depthImage.imageExtent = drawImageExtent;
    VkImageUsageFlags depthImageUsages{};
    depthImageUsages |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    VkImageCreateInfo dimg_info = vkinit::image_create_info(_depthImage.imageFormat, depthImageUsages, drawImageExtent);

    //allocate and create the image
    vmaCreateImage(m_Allocator, &dimg_info, &rimg_allocinfo, &_depthImage.image, &_depthImage.allocation, nullptr);

    //build a image-view for the draw image to use for rendering
    VkImageViewCreateInfo dview_info = vkinit::imageview_create_info(_depthImage.imageFormat, _depthImage.image, VK_IMAGE_ASPECT_DEPTH_BIT);

    VK_CHECK(vkCreateImageView(m_Device, &dview_info, nullptr, &_depthImage.imageView));
}

void Swapchain::Resize(VkPhysicalDevice chosenGPU, VkSurfaceKHR surface) {
    Destroy();

    int w, h;
    SDL_GetWindowSize(m_Window, &w, &h);
    m_WindowExtent.width = w;
    m_WindowExtent.height = h;

    Create(m_WindowExtent.width, m_WindowExtent.height, chosenGPU, surface);
}

void Swapchain::Destroy() {
    vkDestroyImageView(m_Device, _drawImage.imageView, nullptr);
    vmaDestroyImage(m_Allocator, _drawImage.image, _drawImage.allocation);
    vkDestroyImageView(m_Device, _depthImage.imageView, nullptr);
    vmaDestroyImage(m_Allocator, _depthImage.image, _depthImage.allocation);

    for (int i = 0; i < ImageViews.size(); i++) {
        vkDestroyImageView(m_Device, ImageViews[i], nullptr);
    }

    vkDestroySwapchainKHR(m_Device, SwapchainKHR, nullptr);
}