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

project(HULModuleSort)

# Source files
set(HUC_SRCS "${CMAKE_CURRENT_SOURCE_DIR}")
set(HUL_SRCS "${CMAKE_CURRENT_SOURCE_DIR}")

# Testing
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()

# Testing Log
if(BUILD_TESTING_LOG OR BUILD_TESTING_GEN_LOGS)
  add_subdirectory(TestingLog)
endif()
