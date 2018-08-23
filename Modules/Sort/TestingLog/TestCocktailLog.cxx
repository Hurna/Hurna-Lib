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
#include <cocktail_log.hxx>

// STD includes
#include <ostream>

// Hurna Lib namespace
using namespace hul;
using namespace hul::sort;

#ifndef DOXYGEN_SKIP
namespace {
  typedef Vector<int> Array;
  typedef Array::h_iterator IT;
  typedef Cocktail<IT> Sort;
}
#endif /* DOXYGEN_SKIP */

TEST(TestCocktailLog, buildFacto)
{
  std::stringstream dumpStream;
  auto logger = std::shared_ptr<Logger>(new Logger(dumpStream));

  // Run
  Array data(logger, { 1, -4, 2, 3, -1, 4, 0 , -2, -5, -3 });
  Sort::Build(*logger.get(), data.h_begin(), data.h_end());

  // Test: all elements of the final array are sorted
  for (auto it = data.begin(); it < data.end() - 1; ++it)
    EXPECT_LE(*it, *(it + 1));
}
