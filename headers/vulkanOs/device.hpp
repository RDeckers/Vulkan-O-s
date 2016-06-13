#ifndef _VKOS_INSTANCE_H
#define  _VKOS_INSTANCE_H
#include "vulkanOs/global.hpp"
#include <vector>
namespace vkos{
  class Device{
    VkResult m_result;
    VkDevice m_device;
    #define VK_DEVICE_LEVEL_FUNCTION( fun ) PFN_##fun m_##fun = 0;
    #include "ListOfFunctions.inl"
    PFN_vkVoidFunction load_function(const char *function_name);
    void init(const VkDeviceCreateInfo *create_info, const VkAllocationCallbacks *allocator);
  public:
    Device(const VkDeviceCreateInfo &create_info){
      init(&create_info, nullptr);
    }
    Device(const VkDeviceCreateInfo &create_info, const VkAllocationCallbacks &allocator){
      init(&create_info, &allocator);
    }
    ~Device();
    VkResult result(){return m_result;}
    #define VK_DEVICE_LEVEL_FUNCTION( fun ) \
    template<typename... Args>\
    auto fun(Args... args){return m_##fun(m_device, args...);}
    #include "ListOfFunctions.inl"
  };
};
#endif
