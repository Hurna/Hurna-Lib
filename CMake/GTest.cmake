#############################################################################################################
#
# Project: Hurna Lib
#
# Copyright (c) Michael Jeulin-Lagarrigue
#
#  Licensed under the MIT License, you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#         https://github.com/Hurna/Hurna-Lib/blob/master/LICENSE
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is
# distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and limitations under the License.
#
# The above copyright notice and this permission notice shall be included in all copies or
# substantial portions of the Software.
#
#############################################################################################################

# Downloads GTest at compile time using git.
# It then compiles it.

find_package(Threads)
include(ExternalProject)

ExternalProject_Add(gtest_ext
  GIT_REPOSITORY "https://chromium.googlesource.com/external/github.com/google/googletest/"
#  GIT_TAG "d8df1fb4884c07b032ff080d95bc6506d2a33509"
  BINARY_DIR "${CMAKE_BINARY_DIR}/gtest-build"
  SOURCE_DIR "${CMAKE_BINARY_DIR}/gtest-src"
  INSTALL_COMMAND ""
  CMAKE_ARGS "${gtest_cmake_args}"
             "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
)

set(GTEST_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/gtest-src/googletest/include")
set(GTEST_SRC_DIRS "${CMAKE_BINARY_DIR}/gtest-src/googletest")
enable_testing()
function(cxx_gtest name sources sourcesDirectory)
  if(MSVC)
    link_directories("${CMAKE_BINARY_DIR}/gtest-build/googlemock/gtest/${CMAKE_BUILD_TYPE}")
  else()
    link_directories("${CMAKE_BINARY_DIR}/gtest-build/googlemock/gtest")
  endif()
  add_executable(${name} ${sources})
  add_dependencies(${name} gtest_ext)

  #target_link_libraries(
  #  ${name} ${ARGN}
  #  debug ${GTEST_SRC_DIRS}/Debug/${CMAKE_FIND_LIBRARY_PREFIXES}gtest
  #  optimized ${GTEST_SRC_DIRS}/Release/${CMAKE_FIND_LIBRARY_PREFIXES}gtest
  #  ${Pthread})

  target_link_libraries(${name} ${ARGN} gtest gtest_main ${CMAKE_THREAD_LIBS_INIT} ${Pthread})
  set_property(TARGET ${name} APPEND PROPERTY INCLUDE_DIRECTORIES ${GTEST_INCLUDE_DIRS} ${sourcesDirectory})
  add_test(NAME ${name} COMMAND ${name} "--gtest_break_on_failure")
endfunction()
