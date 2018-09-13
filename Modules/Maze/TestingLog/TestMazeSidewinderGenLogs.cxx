/*===========================================================================================================
 *
 * HUL - Hurna Lib
 *
 * Copyright (c) Michael Jeulin-Lagarrigue
 *
 *  Licensed under the MIT License, you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://github.com/Hurna/Hurna-Lib/blob/master/LICENSE
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations under the License.
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 *=========================================================================================================*/
#include <gtest/gtest.h>
#include <sidewinder_generator_generator_log.hxx>

// STD includes
#include <fstream>

// Testing namespace
using namespace HUL_Logger;

#ifndef DOXYGEN_SKIP
namespace {
  std::vector<uint8_t> Widths = {5, 10, 20, 30, 50, 75};
  std::vector<uint8_t> Seeds = {1, 2, 3, 4};
}
#endif /* DOXYGEN_SKIP */

// Test TestAlgo Construction
TEST(TestMazeSidewinderLog, build)
{
  // Generate log for all Random integers
  for (auto seed = Seeds.begin(); seed != Seeds.end(); ++seed)
    for (auto width = Widths.rbegin(); width != Widths.rend(); ++width)
      for (auto height = width; std::distance(width, height) != 3 && height != Widths.rend(); ++height)
      {
        OFStream fileStream(std::string(
                              ToString(*width) + "_" + ToString(*height) + "_" + ToString(*seed) + ".json"));

        // Build Maze
        MazeSidewinderLog::Build(fileStream, *width, *height, *seed);
      }
}
