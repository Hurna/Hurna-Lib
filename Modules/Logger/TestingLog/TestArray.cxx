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
#include <array.hxx>

// STD includes
#include <string>

using namespace HUL_Logger;

#ifndef DOXYGEN_SKIP
namespace {
  typedef Array<std::vector<int>::iterator> Array_Type;
}
#endif /* DOXYGEN_SKIP */

// Test Array
TEST(TestArray, build)
{
  // Empty sequence
  {
    std::vector<int> sequence;
    Array_Type::Build(std::cout, "p_0", "begin", sequence.begin(), "end", sequence.end());
  }

  // Simple sequence
  {
    std::vector<int> sequence;
    sequence.push_back(2);
    sequence.push_back(-1);
    sequence.push_back(5);
    sequence.push_back(7);
    sequence.push_back(0);
    Array_Type::Build(std::cout, "p_0", "begin", sequence.begin(), "end", sequence.end());
  }

  // String sequence
  {
    std::string str = "StringToBeRespresentatedAsArray";
    Array<std::string::iterator>::Build(std::cout, "p_0", "begin", str.begin(), "end", str.end());
  }
}
