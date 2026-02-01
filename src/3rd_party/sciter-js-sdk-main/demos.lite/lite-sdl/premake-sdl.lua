

function include_files_sdl() 

    local DIR = "demos.lite/lite-sdl/"
  
    defines {"SDL_STATIC_LIB", "SDL_BUILD_MAJOR_VERSION=3", "SDL_BUILD_MINOR_VERSION=2", "SDL_BUILD_MICRO_VERSION=20"}

    filter {}

    includedirs { 
      DIR .. "sdl/include/build_config", 
      DIR .. "sdl/include",
      DIR .. "sdl/include/SDL3",
      DIR .. "sdl/src",
      DIR .. "sdl/src/video" }

    externalincludedirs {
      DIR .. "sdl/include",
    }      

    files { 

       DIR .. "sdl/include/SDL3/*.h",
       DIR .. "sdl/include/build_config/*.h",

       DIR .. "sdl/src/*.c",    
       DIR .. "sdl/src/*.h",    
       DIR .. "sdl/src/atomic/*.c",    
       DIR .. "sdl/src/atomic/*.h",    

       DIR .. "sdl/src/audio/*.c",    
       DIR .. "sdl/src/audio/dummy/*.c",    
       DIR .. "sdl/src/audio/disk/*.c",    

       DIR .. "sdl/src/core/*.c", DIR .. "sdl/src/core/*.h",      
       DIR .. "sdl/src/cpuinfo/*.c",    
       DIR .. "sdl/src/dynapi/*.c",    
       DIR .. "sdl/src/events/*.c", DIR .. "sdl/src/events/*.h",     
       DIR .. "sdl/src/file/*.c", DIR .. "sdl/src/file/*.h",   
       DIR .. "sdl/src/haptic/*.c",    
       DIR .. "sdl/src/hidapi/*.c",    
       
       DIR .. "sdl/src/joystick/*.c",    
       DIR .. "sdl/src/joystick/virtual/*.c", 
       DIR .. "sdl/src/joystick/hidapi/*.c",  

       DIR .. "sdl/src/libm/*.c",    
       DIR .. "sdl/src/locale/*.c",    
       DIR .. "sdl/src/main/*.c",    
       DIR .. "sdl/src/misc/*.c",    
       DIR .. "sdl/src/power/*.c",    

       DIR .. "sdl/src/render/*.c",    
       DIR .. "sdl/src/render/software/*.c",    

       DIR .. "sdl/src/render/gpu/*.c",
       DIR .. "sdl/src/render/opengl/*.c",  
       DIR .. "sdl/src/render/opengles2/*.c",    
       DIR .. "sdl/src/render/vulkan/*.c",
       DIR .. "sdl/src/render/ps2/*.c",    
       DIR .. "sdl/src/render/psp/*.c",    
       DIR .. "sdl/src/render/vitagxm/*.c",    

       DIR .. "sdl/src/sensor/*.c",    
       DIR .. "sdl/src/stdlib/*.c",    
       DIR .. "sdl/src/thread/*.c",    
       DIR .. "sdl/src/timer/*.c",    

       DIR .. "sdl/src/video/*.c", DIR .. "sdl/src/video/*.h",     
       DIR .. "sdl/src/video/yuv2rgb/*.c", 
       DIR .. "sdl/src/video/dummy/*.c",    

       DIR .. "sdl/src/camera/*.c",    
       DIR .. "sdl/src/camera/dummy/*.c",

       DIR .. "sdl/src/dialog/*.c",

       DIR .. "sdl/src/main/*.c",    
       DIR .. "sdl/src/main/*.h",    

       DIR .. "sdl/src/tray/*.c",    
       DIR .. "sdl/src/tray/*.h",    

       DIR .. "sdl/src/io/*.c",    
       DIR .. "sdl/src/io/*.h",    
       DIR .. "sdl/src/io/generic/*.c",

       DIR .. "sdl/src/process/*.c",    
       DIR .. "sdl/src/process/*.h",    

       DIR .. "sdl/src/gpu/*.c",    
       DIR .. "sdl/src/gpu/*.h",    


       DIR .. "sdl/src/video/offscreen/*.c",
       DIR .. "sdl/src/main/generic/SDL_sysmain_callbacks.c",

       DIR .. "sdl/src/time/*.c",DIR .. "sdl/src/time/*.h",
       DIR .. "sdl/src/storage/*.c", DIR .. "sdl/src/storage/*.h",
       DIR .. "sdl/src/filesystem/*.c", DIR .. "sdl/src/filesystem/*.h",

      }

      filter "system:windows"

        files { 

         DIR .. "sdl/src/render/direct3d/*.c",
         DIR .. "sdl/src/render/direct3d11/*.c",
         DIR .. "sdl/src/render/direct3d12/*.c",

         DIR .. "sdl/src/tray/windows/*.c",    
         DIR .. "sdl/src/process/windows/*.c",    
         DIR .. "sdl/src/io/windows/*.c",
         DIR .. "sdl/src/core/windows/*.c",    
         DIR .. "sdl/src/misc/windows/*.c",    
         DIR .. "sdl/src/audio/directsound/*.c",    
         DIR .. "sdl/src/audio/wasapi/*.c*",    
         DIR .. "sdl/src/video/windows/*.c",
         DIR .. "sdl/src/video/windows/*.cpp",
         DIR .. "sdl/src/sensor/windows/*.c", 
         DIR .. "sdl/src/thread/generic/SDL_syscond.c",    
         DIR .. "sdl/src/thread/generic/SDL_sysrwlock.c",    
         DIR .. "sdl/src/thread/windows/*.c",    
         DIR .. "sdl/src/power/windows/*.c",    
         DIR .. "sdl/src/locale/windows/*.c",    
         DIR .. "sdl/src/filesystem/windows/*.c",    
         DIR .. "sdl/src/timer/windows/*.c",    
         DIR .. "sdl/src/loadso/windows/*.c",    
         DIR .. "sdl/src/joystick/hidapi/*.c",    
         DIR .. "sdl/src/joystick/windows/*.c",
         DIR .. "sdl/src/joystick/gdk/*.c",    
         DIR .. "sdl/src/haptic/windows/*.c",   
         DIR .. "sdl/src/dialog/windows/*.c",  
         DIR .. "sdl/src/camera/mediafoundation/*.c",
         DIR .. "sdl/src/time/windows/*.c",
         DIR .. "sdl/src/storage/generic/*.c",
         DIR .. "sdl/src/filesystem/windows/*.c",

         DIR .. "sdl/src/main/windows/*.c*",

         DIR .. "sdl/src/gpu/vulkan/*.c",
         DIR .. "sdl/src/gpu/d3d12/*.c",    
       }

       links { "setupapi", "imm32", "winmm", "version" }
       
    filter "system:linux"
      
      defines {
        "linux",
        "SDL_PLATFORM_LINUX",
        "SDL_THREAD_PTHREAD",
      }

      files { 

       DIR .. "sdl/src/core/linux/*.c",    
       DIR .. "sdl/src/core/unix/*.c",    
       DIR .. "sdl/src/misc/unix/*.c",    

       DIR .. "sdl/src/audio/alsa/*.c",    
       DIR .. "sdl/src/audio/pulseaudio/*.c",    
       DIR .. "sdl/src/audio/aaudio/*.c",
       
       DIR .. "sdl/src/video/wayland/*.c", DIR .. "sdl/src/video/wayland/*.h",  
       DIR .. "sdl/src/video/x11/*.c", DIR .. "sdl/src/video/x11/*.h",
       -- DIR .. "sdl/src/video/kmsdrm/*.c",
       
       DIR .. "sdl/src/sensor/dummy/*.c", 

       DIR .. "sdl/src/thread/pthread/*.c",

       DIR .. "sdl/src/power/linux/*.c",    
       DIR .. "sdl/src/locale/unix/*.c",    
       DIR .. "sdl/src/filesystem/unix/*.c",    
       DIR .. "sdl/src/timer/unix/*.c",    

       DIR .. "sdl/src/loadso/dlopen/*.c",    
       DIR .. "sdl/src/joystick/linux/*.c", 
       DIR .. "sdl/src/joystick/hidapi/*.c",    
       DIR .. "sdl/src/joystick/steam/*.c",
       DIR .. "sdl/src/haptic/linux/*.c",   
       DIR .. "sdl/src/camera/v4l2/*.c",   
       DIR .. "sdl/src/dialog/unix/*.c",  

       DIR .. "sdl/src/time/unix/*.c",
       DIR .. "sdl/src/storage/generic/*.c",

       DIR .. "sdl/src/filesystem/unix/*.c",
       DIR .. "sdl/src/filesystem/posix/*.c",
       
       DIR .. "sdl/wayland-generated-protocols/*.c",
       
       DIR .. "sdl/src/tray/unix/*.c",
       DIR .. "sdl/src/process/posix/*.c",
     }

     includedirs {
       DIR .. "sdl/wayland-generated-protocols"
     }
     
     buildoptions {
       "`pkg-config dbus-1 --cflags`",      
     }



     filter "system:macosx"
      
      defines {
        --"SDL_PLATFORM_LINUX",
        --"SDL_THREAD_PTHREAD",
      }

      files { 

       DIR .. "sdl/src/core/unix/*.c",    
       DIR .. "sdl/src/misc/unix/*.c",    
       DIR .. "sdl/src/file/cocoa/*.*", 

       DIR .. "sdl/src/render/metal/*.*",    

       DIR .. "sdl/src/audio/coreaudio/*.*",    
       
       DIR .. "sdl/src/video/cocoa/*.*",    
       
       DIR .. "sdl/src/sensor/dummy/*.c", 

       DIR .. "sdl/src/thread/pthread/*.c",

       DIR .. "sdl/src/power/macos/*.*",    
       DIR .. "sdl/src/locale/macos/*.*",    
       DIR .. "sdl/src/filesystem/cocoa/*.*",    
       DIR .. "sdl/src/timer/unix/*.*",    

       DIR .. "sdl/src/loadso/dlopen/*.c",    
       DIR .. "sdl/src/joystick/apple/*.*",
       DIR .. "sdl/src/joystick/darwin/*.*", 
       DIR .. "sdl/src/haptic/darwin/*.*",   
       DIR .. "sdl/src/camera/coremedia/*.*",   
       DIR .. "sdl/src/dialog/cocoa/*.*", 

       DIR .. "sdl/src/time/unix/*.c",
       DIR .. "sdl/src/storage/generic/*.c",
       DIR .. "sdl/src/filesystem/cocoa/*.c",
       
     }


    filter {}
end

