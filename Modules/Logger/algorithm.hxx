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
#ifndef MODULE_LOGGER_ALGORITHM_HXX
#define MODULE_LOGGER_ALGORITHM_HXX

#include <Logger/logger.hxx>

namespace hul
{
  /// @class Algo_Traits
  ///
  template <typename Algo>
  class Algo_Traits
  {
  public:
    static bool Build(Logger& logger)
    {
      logger.AddEntry("type", Algo::GetType());
      logger.AddEntry("version", Algo::GetVersion());
      logger.AddEntry("name", Algo::GetName());
      if (logger.GetCurrentLevel() > 0) logger.AddEntry("level", logger.GetCurrentLevel());

      return true;
    }
  };
}

#endif // MODULE_LOGGER_ALGORITHM_HXX
