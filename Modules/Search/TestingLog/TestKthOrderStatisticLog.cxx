/*===========================================================================================================
 *
 * SHA-L - Simple Hybesis Algorithm Logger
 *
 * Copyright (c) Michael Jeulin-Lagarrigue
 *
 *  Licensed under the MIT License, you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://github.com/michael-jeulinl/Simple-Hybesis-Algorithms-Logger/blob/master/LICENSE
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
#include <kth_order_statistic_log.hxx>
#include "data.hxx"

// STD includes
#include <ostream>

// Testing namespace
using namespace hul;
using namespace hul::search;

#ifndef DOXYGEN_SKIP
namespace {
  typedef Vector<int> Array;
  typedef Array::h_iterator IT;
  typedef KthOrderStatistic<IT> Search;
}
#endif /* DOXYGEN_SKIP */

// Test KthElement with different integer sequences
TEST(TestKthElementLog, build)
{
  // Generate log for all Random integers
  for (auto it = DATA::Integers.begin(); it != DATA::Integers.end(); ++it)
  {
    auto rangeIt = DATA::IndexSearch.find(it->second.size());
    if (rangeIt == DATA::IndexSearch.end())
      continue;

    for (auto idxIt = rangeIt->second.begin(); idxIt != rangeIt->second.end(); ++idxIt)
    {
      std::stringstream dumpStream;
      auto logger = std::shared_ptr<Logger>(new Logger(dumpStream));
      Array data(logger, it->second);

      Search::Build(*logger.get(), data.h_begin(), data.h_end(), *idxIt);
    }
  }
}

