cmake_minimum_required(VERSION 3.0)
project(lab2)

set(CMAKE_TOOLCHAIN_FILE "C:/Users/JAMIE/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

find_package(assimp CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_subdirectory(external)

include_directories(
		external/glfw-3.1.2/include/
		external/glm-0.9.7.1/
		external/glad-opengl-3.3/include/
		external/tinygltf-2.9.3/
	external/
	lab2/
)

add_executable(lab2_building
        lab2/Final.cpp
	lab2/render/shader.cpp
		lab2/components/skyBox.cpp
		lab2/components/skyBox.h
		lab2/components/drone.h
		lab2/components/drone.cpp
		lab2/components/animation_model.cpp
		lab2/components/AnimationRobot/animation_model.h
		lab2/components/tinygltf_implementation.cpp
		lab2/components/floor.cpp
		lab2/components/Floor/floor.h
        lab2/components/Buildings/Building.cpp
		lab2/components/Buildings/Building.h
		lab2/components/Billboard/Billboard.cpp
		lab2/components/Billboard/Billboard.h
		lab2/components/ParticleSystem/ParticleSystem.cpp
		lab2/components/ParticleSystem/ParticleSystem.h
		lab2/components/WelcomeSign/WelcomeSign.cpp
		lab2/components/WelcomeSign/WelcomeSign.h
		lab2/components/CoolerParticles/movingParticles.cpp
		lab2/components/CoolerParticles/movingParticles.h
		lab2/components/Flag/Flag.cpp
		lab2/components/Flag/Flag.h
		lab2/components/HoverCar/HoverCar.cpp
		lab2/components/HoverCar/HoverCar.h
		lab2/components/ObjModel/ObjModel.cpp
		lab2/components/ObjModel/ObjModel.h
)
target_link_libraries(lab2_building
	${OPENGL_LIBRARY}
	glfw
	glad
		assimp::assimp
)
