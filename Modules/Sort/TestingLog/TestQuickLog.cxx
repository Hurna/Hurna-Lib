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
#include <quick_log.hxx>

// STD includes
#include <ostream>

// Hurna Lib namespace
using namespace hul;
using namespace hul::sort;

#ifndef DOXYGEN_SKIP
namespace {
  typedef Vector<int> Array;
  typedef Array::h_iterator IT;
  typedef IT::value_type T;

  // Defines the different strategies
  typedef Quick<IT, std::less<T>, picker::First<IT>> QuickFirst;
  typedef Quick<IT, std::less<T>, picker::Last<IT>> QuickLast;
  typedef Quick<IT, std::less<T>, picker::Middle<IT>> QuickMiddle;
  typedef Quick<IT, std::less<T>, picker::Random<IT>> QuickRand;
  typedef Quick<IT, std::less<T>, picker::ThreeMedian<IT>> QuickThreeMed;

  const std::vector<int> DUMP = { 1, -4, 2, 3, -1, 4, 0 , -2, -5, -3 };
}
#endif /* DOXYGEN_SKIP */

TEST(TestQuickLog, QuickFirstLog)
{
  std::stringstream dumpStream;
  auto logger = std::shared_ptr<Logger>(new Logger(dumpStream));
  Array data(logger, DUMP);

  QuickFirst::Build(*logger.get(), data.h_begin(), data.h_end());

  // All elements of the final array are sorted
  for (auto it = data.begin(); it < data.end() - 1; ++it)
    EXPECT_LE(*it, *(it + 1));
}

TEST(TestQuickLog, QuickLastLog)
{
  std::stringstream dumpStream;
  auto logger = std::shared_ptr<Logger>(new Logger(dumpStream));
  Array data(logger, DUMP);

  QuickLast::Build(*logger.get(), data.h_begin(), data.h_end());

  // All elements of the final array are sorted
  for (auto it = data.begin(); it < data.end() - 1; ++it)
    EXPECT_LE(*it, *(it + 1));
}

TEST(TestQuickLog, QuicMiddleLog)
{
  std::stringstream dumpStream;
  auto logger = std::shared_ptr<Logger>(new Logger(dumpStream));
  Array data(logger, DUMP);

  QuickMiddle::Build(*logger.get(), data.h_begin(), data.h_end());

  // All elements of the final array are sorted
  for (auto it = data.begin(); it < data.end() - 1; ++it)
    EXPECT_LE(*it, *(it + 1));
}

TEST(TestQuickLog, QuickRandLog)
{
  std::stringstream dumpStream;
  auto logger = std::shared_ptr<Logger>(new Logger(dumpStream));
  Array data(logger, DUMP);

  QuickRand::Build(*logger.get(), data.h_begin(), data.h_end());

  // All elements of the final array are sorted
  for (auto it = data.begin(); it < data.end() - 1; ++it)
    EXPECT_LE(*it, *(it + 1));
}

TEST(TestQuickLog, QuickThreeMedLog)
{
  std::stringstream dumpStream;
  auto logger = std::shared_ptr<Logger>(new Logger(dumpStream));
  Array data(logger, DUMP);

  QuickThreeMed::Build(*logger.get(), data.h_begin(), data.h_end());

  // All elements of the final array are sorted
  for (auto it = data.begin(); it < data.end() - 1; ++it)
    EXPECT_LE(*it, *(it + 1));
}
