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
#ifndef MODULE_LOGGER_OPTIONS_HXX
#define MODULE_LOGGER_OPTIONS_HXX

namespace HUL_Logger
{
  // @todo option with template<stats> --> avoid manual cp/past
  // @todo comment Level to retrieve (-1 for none) --> log file much much lighter

  enum Options
  {
    OpGetMin = 0x00,      // Retrieve only logs
    OpGetName = 0x01,     // Retrieve the name
    OpGetDoc = 0x02,      // Retrieve the technical documentation
    OpGetInfo = 0x04,     // Retrieve diverse information
    OpGetSrc = 0x08,      // Retrieve the original source code
    OpGetComment = 0x10,  // Retrieve comments written during computation
    OpIsSub = 0x10,       // !SubRoutine Call - Do not build parameter but only indexes!
    OpGetAll = OpGetName | OpGetDoc | OpGetInfo | OpGetSrc  // Retrieve all information
  };
}

#endif // MODULE_LOGGER_OPTIONS_HXX
