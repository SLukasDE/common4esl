cmake_minimum_required(VERSION 3.23)

project(common4esl VERSION 1.5.0
            DESCRIPTION "Common files for eslx library"
            LANGUAGES CXX)
            
#include(FetchContent)
#FetchContent_Declare(
#  catch
#  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#  GIT_TAG        v2.13.6
#)
#FetchContent_MakeAvailable(catch)


# The compiled library code is here
#add_subdirectory(src/main)
# STATIC, SHARED, MODULE, INTERFACE
add_library(common4esl STATIC
      ./src/main/common4esl/ExceptionHandler.cpp
      ./src/main/common4esl/HttpStatusCode.h
      ./src/main/common4esl/ExceptionHandler.h
      ./src/main/common4esl/config/Config.h
      ./src/main/common4esl/config/context/EntryImpl.h
      ./src/main/common4esl/config/context/Database.h
      ./src/main/common4esl/config/context/Database.cpp
      ./src/main/common4esl/config/context/Procedure.cpp
      ./src/main/common4esl/config/context/BasicClient.h
      ./src/main/common4esl/config/context/BasicClient.cpp
      ./src/main/common4esl/config/context/HttpClient.cpp
      ./src/main/common4esl/config/context/HttpServer.cpp
      ./src/main/common4esl/config/context/BasicServer.h
      ./src/main/common4esl/config/context/BasicServer.cpp
      ./src/main/common4esl/config/context/Object.cpp
      ./src/main/common4esl/config/context/HttpServer.h
      ./src/main/common4esl/config/context/Context.h
      ./src/main/common4esl/config/context/Object.h
      ./src/main/common4esl/config/context/HttpClient.h
      ./src/main/common4esl/config/context/Procedure.h
      ./src/main/common4esl/config/context/Context.cpp
      ./src/main/common4esl/config/context/Entry.h
      ./src/main/common4esl/config/context/EntryImpl.cpp
      ./src/main/common4esl/config/Setting.cpp
      ./src/main/common4esl/config/Setting.h
      ./src/main/common4esl/config/FilePosition.cpp
      ./src/main/common4esl/config/Config.cpp
      ./src/main/common4esl/config/FilePosition.h
      ./src/main/common4esl/logging/MemBufferAppender.h
      ./src/main/common4esl/logging/OStreamAppender.cpp
      ./src/main/common4esl/logging/OStreamAppender.h
      ./src/main/common4esl/logging/MemBufferAppender.cpp
      ./src/main/common4esl/logging/DefaultLayout.cpp
      ./src/main/common4esl/logging/DefaultLayout.h
      ./src/main/common4esl/object/Context.h
      ./src/main/common4esl/object/Context.cpp
      ./src/main/common4esl/HttpStatusCode.cpp
      ./src/main/common4esl/Logger.h
      ./src/main/common4esl/processing/ExceptionHandler.cpp
      ./src/main/common4esl/processing/ContextEntry.cpp
      ./src/main/common4esl/processing/ContextEntry.h
      ./src/main/common4esl/processing/TaskThread.h
      ./src/main/common4esl/processing/TaskFactory.h
      ./src/main/common4esl/processing/TaskThread.cpp
      ./src/main/common4esl/processing/TaskBinding.h
      ./src/main/common4esl/processing/TaskBinding.cpp
      ./src/main/common4esl/processing/ExceptionHandler.h
      ./src/main/common4esl/processing/Context.h
      ./src/main/common4esl/processing/Context.cpp
      ./src/main/common4esl/processing/TaskFactory.cpp
)

target_include_directories(common4esl PUBLIC ./src/main)
target_compile_features(common4esl PUBLIC cxx_std_11)
set_property(TARGET common4esl PROPERTY POSITION_INDEPENDENT_CODE ON)

find_package(Boost REQUIRED)
find_package(tinyxml2 REQUIRED)
find_package(esl REQUIRED)

target_link_libraries(common4esl PRIVATE Boost::boost)
target_link_libraries(common4esl PRIVATE esl::esl)
target_link_libraries(common4esl PRIVATE tinyxml2::tinyxml2)

install(TARGETS common4esl
        EXPORT common4esl-export
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(EXPORT      common4esl-export
        FILE        common4eslTargets.cmake
        NAMESPACE   common4esl::
        DESTINATION lib/cmake
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/common4eslConfig.cmake
        DESTINATION "lib/cmake"
)




# The compiled test code is here
#add_subdirectory(src/test)
