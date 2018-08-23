#############################################################################################################
#
# HUL - Hurna Lib
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

include_directories(${MODULES_DIR})
include_directories(${LIB_DIR})

# --------------------------------------------------------------------------
# Build Testing Log executables for simple testing
# --------------------------------------------------------------------------
if(BUILD_TESTING_LOG)
  set(MODULE_SEARCH_LOG_SRCS TestBinaryLog.cxx
                             TestKthOrderStatisticLog.cxx)

  cxx_gtest(TestModuleSearchLog "${MODULE_SEARCH_LOG_SRCS}" ${HUL_SRCS})
endif()

# --------------------------------------------------------------------------
# Build Testing Log executables for the data logs generation
# --------------------------------------------------------------------------
if(BUILD_TESTING_GEN_LOGS)
  set(MODULE_SEARCH_GEN_LOGS_SRCS TestBinaryGenLogs.cxx
                                  TestKthOrderStatisticGenLogs.cxx)

  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/binary)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/order_statistic)

  cxx_gtest(TestModuleSearchGenLogs "${MODULE_SEARCH_GEN_LOGS_SRCS}" ${HUL_SRCS})
endif()
