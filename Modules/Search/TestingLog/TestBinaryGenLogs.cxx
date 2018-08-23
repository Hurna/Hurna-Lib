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
#include <binary_log.hxx>
#include "data.hxx"

// STD includes
#include <fstream>

// Testing namespace
using namespace hul;

#ifndef DOXYGEN_SKIP
namespace {
  typedef Vector<int> Array;
  typedef Array::h_iterator IT;
  typedef search::Binary<IT> Search;

  const std::string DIR = "binary";
}
#endif /* DOXYGEN_SKIP */

// Test Binary with different integer sequences
TEST(TestBinaryGenLogs, build)
{
  // Generate log for all Random integers
  for (auto it = DATA::Integers.begin(); it != DATA::Integers.end(); ++it)
  {
    auto rangeIt = DATA::IndexSearch.find(it->second.size());
    if (rangeIt == DATA::IndexSearch.end())
      continue;

    for (auto keyIt = rangeIt->second.begin(); keyIt != rangeIt->second.end(); ++keyIt)
    {
      OFStream fileStream(DIR + "/" + it->first + "_" + std::to_string(*keyIt) + ".json");
      auto logger = std::shared_ptr<Logger>(new Logger(fileStream));

      const auto key = it->second[*keyIt];
      Array data(logger, it->second);
      std::sort(data.begin(), data.end());

      Search::Build(*logger.get(), data.h_begin(), data.h_end(), key);
    }
  }
}
