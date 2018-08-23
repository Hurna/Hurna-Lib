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
#ifndef MODULE_LOGGER_VALUE_TYPE_HXX
#define MODULE_LOGGER_VALUE_TYPE_HXX

#include <Logger/typedef.hxx>

namespace HUL_Logger
{
  /// @class ValueType
  ///
  class ValueType
  {
    public:
      // Assert correct JSON construction.
      ~ValueType() { assert(this->writer->IsComplete()); }

      /// Instantiate a new json writer using the stream passed as
      /// argument and write Value_Type information depending on type.
      ///
      /// @return stream reference filled up with Iterator object information,
      ///         error information in case of failure.
      template <typename T>
      static std::ostream& Build(std::ostream& os, const T& value)
      {
        // Create ValueType and add it to the logger
        std::unique_ptr<ValueType> builder = std::unique_ptr<ValueType>(new Iterator(os));
        builder->Write(value);

        return os;
      }

      /// Use json writer passed as parameter to write Value_Type information.
      ///
      /// @return stream reference filled up with Array object information depending on type,
      ///         error information in case of failure.
      template <typename T>
      static Writer& Build(Writer& writer, const T& value)
      {
        Write(writer, value);

        return writer;
      }

      /// Instantiate a new json writer using the stream passed as
      /// argument and write array information depending on type.
      ///
      /// @return stream reference filled up with Iterator object information,
      ///         error information in case of failure.
      template <typename IteratorT>
      static std::ostream& BuildArray(std::ostream& os, const IteratorT& begin, const IteratorT& end)
      {
        // Create ValueType logger
        std::unique_ptr<ValueType> builder = std::unique_ptr<ValueType>(new ValueType(os));

        // @todo check & log error? --> time consuming
        builder->writer->StartArray();
        for (auto it = begin; it != end; ++it)
          builder->Write(*it);
        builder->writer->EndArray();

        return os;
      }

      /// Use json writer passed as parameter to write array of value_type information.
      ///
      /// @return stream reference filled up with Array object information depending on type,
      ///         error information in case of failure.
      template <typename IteratorT>
      static Writer& BuildArray(Writer& writer, const IteratorT& begin, const IteratorT& end)
      {
        // @todo check & log error? --> time consuming
        writer.StartArray();
        for (auto it = begin; it != end; ++it)
          Write(writer, *it);
        writer.EndArray();

        return writer;
      }

    private:
      ValueType(std::ostream& os) : stream(std::unique_ptr<Stream>(new Stream(os))),
                                    writer(std::unique_ptr<Writer>(new Writer(*this->stream))) {}
      ValueType operator=(ValueType&) {} // Not Implemented

      // Wrapper using internal writer
      template <typename T>
      bool Write(T value) { return this->Write(*this->writer, value); }

      // Specifications
      static bool Write(Writer& writer, char value) { return writer.String(String(1, value)); }
      static bool Write(Writer& writer, bool value) { return writer.Bool(value); }
      static bool Write(Writer& writer, double value) { return writer.Double(value); }
      static bool Write(Writer& writer, int value) { return writer.Int(value); }
      static bool Write(Writer& writer, int64_t value) { return writer.Int64(value); }
      static bool Write(Writer& writer, const String& value) { return writer.String(value); }
      static bool Write(Writer& writer, unsigned value) { return writer.Uint(value); }
      static bool Write(Writer& writer, uint64_t value) { return writer.Uint64(value); }

      std::unique_ptr<Stream> stream; // Stream wrapper
      std::unique_ptr<Writer> writer; // Writer used to fill the stream
  };
}

#endif // MODULE_LOGGER_VALUE_TYPE_HXX
