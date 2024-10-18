include(CMakeFindDependencyMacro)

find_dependency(esa)
find_dependency(esl)
find_dependency(tinyxml2)

include("${CMAKE_CURRENT_LIST_DIR}/common4eslTargets.cmake")
