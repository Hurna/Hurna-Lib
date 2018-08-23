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
#include <vector.hxx>

// STD includes
#include <fstream>
#include <string>

using namespace hul;

#ifndef DOXYGEN_SKIP
namespace {}
#endif /* DOXYGEN_SKIP */

// Test Vector
TEST(TestVector, constructor)
{
  // Has to be linked to a stream otherwise useless to use
  // Can still use its normal iterators to handle unlogged operations
  OFStream fileStream("vec_log.json");
  Vector<int> sequence(fileStream, {1, 2, 3, 4, 5});
}
