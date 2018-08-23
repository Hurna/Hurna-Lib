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
#include "data.hxx"

// Hurna Lib namespace
using namespace hul;
using namespace hul::sort;

// STD includes
#include <fstream>
#include <functional>

#ifndef DOXYGEN_SKIP
namespace {
  typedef Vector<int> Array;
  typedef Array::h_iterator IT;
  typedef AggregateInPlace<IT> Sort;

  const std::string DIR = "aggregate";
}
#endif /* DOXYGEN_SKIP */

// Test AggregateInPlace with different integer sequences
TEST(TestAggregateInPlaceGenLogs, build)
{
  // Generate log for all Random integers
  for (auto it = HUL_DATA::RotatedIntegers.begin(); it != HUL_DATA::RotatedIntegers.end(); ++it)
  {
    OFStream stream(DIR + "/Int_Rand_" + ToString(it->second.size()) + "_" + ToString(it->first) + ".json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    Array data(logger, it->second);
    auto pivot = IT(data.h_begin() + it->first);


    Sort::Build(*logger.get(), data.h_begin(), pivot, data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}

// Test AggregateInPlace with elements in first bigger than second
TEST(TestAggregateInPlaceGenLogs, FirstBigger)
{
  std::vector<int> sizes({10, 20, 50});
  std::vector<int> pivots({4, 12, 25}); // Create rotation on those pivots

  auto pivotIdx = pivots.begin();
  for (auto size = sizes.begin(); size != sizes.end(); ++size, ++pivotIdx)
  {
    OFStream stream(DIR + "/Int_First_" + ToString(*size) + "_" + ToString(*pivotIdx) + ".json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    Array data(logger);
    data.reserve(*size);
    // Create first half with biggest elements sorted
    for (auto i = 0; i < *pivotIdx; ++i)
      data.push_back((*size / 2) - *pivotIdx + i);
    // Create second half with smallest elements sorted
    for (auto i = *pivotIdx; i < *size; ++i)
      data.push_back(i - *pivotIdx - (*size / 2));

    auto pivot = (data.h_begin() + *pivotIdx);
    Sort::Build(*logger.get(), data.h_begin(), pivot, data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}

// Test AggregateInPlace with elements in second bigger than first
TEST(TestAggregateInPlaceGenLogs, SecondBigger)
{
  std::vector<int> sizes({10, 20, 50});
  std::vector<int> pivots({4, 12, 30}); // Create rotation on those pivots

  auto pivotIdx = pivots.begin();
  for (auto size = sizes.begin(); size != sizes.end(); ++size, ++pivotIdx)
  {
    OFStream stream(DIR + "/Int_Second_" + ToString(*size) + "_" + ToString(*pivotIdx) + ".json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    Array data(logger);
    data.reserve(*size);
    // Create first half with biggest elements sorted
    for (auto i = 0; i < *pivotIdx; ++i)
      data.push_back(-(*size / 2) + i);
    // Create second half with smallest elements sorted
    for (auto i = *pivotIdx; i < *size; ++i)
      data.push_back((*size / 2) - *pivotIdx + i);

    auto pivot = (data.h_begin() + *pivotIdx);
    Sort::Build(*logger.get(), data.h_begin(), pivot, data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}

// Test with random char sequences
TEST(TestAggregateInPlaceGenLogs, RotatedChars)
{
  // Generate log for all Random integers
  for (auto it = HUL_DATA::RotatedIntegers.begin(); it != HUL_DATA::RotatedIntegers.end(); ++it)
  {
    OFStream stream(DIR + "/Char_Rand_" + ToString(it->second.size()) + "_" + ToString(it->first) + ".json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    const auto size = it->second.size();
    Vector<char> data(logger);
    data.reserve(size);

    for (auto i = 0; i < size; ++i)
      data.push_back(static_cast<char>(it->second[i] + (size / 2) + 65));
    Vector<char>::h_iterator pivot = data.h_begin() + it->first;

    AggregateInPlace<Vector<char>::h_iterator>::Build(*logger.get(), data.h_begin(), pivot, data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}
