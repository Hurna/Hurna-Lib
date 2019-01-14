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
#ifndef MODULE_LOGGER_TYPEDEF_HXX
#define MODULE_LOGGER_TYPEDEF_HXX

// JSON lib includes
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

// STD includes
#include <sstream>
#include <string>

namespace hul
{
  typedef rapidjson::OStreamWrapper Stream;
  //typedef rapidjson::PrettyWriter<Stream> Writer;
  typedef rapidjson::Writer<Stream> Writer;

  // STD typedef
  typedef std::string String;
  typedef std::ostream Ostream;
  typedef std::ofstream OFStream;

  // String value
  template<typename T>
  typename std::enable_if<std::is_fundamental<T>::value, std::string>::type ToString(const T& t)
  { return std::to_string(t); }

  template<>
  inline std::string ToString<char>(const char& t)
  { return std::string(1, t); }

  // Internal id (Internal Memory Address) to String
  template<typename T>
  typename std::enable_if<std::is_pointer<T>::value, std::string>::type ToString(const T& address)
  {
    std::stringstream ss;
    ss << address;
    return ss.str();
  }

  template<typename T>
  typename std::enable_if<!std::is_fundamental<T>::value && !std::is_pointer<T>::value, std::string>::type
  ToString(const T& t)
  { return std::string(t); }

  // Range To String
  /// @todo tuple to make it more generic
  template<class Pair>
  inline std::string RangeToString(const Pair& range)
  { return "[" + ToString(range.first) + ", " + ToString(range.second) + "]"; }
}

#endif // MODULE_LOGGER_TYPEDEF_HXX
