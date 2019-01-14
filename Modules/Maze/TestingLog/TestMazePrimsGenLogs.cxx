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
#include <prims_generator_log.hxx>

// STD includes
#include <fstream>

// H.urna namespaces
using namespace hul;
using namespace hul::maze;

#ifndef DOXYGEN_SKIP
namespace {
  const std::string DIR = "prims";
  std::vector<uint8_t> Widths = {5, 10, 20, 30, 50, 75};
}
#endif /* DOXYGEN_SKIP */

// Generate log files
TEST(TestMazePrimsLog, build)
{
  // Generate mazes with startCell = {TopLeft, Middle, BottomRight}
  for (u_int8_t cellId = 0; cellId != 3; ++cellId)
    for (auto width = Widths.rbegin(); width != Widths.rend(); ++width)
      for (auto height = width; std::distance(width, height) != 3 && height != Widths.rend(); ++height)
      {
        PrimsGenerator::Point startPoint;
        std::string cellIdStr;

        switch (cellId) {
          case 0:
            startPoint.x = 0;
            startPoint.y = 0;
            cellIdStr = "TL";
            break;
          case 1:
            startPoint.x = (*width / 2) - 1;
            startPoint.y = (*height / 2) - 1;
            cellIdStr = "M";
            break;
          case 2:
            startPoint.x = *width - 1;
            startPoint.y = *height - 1;
            cellIdStr = "BR";
            break;
          default:
            break;
        }

        OFStream
          fileStream(DIR + "/" + ToString(*width) + "_" + ToString(*height) + "_" + cellIdStr + ".json");
        PrimsGenerator::Build(fileStream, *width, *height, startPoint);
      }
}
