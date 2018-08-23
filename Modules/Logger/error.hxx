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
#ifndef MODULE_LOGGER_ERROR_HXX
#define MODULE_LOGGER_ERROR_HXX

#include <Logger/typedef.hxx>

namespace HUL_Logger
{
  /// @class Error
  /// parameter.
  ///
  class Error
  {
    public:
      // Assert correct JSON construction.
      ~Error() { assert(this->writer->IsComplete()); }

      /// Instantiate a new json writer using the stream passed as
      /// argument and write value information.
      ///
      /// @return stream reference filled up with Error object information,
      ///         error information in case of failure.
      static Ostream& Build(Ostream& os, const String& file, int line, const String& message)
      {
        std::unique_ptr<Error> builder = std::unique_ptr<Error>(new Error(os));
        builder->Write(file, line, message);

        return os;
      }

      /// Use json writer passed as parameter to write iterator information.
      ///
      /// @return stream reference filled up with Error object information,
      ///         error information in case of failure.
      static Writer& Build(Writer& writer, const String& file, int line, const String& message)
      {
        Write(writer, file, line, message);

        return writer;
      }

    private:
      Error(Ostream& os) : stream(std::unique_ptr<Stream>(new Stream(os))),
                           writer(std::unique_ptr<Writer>(new Writer(*this->stream))) {}
      Error operator=(Error&) {} // Not Implemented

      bool Write(const String& file, int line, const String& message)
      { return Write(*this->writer, file, line, message); }

      static bool Write(Writer& writer, const String& file, int line, const String& message)
      {
        writer.StartObject();
        writer.Key("type");
        writer.String("error");
        writer.Key("file");
        writer.String(file);
        writer.Key("line");
        writer.Int(line);
        writer.Key("message");
        writer.String("Error: " + message);
        writer.EndObject();

        return true;
      }

      std::unique_ptr<Stream> stream; // Stream wrapper
      std::unique_ptr<Writer> writer; // Writer used to fill the stream
  };
}

#endif // MODULE_LOGGER_ERROR_HXX
