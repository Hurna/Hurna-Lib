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
#ifndef MODULE_LOGGER_COMMAND_HXX
#define MODULE_LOGGER_COMMAND_HXX

#include <Logger/logger.hxx>

namespace hul
{
  class Swap
  {
  public:
    template <typename IT>
    void operator()(Logger& logger, const IT& first, const IT& second)
    {
      const bool sharedOwner = (first.GetOwnerRef() == second.GetOwnerRef());

      logger.StartObject();
        logger.AddEntry("type", "operation");
        logger.AddEntry("name", "Swap");

        // Owner reference(s)
        if (sharedOwner) logger.AddEntry("ref", first.GetOwnerRef());
        else
        {
          logger.StartArray("refs");
            logger.Add(first.GetOwnerRef());
            logger.Add(second.GetOwnerRef());
          logger.EndArray();
        }

        logger.StartArray("indexes");
          logger.Add(first.GetIndex());
          logger.Add(second.GetIndex());
        logger.EndArray();
      logger.EndObject();

      std::swap(*first, *second);
      first.AddSwap(true);
      second.AddSwap((sharedOwner) ? false : true);
    }
  };

  // A container will log only once the comparisons on two of its iterator
  // e.g. vector would not be able to say which iterator it owns or have owned.
  // necessaary to keep statistic reliable.
  template <typename IT, typename CompareT>
  class CompareWrap
  {
  public:
    bool operator()(const IT& first, const IT& second)
    {
      const bool sharedOwner = (first.GetOwnerRef() == second.GetOwnerRef());

      first.AddCompare(true);
      second.AddCompare((sharedOwner) ? false : true);

      return CompareT()(*first, *second);
    }
  };
}

#endif // MODULE_LOGGER_COMMAND_HXX
