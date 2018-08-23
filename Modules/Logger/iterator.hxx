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
#ifndef MODULE_LOGGER_ITERATOR_HXX
#define MODULE_LOGGER_ITERATOR_HXX

#include <Logger/error.hxx>
#include <Logger/typedef.hxx>

namespace HUL_Logger
{
  /// @class Iterator
  /// parameter.
  ///
  class Iterator
  {
    public:
      // Assert correct JSON construction.
      ~Iterator() { assert(this->writer->IsComplete()); }

      /// Instantiate a new json writer using the stream passed as
      /// argument and write iterator information.
      ///
      /// @return stream reference filled up with Iterator object information,
      ///         error information in case of failure.
      static Ostream& Build
        (Ostream& os, const String& parentId, const String& name, int index, const String& comment = "")
      {
        std::unique_ptr<Iterator> builder = std::unique_ptr<Iterator>(new Iterator(os));
        builder->Write(parentId, name, index, comment);

        return os;
      }

      /// Use json writer passed as parameter to write iterator information.
      ///
      /// @return stream reference filled up with Iterator object information,
      ///         error information in case of failure.
      static Writer& Build
        (Writer& writer, const String& parentId, const String& name, int index, const String& comment = "")
      {
        Write(writer, parentId, name, index, comment);

        return writer;
      }

      /// Instantiate a new json writer using the stream passed as
      /// argument and write iterator information.
      ///
      /// @return stream reference filled up with Iterator object information,
      ///         error information in case of failure.
      template <typename T>
      static const T& BuildIt (Ostream& os, const String& parentId, const String& name, int index,
                              const T& it, const String& comment = "")
      {
        std::unique_ptr<Iterator> builder = std::unique_ptr<Iterator>(new Iterator(os));
        builder->Write(parentId, name, index, comment);

        return it;
      }

      /// Use json writer passed as parameter to write iterator information.
      ///
      /// @return stream reference filled up with Iterator object information,
      ///         error information in case of failure.
      template <typename T>
      static const T& BuildIt (Writer& writer, const String& parentId, const String& name, int index,
                               const T& it, const String& comment = "")
      {
        Write(writer, parentId, name, index, comment);

        return it;
      }

    private:
      Iterator(Ostream& os) : stream(std::unique_ptr<Stream>(new Stream(os))),
                              writer(std::unique_ptr<Writer>(new Writer(*this->stream))) {}
      Iterator operator=(Iterator&) {} // Not Implemented

      bool Write(const String& parentId, const String& name, int index, const String& comment)
      { return Write(*this->writer, parentId, name, index, comment); }

      static bool Write
        (Writer& writer, const String& parentId, const String& name, int index, const String& comment)
      {
        // Add Error Object log in case of failure
        if (parentId.empty() || name.empty())
        {
          Error::Build(writer, __FILE__, __LINE__,
            "Missing parameter: " +
            (parentId.empty()) ? "+ parentId " : "" +
            (name.empty()) ? "+ name " : "");
          return false;
        }

        // Add iterator information
        writer.StartObject();
        writer.Key("type");
        writer.String("iterator");

        writer.Key("name");
        writer.String(name);

        writer.Key("ref");
        writer.String(parentId);

        writer.Key("data");
        writer.Int(index);

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

#endif // MODULE_LOGGER_ITERATOR_HXX
