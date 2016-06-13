#include "vulkanOs/instance.hpp"
//#include  <vulkan/vulkan.h>
#include <utilities/logging.h>
// #include <utilities/file.h>
// #include <utilities/rng.h>


using namespace vkos;
int main(int argc, const char **argv){
  int REPORT_W_COLORS = 1;

  auto extensions = Instance::extensions();
  puts("GLOBAL EXTENSIONS:");
  for(int j = 0; j < extensions.size(); j++){
    printf(" %s\n", extensions[j].extensionName);
  }
  puts("\nLAYERS:");
  auto layers = Instance::layers();
  for(int i = 0; i < layers.size(); i++){
    printf("%s:\n(%s)\n", layers[i].layerName, layers[i].description);
    puts("  EXTENSIONS:");
    auto extensions = Instance::extensions(layers[i].layerName);
    for(int j = 0; j < extensions.size(); j++){
      printf("   %s\n", extensions[j].extensionName);
    }
    puts(""
  );
  }

  VkApplicationInfo application_info = {
    VK_STRUCTURE_TYPE_APPLICATION_INFO,             // VkStructureType            sType
    nullptr,                                        // const void                *pNext
    "test app, plz ignore",  // const char                *pApplicationName
    VK_MAKE_VERSION( 1, 0, 0 ),                     // uint32_t                   applicationVersion
    "Vulkan-O's, part of a healthy pipeline!",                     // const char                *pEngineName
    VK_MAKE_VERSION( 1, 0, 0 ),                     // uint32_t                   engineVersion
    VK_MAKE_VERSION(1, 0, 3)                                  // uint32_t                   apiVersion
  };

  VkInstanceCreateInfo instance_create_info = {
    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,         // VkStructureType            sType
    nullptr,                                        // const void*                pNext
    0,                                              // VkInstanceCreateFlags      flags
    &application_info,                              // const VkApplicationInfo   *pApplicationInfo
    0,                                              // uint32_t                   enabledLayerCount
    nullptr,                                        // const char * const        *ppEnabledLayerNames
    0,                                              // uint32_t                   enabledExtensionCount
    nullptr                                         // const char * const        *ppEnabledExtensionNames
  };
  Instance instance(instance_create_info);
  if( instance.result() != VK_SUCCESS ) {
    report(FAIL, "Could not create Vulkan instance!");
    return -1;
  }

  report(PASS, "Created a vulkan instance : %p", instance);
  report(INFO, "Instance contains %u devices", instance.device_count());
  return 0;
}
