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
#ifndef MODULE_LOGGER_VALUE_HXX
#define MODULE_LOGGER_VALUE_HXX

#include <Logger/error.hxx>
#include <Logger/typedef.hxx>
#include <Logger/value_type.hxx>

namespace HUL_Logger
{
  /// @class Value
  /// Value parameter
  ///
  template <typename T>
  class Value
  {
    public:
      // Assert correct JSON construction.
      ~Value() { assert(this->writer->IsComplete()); }

      /// Instantiate a new json writer using the stream passed as
      /// argument and write value information.
      ///
      /// @return stream reference filled up with Value object information,
      ///         error information in case of failure.
      static Ostream& Build(Ostream& os, const String& name, const T& value, const String& comment = "")
      {
        std::unique_ptr<Value> builder = std::unique_ptr<Value>(new Value(os));
        builder->Write(name, value, comment);

        return os;
      }

      /// Use json writer passed as parameter to write iterator information.
      ///
      /// @return stream reference filled up with Value object information,
      ///         error information in case of failure.
      static const T& BuildValue
        (Writer& writer, const String& name, const T& value, const String& comment = "")
      {
        Write(writer, name, value, comment);

        return value;
      }

      /// Instantiate a new json writer using the stream passed as
      /// argument and write value information.
      ///
      /// @return stream reference filled up with Value object information,
      ///         error information in case of failure.
      static const T& BuildValue(Ostream& os, const String& name, const T& value, const String& comment = "")
      {
        std::unique_ptr<Value> builder = std::unique_ptr<Value>(new Value(os));
        builder->Write(name, value, comment);

        return value;
      }

      /// Use json writer passed as parameter to write iterator information.
      ///
      /// @return stream reference filled up with Value object information,
      ///         error information in case of failure.
      static Writer& Build(Writer& writer, const String& name, const T& value, const String& comment = "")
      {
        Write(writer, name, value, comment);

        return writer;
      }

    private:
      Value(Ostream& os) : stream(std::unique_ptr<Stream>(new Stream(os))),
                           writer(std::unique_ptr<Writer>(new Writer(*this->stream))) {}
      Value operator=(Value&) {} // Not Implemented

      bool Write(const String& name, const T& value, const String& comment)
      { return Write(*this->writer, name, value, comment); }

      static bool Write(Writer& writer, const String& name, const T& value, const String& comment)
      {
        // Add Error Object log in case of failure
        if (name.empty())
        {
          Error::Build(writer, __FILE__, __LINE__, "name parameter empty.");
          return false;
        }

        // Add iterator information
        writer.StartObject();
        writer.Key("type");
        writer.String("value");

        writer.Key("name");
        writer.String(name);

        writer.Key("data");
        ValueType::Build<T>(writer, value);

        if (!comment.empty())
        {
          writer.Key("comment");
          writer.String(comment);
        }
        writer.EndObject();

        return true;
      }

      std::unique_ptr<Stream> stream; // Stream wrapper
      std::unique_ptr<Writer> writer; // Writer used to fill the stream
  };
}

#endif // MODULE_LOGGER_VALUE_HXX
