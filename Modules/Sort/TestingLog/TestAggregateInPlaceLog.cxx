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
#include <aggregate_in_place_log.hxx>

// Hurna Lib namespace
using namespace hul;

// STD includes
#include <ostream>

#ifndef DOXYGEN_SKIP
namespace {
  typedef Vector<int> Vec;
  typedef Vec::h_iterator IT;
  typedef sort::AggregateInPlace<IT> Sort;
}
#endif /* DOXYGEN_SKIP */

TEST(TestAggregateInPlaceLog, buildFacto)
{
  std::stringstream dumpStream;
  auto logger = std::shared_ptr<Logger>(new Logger(dumpStream));

  // Run
  Vec data(logger, {-8, -6, -4, -3, -1, 0, 2, 4, 6, 9, -7, -5, -2, 1, 3, 5, 7, 8, 9, 10});
  auto pivot = IT(data.h_begin() + 10, "pivot"); // Rotation Index

  Sort::Build(*logger.get(), data.h_begin(), pivot, data.h_end());

  // All elements of the final array are sorted
  for (auto it = data.begin(); it < data.end() - 1; ++it)
    EXPECT_LE(*it, *(it + 1));
}
