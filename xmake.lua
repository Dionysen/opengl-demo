add_rules("mode.debug", "mode.release")

-- Packages
if is_plat("linux") then 
    add_requires("devil", "glfw", "fftw", "glew", "assimp")
end

target("opengl")

-- Base
    set_kind("binary")
    set_license("GPL-2.0")

-- Sources    
    add_files("src/*.cpp","./include/glad/glad.c", "./include/imgui/src/*.cpp", "framework/*.cpp")

-- Includes
    add_includedirs("include/glad",
                    "include/imgui/include",
                    "include/glew/include",
                    "include/glfw/include",
                    "include/stb_image/include",
                    "include/glm",
                    "include/devil/include",
                    "include/fftw3",
                    "include/assimp",
                    "include",
                    ".",
                    "framework",
                    "framework/abstract_class",
                    "framework/utils",
                    "src")
    
-- Libraries
    if is_plat("linux") then 
        add_packages("devil", "glfw", "fftw", "glew", "assimp")
    end

-- Linking
    if is_plat("windows") then
        add_linkdirs(
            "include/fftw3", 
            "include/devil/lib",
            "include/glew/lib",
            "include/glfw/lib",
            "include/assimp")
        add_links("libfftw3-3", "libfftw3f-3", "libfftw3l-3", "OpenGL32", "DevIL", "glew32", "glfw3", "assimp")
    end
    -- /NODEFAULTLIB:msvcrt

    -- add_files("include/dll/*")