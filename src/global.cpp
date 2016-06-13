#include "VulkanOs/global.hpp"
#include <utilities/logging.h>
#include <algorithm>
#include <string>
#ifndef _WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif

namespace vkos{
  namespace internal_function_pointers{
    vkos_lib lib_pointer = 0;
    #define VK_GLOBAL_LEVEL_FUNCTION( fun ) PFN_##fun fun = 0;
    #define VK_EXPORTED_FUNCTION( fun ) VK_GLOBAL_LEVEL_FUNCTION( fun )
    #include "ListOfFunctions.inl"
  }
  int init(){
    #ifdef _WIN32
    internal_function_pointers::lib_pointer= LoadLibrary( "vulkan-1.dll" );
    if(NULL == internal_function_pointers::lib_pointer ) {
      DWORD error = GetLastError();
      LPSTR messageBuffer = nullptr;
      size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer,
        0,
        NULL
      );
      std::string  as_string(messageBuffer, size);
      as_string.erase(std::remove(as_string.begin(), as_string.end(), '\n'), as_string.end());
      report(FAIL, "Could not open vulkan library (%d): %s", error, as_string.c_str());
      LocalFree(messageBuffer);
      return -1;
    }else{
      internal_function_pointers::vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr) GetProcAddress(internal_function_pointers::lib_pointer, "vkGetInstanceProcAddr");
      if(NULL == internal_function_pointers::vkGetInstanceProcAddr ) {
        DWORD error = GetLastError();
        LPSTR messageBuffer = nullptr;
        size_t size = FormatMessageA(
          FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
          NULL,
          error,
          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
          (LPSTR)&messageBuffer,
          0,
          NULL
        );
        std::string  as_string(messageBuffer, size);
        as_string.erase(std::remove(as_string.begin(), as_string.end(), '\n'), as_string.end());
        report(FAIL, "Could not load vkGetInstanceProcAddr (%d): %s", error, as_string.c_str());
        LocalFree(messageBuffer);
        return -2;
      }
    }
    #else //TODO: index systems which use dlopen here.
    internal_function_pointers::lib_pointer = dlopen( "libvulkan.so", RTLD_NOW );
    if(NULL == internal_function_pointers::lib_pointer ) {
      report(FAIL, "Could not open vulkan library: %s", dlerror());
      return -1;
    }else{
      char *error = dlerror();
      internal_function_pointers::vkGetInstanceProcAddr = dlsym(internal_function_pointers::lib_pointer, "vkGetInstanceProcAddr");
      error = dlerror()
      if(NULL != error){
        report(FAIL, "Could not load vkGetInstanceProcAddr: %s", error);
      }
      return -1;
    }
    //#else
    //#error I do not know how to load dynamic libraries on this system!
    #endif
    PFN_vkVoidFunction function = nullptr;
    #define VK_GLOBAL_LEVEL_FUNCTION(fun) internal_function_pointers::fun = (PFN_##fun)\
      internal_function_pointers::vkGetInstanceProcAddr(nullptr, #fun);\
    if(nullptr == internal_function_pointers::fun){\
      report(WARN, "Could not load  %s", #fun);\
    }else{\
      report(PASS, "Loaded function %s", #fun);\
    }
    #include "ListOfFunctions.inl"
    #ifdef _WIN32//TODO: deinit
    //FreeLibrary(lib_pointer);
    #else
    //dlclose(lib_pointer);
    #endif
    return 0;
  }
}
