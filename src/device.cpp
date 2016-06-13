#include "vulkanOs/instance.hpp"
#include <utilities/logging.h>

namespace vkos{
  Instance::init(const VkInstanceCreateInfo *create_info, const VkAllocationCallbacks *allocator){
    report(INFO, "initing with %p, %p", create_info, allocator);
    if(VK_SUCCESS == (m_result = vkos::vkCreateInstance(create_info, allocator, &m_instance))){
      report(PASS, "succes!");
      #define VK_INSTANCE_LEVEL_FUNCTION(fun) m_##fun = (PFN_##fun)this->load_function(#fun);
      #include "ListOfFunctions.inl"
    }else{
      report(FAIL, "Couldn't create an instance! %d/%d", m_result, VK_SUCCESS);
    }
  }

  Instance::~Instance(){

  }

  PFN_vkVoidFunction Instance::load_function(const char *function_name){
    PFN_vkVoidFunction function =  vkos::vkGetInstanceProcAddr(m_instance, function_name);
    if(nullptr == function){
      report(FAIL, "Could not load  %s", function_name);
    }else{
      report(PASS, "Loaded function %s", function_name);
    }
    return function;
  }

  std::vector<VkLayerProperties> Instance::layers(){
    std::vector<VkLayerProperties> ret;
    VkResult result;
    uint32_t count;
    if(VK_SUCCESS == (result = vkos::vkEnumerateInstanceLayerProperties(&count, nullptr))){
      ret.resize(count);
      if(VK_SUCCESS == (result = vkos::vkEnumerateInstanceLayerProperties(&count, &ret[0]))){
        return ret;
      }
    }
    report(FAIL, "Failed to enumerate instance layer properties");
    return ret;
  }

  std::vector<VkExtensionProperties> Instance::extensions(const char *layerName){
    std::vector<VkExtensionProperties> ret;
    VkResult result;
    uint32_t count;
    if(VK_SUCCESS == (result = vkos::vkEnumerateInstanceExtensionProperties(layerName, &count, nullptr))){
      ret.resize(count);
      if(VK_SUCCESS == (result = vkos::vkEnumerateInstanceExtensionProperties(layerName, &count, &ret[0]))){
        return ret;
      }
    }
    layerName ? report(FAIL, "Failed to enumerate extension properties for layer %s", layerName) : report(FAIL, "Failed to enumerate extension properties");
    return ret;
  }

  uint32_t Instance::device_count(){
    uint32_t num_devices = 0;
    if(VK_SUCCESS != vkEnumeratePhysicalDevices(&num_devices, nullptr)){
      report(FAIL, "Could not enumerate physical devices");
    }
    return num_devices;
  }

}
