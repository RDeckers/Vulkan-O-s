#ifndef _VKOS_INSTANCE_H
#define  _VKOS_INSTANCE_H
#include "vulkanOs/global.hpp"
#include <vector>
namespace vkos{
  class Instance{
    VkResult m_result;
    VkInstance m_instance;
    #define VK_INSTANCE_LEVEL_FUNCTION( fun ) PFN_##fun m_##fun = 0;
    #include "ListOfFunctions.inl"
    PFN_vkVoidFunction load_function(const char *function_name);
    void init(const VkInstanceCreateInfo *create_info, const VkAllocationCallbacks *allocator);
  public:
    Instance(const VkInstanceCreateInfo &create_info){
      init(&create_info, nullptr);
    }
    Instance(const VkInstanceCreateInfo &create_info, const VkAllocationCallbacks &allocator){
      init(&create_info, &allocator);
    }
    ~Instance();
    VkResult result(){return m_result;}
    #define VK_INSTANCE_LEVEL_FUNCTION( fun ) \
    template<typename... Args>\
    auto fun(Args... args){return m_##fun(m_instance, args...);}
    #include "ListOfFunctions.inl"
    uint32_t device_count();
    std::vector<int> devices();
    static std::vector<VkLayerProperties> layers();
    static std::vector<VkExtensionProperties> extensions(const char *layerName = nullptr);
  };
};
#endif
