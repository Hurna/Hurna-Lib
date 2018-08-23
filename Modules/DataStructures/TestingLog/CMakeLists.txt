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

# Source files
set(MODULE_DS_LOG_SRCS TestMazeBinaryTreeLog.cxx
                       TestMazeDFSLog.cxx
                       TestMazeKruskalsLog.cxx
                       TestMazePrimsLog.cxx
                       TestMazeRecursiveDivisionLog.cxx
                       TestMazeSidewinderLog.cxx)

# --------------------------------------------------------------------------
# Build Testing Log executables
# --------------------------------------------------------------------------
include_directories(${MODULES_DIR})
include_directories(${LIB_DIR})
cxx_gtest(TestModuleDataStructuresLog "${MODULE_DS_LOG_SRCS}" ${HUL_SRCS})
