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
#include <maze_dfs_log.hxx>

// STD includes
#include <fstream>

// Testing namespace
using namespace HUL_Logger;

#ifndef DOXYGEN_SKIP
namespace {
  std::vector<uint8_t> Widths = {5, 10, 20, 30, 50, 75};
}
#endif /* DOXYGEN_SKIP */

// Test TestAlgo Construction
TEST(TestMazeDFSLog, build)
{
  // Generate mazes with startCell = {TopLeft, Middle, BottomRight}
  for (u_int8_t cellId = 0; cellId != 3; ++cellId)
    for (auto width = Widths.rbegin(); width != Widths.rend(); ++width)
      for (auto height = width; std::distance(width, height) != 3 && height != Widths.rend(); ++height)
      {
        std::pair<uint8_t, uint8_t> startCell;
        std::string cellIdStr;

        switch (cellId) {
          case 0:
            startCell.first = 0;
            startCell.second = 0;
            cellIdStr = "TL";
            break;
          case 1:
            startCell.first = *width / 2;
            startCell.second = *height / 2;
            cellIdStr = "M";
            break;
          case 2:
            startCell.first = *width - 1;
            startCell.second = *height - 1;
            cellIdStr = "BR";
            break;
          default:
            break;
        }

        OFStream fileStream(std::string(
                              ToString(*width) + "_" + ToString(*height) + "_" + cellIdStr + ".json"));

        // Build Maze
        MazeDFSLog::Build(fileStream, *width, *height, startCell);
      }
}
