#ifndef _VKOS_GLOBAL_H
#define  _VKOS_GLOBAL_H
#ifdef _WIN32
#include <windows.h>
#endif
#define VK_NO_PROTOTYPES 
#include <vulkan/vulkan.h>
#include <utilities/logging.h>
namespace vkos{
  enum{
    SUCCES = 0
  };
  namespace internal_function_pointers{
    #ifdef _WIN32
      typedef HMODULE vkos_lib;
    #else
      typedef void* vkos_lib;
    #endif
    extern vkos_lib lib_pointer; //we need to maintain a pointer to the shared library.
    #define VK_GLOBAL_LEVEL_FUNCTION( fun ) extern PFN_##fun fun;
    #define VK_EXPORTED_FUNCTION( fun ) VK_GLOBAL_LEVEL_FUNCTION( fun )
    #include "ListOfFunctions.inl"
  }
  //TODO: write
  int init() __attribute__((constructor)); //Loads the neccesary top-level functions.
  #define VK_GLOBAL_LEVEL_FUNCTION( fun ) \
  template<typename... Args>\
  auto fun(Args... args){\
   /*report(INFO, "%s: calling %s (%p) via vkos pointers", __PRETTY_FUNCTION__, #fun, internal_function_pointers::fun);*/\
   return internal_function_pointers::fun(args...);\
 }
  #define VK_EXPORTED_FUNCTION( fun ) VK_GLOBAL_LEVEL_FUNCTION( fun )
  #include "ListOfFunctions.inl"
}
#endif //  _VKOS_GLOBAL_H
