/*===========================================================================================================
 *
 * HUC - Hurna Core
 *
 * Copyright (c) Michael Jeulin-Lagarrigue
 *
 *  Licensed under the MIT License, you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://github.com/Hurna/Hurna-Core/blob/master/LICENSE
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
#include <comb.hxx>

// STD includes
#include <functional>
#include <vector>
#include <string>

// Testing namespace
using namespace huc::sort;

#ifndef DOXYGEN_SKIP
namespace {
  // Simple sorted array of integers with negative values
  const int SortedArrayInt[] = {-3, -2, 0, 2, 8, 15, 36, 212, 366};
  // Simple random array of integers with negative values
  const int RandomArrayInt[] = {4, 3, 5, 2, -18, 3, 2, 3, 4, 5, -5};
  // Random string
  const std::string RandomStr = "xacvgeze";

  typedef std::vector<int> Container;
  typedef Container::iterator IT;
  typedef std::greater<IT::value_type> GE_Comparator;
}
#endif /* DOXYGEN_SKIP */

// Basic Comb-Sort tests
TEST(TestSort, CombSorts)
{
  // Normal Run
  {
    Container randomdArray(RandomArrayInt, RandomArrayInt + sizeof(RandomArrayInt) / sizeof(int));
    Comb<IT>(randomdArray.begin(), randomdArray.end());

    // All elements are sorted
    for (auto it = randomdArray.begin(); it < randomdArray.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }

  // Already sortedArray - Array should not be affected
  {
    Container sortedArray(SortedArrayInt, SortedArrayInt + sizeof(SortedArrayInt) / sizeof(int));
    Comb<IT>(sortedArray.begin(), sortedArray.end());

    // All elements are still sorted
    for (auto it = sortedArray.begin(); it < sortedArray.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }

  // Inverse iterator order - Array should not be affected
  {
    Container randomdArray(RandomArrayInt, RandomArrayInt + sizeof(RandomArrayInt) / sizeof(int));
    Comb<IT>(randomdArray.end(), randomdArray.begin());

    int i = 0;
    for (auto it = randomdArray.begin(); it < randomdArray.end(); ++it, ++i)
      EXPECT_EQ(RandomArrayInt[i], *it);
  }

  // No error unitialized array
  {
    Container emptyArray;
    Comb<IT>(emptyArray.begin(), emptyArray.end());
  }

  // Unique value array - Array should not be affected
  {
    Container uniqueValueArray(1, 511);
    Comb<IT>(uniqueValueArray.begin(), uniqueValueArray.end());
    EXPECT_EQ(511, uniqueValueArray[0]);
  }

  // String - String should be sorted as an array
  {
    std::string stringToSort = RandomStr;
    Comb<std::string::iterator, std::less<char>>(stringToSort.begin(), stringToSort.end());
    for (auto it = stringToSort.begin(); it < stringToSort.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}

// Basic Comb-Sort tests - Inverse Order
TEST(TestSort, CombGreaterComparator)
{
  // Normal Run - Elements should be sorted in inverse order
  {
    Container randomdArray(RandomArrayInt, RandomArrayInt + sizeof(RandomArrayInt) / sizeof(int));
    Comb<IT, GE_Comparator>(randomdArray.begin(), randomdArray.end());

    // All elements are sorted in inverse order
    for (auto it = randomdArray.begin(); it < randomdArray.end() - 1; ++it)
      EXPECT_GE(*it, *(it + 1));
  }

  // Already sorted Array in inverse order - Array should not be affected
  {
    Container invSortedArray(SortedArrayInt, SortedArrayInt + sizeof(SortedArrayInt) / sizeof(int));
    Comb<IT, GE_Comparator>(invSortedArray.begin(), invSortedArray.end());

    // All elements are still sorted in inverse order
    for (auto it = invSortedArray.begin(); it < invSortedArray.end() - 1; ++it)
      EXPECT_GE(*it, *(it + 1));
  }

  // String - String should be sorted in inverse order
  {
    std::string stringToSort = RandomStr;
    Comb<std::string::iterator, std::greater<char>>(stringToSort.begin(), stringToSort.end());

    // All elements are sorted in inverse order
    for (auto it = stringToSort.begin(); it < stringToSort.end() - 1; ++it)
      EXPECT_GE(*it, *(it + 1));
  }
}
