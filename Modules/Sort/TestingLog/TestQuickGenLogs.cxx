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
#include "data.hxx"

// STD includes
#include <fstream>
#include <functional>

// Hurna Lib namespace
using namespace hul;
using namespace hul::sort;

#ifndef DOXYGEN_SKIP
namespace
{
  typedef Vector<int> Array;
  typedef Array::h_iterator IT;
  typedef IT::value_type T;

  // Defines the different strategies
  typedef Quick<IT, std::less<T>, picker::Random<IT>> QuickRand;
  typedef Quick<IT, std::less<T>, picker::First<IT>> QuickFirst;
  typedef Quick<IT, std::less<T>, picker::Last<IT>> QuickLast;
  typedef Quick<IT, std::less<T>, picker::Middle<IT>> QuickMiddle;
  typedef Quick<IT, std::less<T>, picker::ThreeMedian<IT>> QuickThreeMed;

  const std::string DIR = "quick";
}
#endif /* DOXYGEN_SKIP */

// Test Quick with different integer sequences
TEST(TestQuickGenLogs, buildPRand)
{
  // Generate log for all Random integers
  for (auto it = HUL_DATA::Integers.begin(); it != HUL_DATA::Integers.end(); ++it)
  {
    OFStream stream(DIR + "/" + it->first + "_prand.json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    Array data(logger, it->second);
    QuickRand::Build(*logger.get(), data.h_begin(), data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}

// Test Quick with reversed integer sequences
TEST(TestQuickGenLogs, ReversedPRand)
{
  std::vector<int> sizes({10, 20, 50, 100});

  for (auto size = sizes.begin(); size != sizes.end(); ++size)
  {
    OFStream stream(DIR + "/Int_Rev_" + ToString(*size) + "_prand.json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    Array data(logger);
    data.reserve(*size);
    for (auto i = 0; i < *size; ++i)
      data.push_back((*size / 2) - i);

    QuickRand::Build(*logger.get(), data.h_begin(), data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}

// Test Quick using first picking strategy with different integer sequences
TEST(TestQuickGenLogs, buildPFirst)
{
  std::vector<std::string> dataStr = {"Int_Rand_20", "Int_Rand_50", "Int_Rand_100"};

  // Generate log for all Random integers
  for (auto dataStrIt = dataStr.begin(); dataStrIt != dataStr.end(); ++dataStrIt)
  {
    OFStream stream(DIR + "/" + *dataStrIt + "_pfirst.json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    auto dataIt = HUL_DATA::Integers.find(*dataStrIt);
    Array data(logger, dataIt->second);

    QuickFirst::Build(*logger.get(), data.h_begin(), data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}

// Test Quick with reversed integer sequences
TEST(TestQuickGenLogs, ReversedPFirst)
{
  std::vector<int> sizes({10, 20, 50, 100});

  for (auto size = sizes.begin(); size != sizes.end(); ++size)
  {
    OFStream stream(DIR + "/Int_Rev_" + ToString(*size) + "_pfirst.json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    Array data(logger);
    data.reserve(*size);
    for (auto i = 0; i < *size; ++i)
      data.push_back((*size / 2) - i);

    QuickFirst::Build(*logger.get(), data.h_begin(), data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}

// Test Quick using last picking strategy with different integer sequences
TEST(TestQuickGenLogs, buildPLast)
{
  std::vector<std::string> dataStr = {"Int_Rand_20", "Int_Rand_50", "Int_Rand_100"};

  // Generate log for all Random integers
  for (auto dataStrIt = dataStr.begin(); dataStrIt != dataStr.end(); ++dataStrIt)
  {
    OFStream stream(DIR + "/" + *dataStrIt + "_plast.json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    auto dataIt = HUL_DATA::Integers.find(*dataStrIt);
    Array data(logger, dataIt->second);

    QuickFirst::Build(*logger.get(), data.h_begin(), data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}

// Test Quick using last picking strategy with reversed integer sequences
TEST(TestQuickGenLogs, ReversedPLast)
{
  std::vector<int> sizes({10, 20, 50, 100});

  for (auto size = sizes.begin(); size != sizes.end(); ++size)
  {
    OFStream stream(DIR + "/Int_Rev_" + ToString(*size) + "_plast.json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    Array data(logger);
    data.reserve(*size);
    for (auto i = 0; i < *size; ++i)
      data.push_back((*size / 2) - i);

    QuickFirst::Build(*logger.get(), data.h_begin(), data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}

// Test Quick using middle picking strategy with different integer sequences
TEST(TestQuickGenLogs, buildPMiddle)
{
  std::vector<std::string> dataStr = {"Int_Rand_20", "Int_Rand_50", "Int_Rand_100"};

  // Generate log for all Random integers
  for (auto dataStrIt = dataStr.begin(); dataStrIt != dataStr.end(); ++dataStrIt)
  {
    OFStream stream(DIR + "/" + *dataStrIt + "_pmiddle.json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    auto dataIt = HUL_DATA::Integers.find(*dataStrIt);
    Array data(logger, dataIt->second);

    QuickFirst::Build(*logger.get(), data.h_begin(), data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}

// Test Quick using middle picking strategy with reversed integer sequences
TEST(TestQuickGenLogs, ReversedPMiddle)
{
  std::vector<int> sizes({10, 20, 50, 100});

  for (auto size = sizes.begin(); size != sizes.end(); ++size)
  {
    OFStream stream(DIR + "/Int_Rev_" + ToString(*size) + "_pmiddle.json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    Array data(logger);
    data.reserve(*size);
    for (auto i = 0; i < *size; ++i)
      data.push_back((*size / 2) - i);

    QuickFirst::Build(*logger.get(), data.h_begin(), data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}

// Test Quick using three median picking strategy with different integer sequences
TEST(TestQuickGenLogs, buildTmed)
{
  std::vector<std::string> dataStr = {"Int_Rand_20", "Int_Rand_50", "Int_Rand_100"};

  // Generate log for all Random integers
  for (auto dataStrIt = dataStr.begin(); dataStrIt != dataStr.end(); ++dataStrIt)
  {
    OFStream stream(DIR + "/" + *dataStrIt + "_ptmed.json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    auto dataIt = HUL_DATA::Integers.find(*dataStrIt);
    Array data(logger, dataIt->second);

    QuickFirst::Build(*logger.get(), data.h_begin(), data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}

// Test Quick using three median picking strategy with reversed integer sequences
TEST(TestQuickGenLogs, ReversedPTMed)
{
  std::vector<int> sizes({10, 20, 50, 100});

  for (auto size = sizes.begin(); size != sizes.end(); ++size)
  {
    OFStream stream(DIR + "/Int_Rev_" + ToString(*size) + "_ptmed.json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    Array data(logger);
    data.reserve(*size);
    for (auto i = 0; i < *size; ++i)
      data.push_back((*size / 2) - i);

    QuickFirst::Build(*logger.get(), data.h_begin(), data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}

// Test with reversed char sequences
TEST(TestQuickGenLogs, ReversedChars)
{
  std::vector<int> sizes({10, 20, 50});

  for (auto size = sizes.begin(); size != sizes.end(); ++size)
  {
    OFStream stream(DIR + "/Char_Rev_" + ToString(*size) + "_prand.json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    Vector<char> data(logger);
    data.reserve(*size);
    for (auto i = 0; i < *size; ++i)
      data.push_back(static_cast<char>(*size - i + 65));

    Quick<Vector<char>::h_iterator, std::less<char>, picker::Random<Vector<char>::h_iterator>>::
      Build(*logger.get(), data.h_begin(), data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}

// Test with random char sequences
TEST(TestQuickGenLogs, RandomChars)
{
  // Generate log for all Random integers
  for (auto it = HUL_DATA::Integers.begin(); it != HUL_DATA::Integers.end(); ++it)
  {
    auto size = it->second.size();
    if (size > 50)
      continue;

    OFStream stream(DIR + "/Char" + it->first.substr(3) + "_prand.json");
    auto logger = std::shared_ptr<Logger>(new Logger(stream));

    Vector<char> data(logger);
    data.reserve(size);
    for (auto i = 0; i < size; ++i)
      data.push_back(static_cast<char>(it->second[i] + (size / 2) + 65));

    Quick<Vector<char>::h_iterator, std::less<char>, picker::Random<Vector<char>::h_iterator>>::
      Build(*logger.get(), data.h_begin(), data.h_end());

    // All elements of the final array are sorted
    for (auto it = data.begin(); it < data.end() - 1; ++it)
      EXPECT_LE(*it, *(it + 1));
  }
}
