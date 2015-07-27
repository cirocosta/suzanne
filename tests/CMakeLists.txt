# Configure GTEST

find_package(Threads REQUIRED)
include(ExternalProject)
ExternalProject_Add(
  gtest
  GIT_REPOSITORY https://github.com/cirocosta/googletest.git
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
)

add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)

ExternalProject_Get_Property(gtest source_dir binary_dir)
set_target_properties(libgtest PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/libgtest.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
include_directories("${source_dir}/include")


#  --  StyleTree

add_executable(styletree_test main_test.cc styletree_test.cc)
target_link_libraries(styletree_test libgtest)
add_test(NAME styletree_test COMMAND styletree_test)

