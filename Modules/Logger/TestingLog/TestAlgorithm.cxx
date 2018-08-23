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
#include <algorithm.hxx>

using namespace HUL_Logger;

#ifndef DOXYGEN_SKIP
namespace {
  class Algo_Quick
  {
    public:
      static const std::string GetAuthor() { return "Michael Jeulin-L"; }
      static const std::string GetDoc() { return "Documentation."; }
      static const std::string GetModule() { return "Sort"; }
      static const std::string GetName() { return "QuickSort"; }
      static const std::string GetVersion() { return "1.0"; }
  };
}
#endif /* DOXYGEN_SKIP */

// Test TestAlgo Construction
TEST(TestAlgo_Traits, build)
{
  // @todo Passing arguement as: Algo_Quick::Build(std::cout, x1, x2...)
  // { Algo_Traits<Algo_Quick>::Build() };
  //Algo_Traits<Algo_Quick>::Build(std::cout, OpGetAll);
}
