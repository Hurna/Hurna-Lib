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
#ifndef MODULE_LOGGER_ARRAY_HXX
#define MODULE_LOGGER_ARRAY_HXX

#include <Logger/iterator.hxx>
#include <Logger/value_type.hxx>

namespace HUL_Logger
{
  /// @class Array
  /// used to write array parameter.
  ///
  template <typename IT>
  class Array
  {
    public:
      ~Array() { assert(this->writer->IsComplete()); }

      /// Instantiate a new json writer using the stream passed as
      /// argument and write array information.
      ///
      /// @return stream reference filled up with Iterator object information,
      ///         error information in case of failure.
      static Ostream& Build(Ostream& os, const String& name,
                            const String& beginName, const IT& begin,
                            const String& endName, const IT& end)
      {
        std::unique_ptr<Array> builder = std::unique_ptr<Array>(new Array(os));
        builder->Write(name, beginName, begin, endName, end);

        return os;
      }

      /// Use json writer passed as parameter to write iterator information.
      ///
      /// @return stream reference filled up with Array object information,
      ///         error information in case of failure.
      static Writer& Build(Writer& writer, const String& name,
                           const String& beginName, const IT& begin,
                           const String& endName, const IT& end)
      {
        Write(writer, name, beginName, begin, endName, end);

        return writer;
      }

    private:
      Array(Ostream& os) : stream(std::unique_ptr<Stream>(new Stream(os))),
                           writer(std::unique_ptr<Writer>(new Writer(*this->stream))) {}
      Array operator=(Array&) {} // Not Implemented

      bool Write(const String& name,
                 const String& beginName, const IT& begin,
                 const String& endName, const IT& end)
      { return Write(*this->writer, name, beginName, begin, endName, end); }

      static bool Write(Writer& writer, const String& name,
                        const String& beginName, const IT& begin,
                        const String& endName, const IT& end)
      {
        // Add Error Object log in case of failure
        if (name.empty() || beginName.empty() || endName.empty())
        {
          Error::Build(writer, __FILE__, __LINE__,
            "Missing parameter: " +
            (name.empty()) ? "+ name " : "" +
            (beginName.empty()) ? "+ beginName " : "" +
            (endName.empty()) ? "+ endName " : "");
          return false;
        }

        // 2*N for non random accessible iterator
        const int kdataSize = static_cast<int>(std::distance(begin, end));
        if (kdataSize < 1)
        {
          Error::Build(writer, __FILE__, __LINE__, "empty/invalid sequence detected.");
          return false;
        }

        // StartBuild - Main information
        writer.StartObject();
          writer.Key("type");
          writer.String("array");
          writer.Key("name");
          writer.String(name);

          // Add data
          writer.Key("data");
          ValueType::BuildArray<IT>(writer, begin, end);

          // Add Iterators
          writer.Key("iterators");
          writer.StartArray();
          Iterator::Build(writer, name, beginName, 0);
          Iterator::Build(writer, name, endName, static_cast<int>(kdataSize));
          writer.EndArray();

        // Finish object
        writer.EndObject();

        return true;
      }

      std::unique_ptr<Stream> stream; // Stream wrapper
      std::unique_ptr<Writer> writer; // Writer used to fill the stream
  };
}

#endif // MODULE_LOGGER_ARRAY_HXX
