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
#ifndef MODULE_LOGGER_COMMENT_HXX
#define MODULE_LOGGER_COMMENT_HXX

#include <Logger/typedef.hxx>

namespace HUL_Logger
{
  /// @class Comment
  /// parameter.
  ///
  class Comment
  {
    public:
      // Assert correct JSON construction.
      ~Comment() { assert(this->writer->IsComplete()); }

      /// Instantiate a new json writer using the stream passed as
      /// argument and write value information.
      ///
      /// @return stream reference filled up with Comment object information,
      ///         error information in case of failure.
      ///
      /// @note todo pass extent as an enumered type.
      static Ostream& Build(Ostream& os,
                            const String& message, int level = 0, const String extent = "normal")
      {
        std::unique_ptr<Comment> builder = std::unique_ptr<Comment>(new Comment(os));
        builder->Write(message, level, extent);

        return os;
      }

      /// Use json writer passed as parameter to write iterator information.
      ///
      /// // info, error, warn
      /// @return stream reference filled up with Comment object information,
      ///         error information in case of failure.
      static Writer& Build(Writer& writer,
                           const String& message, int level = 0, const String extent = "")
      {
        Write(writer, message, level, extent);

        return writer;
      }

    private:
      Comment(Ostream& os) : stream(std::unique_ptr<Stream>(new Stream(os))),
                             writer(std::unique_ptr<Writer>(new Writer(*this->stream))) {}
      Comment operator=(Comment&) {} // Not Implemented

      bool Write(const String& message, int level, const String& extent)
      { return Write(*this->writer, message, level, extent); }

      static bool Write(Writer& writer, const String& message, int level, const String& extent)
      {
        writer.StartObject();
        writer.Key("type");
        writer.String("comment");
        writer.Key("message");
        writer.String(message);
        writer.Key("level");
        writer.Int(level);
        if (extent != "")
        {
          writer.Key("extent");
          writer.String(extent);
        }
        writer.EndObject();

        return true;
      }

      std::unique_ptr<Stream> stream; // Stream wrapper
      std::unique_ptr<Writer> writer; // Writer used to fill the stream
  };
}

#endif // MODULE_LOGGER_COMMENT_HXX
