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
#include <comb_log.hxx>
#include "data.hxx"

// STD includes
#include <fstream>
#include <functional>

// Hurna Lib namespace
using namespace hul;
using namespace hul::sort;

#ifndef DOXYGEN_SKIP
namespace {
  typedef Vector<int> Array;
  typedef Array::h_iterator IT;
  typedef sort::Comb<IT> Sort;

  const std::string DIR = "comb";
}
#endif /* DOXYGEN_SKIP */

// Test Comb with different integer sequences
TEST(TestCombGenLogs, build)
{
  // Generate log for all Random integers
  for (auto it = HUL_DATA::Integers.begin(); it != HUL_DATA::Integers.end(); ++it)
  {
    OFStream stream(DIR + "/" + it->first + ".json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    Array data(logger, it->second);
    Sort::Build(*logger.get(), data.h_begin(), data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}

// Test Comb with reversed integer sequences
TEST(TestCombGenLogs, Reversed)
{
  std::vector<int> sizes({10, 20, 50, 100});

  for (auto size = sizes.begin(); size != sizes.end(); ++size)
  {
    OFStream stream(DIR + "/Int_Rev_" + ToString(*size) + ".json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    Array data(logger);
    data.reserve(*size);
    for (auto i = 0; i < *size; ++i)
      data.push_back((*size / 2) - i);

    Sort::Build(*logger.get(), data.h_begin(), data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}

// Test with reversed char sequences
TEST(TestCombGenLogs, ReversedChars)
{
  std::vector<int> sizes({10, 20, 50});

  for (auto size = sizes.begin(); size != sizes.end(); ++size)
  {
    OFStream stream(DIR + "/Char_Rev_" + ToString(*size) + ".json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    Vector<char> data(logger);
    data.reserve(*size);
    for (auto i = 0; i < *size; ++i)
      data.push_back(static_cast<char>(*size - i + 65));

    Comb<Vector<char>::h_iterator>::Build(*logger.get(), data.h_begin(), data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}

// Test with random char sequences
TEST(TestCombGenLogs, RandomChars)
{
  // Generate log for all Random integers
  for (auto it = HUL_DATA::Integers.begin(); it != HUL_DATA::Integers.end(); ++it)
  {
    auto size = it->second.size();
    if (size > 50)
      continue;

    OFStream stream(DIR + "/Char" + it->first.substr(3) + ".json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    Vector<char> data(logger);
    data.reserve(size);
    for (auto i = 0; i < size; ++i)
      data.push_back(static_cast<char>(it->second[i] + (size / 2) + 65));

    Comb<Vector<char>::h_iterator>::Build(*logger.get(), data.h_begin(), data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}
