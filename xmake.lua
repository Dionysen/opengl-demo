add_rules("mode.debug", "mode.release")
add_requires( "glfw", "fftw", "glew", "assimp")

target("opengl")
    set_kind("binary")
    set_license("GPL-2.0")
    add_files("src/*.cpp","./include/glad/glad.c", "./include/imgui/src/*.cpp", "framework/*.cpp")

    add_includedirs("include/glad",
                    "include/imgui/include",
                    "include/stb_image/include",
                    "include/glm",
                    "include/devil/include",
                    "include",
                    ".",
                    "framework",
                    "framework/abstract_class",
                    "src")

    add_packages("glfw", "fftw", "glew","assimp")
    add_linkdirs("./include/fftw3", "include/devil/lib")
    add_links("libfftw3-3","libfftw3f-3","libfftw3l-3","OpenGL32", "DevIL")