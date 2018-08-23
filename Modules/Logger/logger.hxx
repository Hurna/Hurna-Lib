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
#ifndef MODULE_LOGGER_LOGGER_HXX
#define MODULE_LOGGER_LOGGER_HXX

#include <Logger/typedef.hxx>

namespace hul
{
  // Should be its own class
  class Logger
  {
  public:
    Logger(Ostream& os) :
      currentLevel(-1),
      stream(std::unique_ptr<Stream>(new Stream(os))),
      writer(std::unique_ptr<Writer>(new Writer(*this->stream))) {}

    // Assert writer has finished
    ~Logger() { assert(this->writer->IsComplete()); }

    void AddEntry(const String& key, const String& value)
    {
      writer->Key(key);
      writer->String(value);
    }

    void AddEntry(const String& key, const int value)
    {
      writer->Key(key);
      writer->Int(value);
    }

    void AddValue(const String& name, const int  value)
    {
      writer->StartObject();
      writer->Key("type");
      writer->String("value");
      writer->Key("name");
      writer->String(name);
      writer->Key("data");
      writer->Int(value);
      writer->EndObject();
    }

    /*template <typename T>
    void AddObject(const String& key, const T& value, bool isConst=false)
    {
      writer->StartObject();
      AddEntry(key, value);
      writer->EndObject();
    }*/

    template <typename T>
    void AddObject(const T& object, bool isConst=false) { object.Log(isConst); }

    template <typename IT>
    void AddData(const IT& begin, const IT& end, const String& key = "")
    {
      if (!key.empty()) writer->Key(key);

      StarArray();
        for (auto it = begin; it != end; ++it) Add(*it);
      EndArray();
    }

    template <typename IT>
    void AddStats(const IT& it, bool logOwner = false)
    {
      if (logOwner)
        it.LogOwnerStats();
      it.LogStats();
    }

    template <typename T>
    void Return(const T& value)
    {
      StartObject();
        AddEntry("type", "operation");
        AddEntry("name", "Return");
        AddEntry("data", value);
      EndObject();
    }

    template <typename IT>
    void AddDataDetails(const IT& begin, const IT& end, bool isConst=false)
    {
      StartObject();
        AddEntry("type", begin.GetOwnerType());
        AddEntry("name", begin.GetOwnerRef());
        AddData(begin, end, "data");

        StartArray("iterators");
          AddObject(begin, isConst);
          AddObject(end, isConst);
        EndArray();
      EndObject();
    }

    void Start()
    {
      ++currentLevel;
      writer->StartObject();
    }

    void StartLoop(const String& comment = "")
    {
      if (!comment.empty()) Comment(comment);
      ++currentLevel;
    }

    void EndLoop(const String& comment = "")
    {
      --currentLevel;
      if (!comment.empty()) Comment(comment);
    }

    void End()
    {
      --currentLevel;
      writer->EndObject();
    }

    /// @todo make it tuple
    template<class Pair>
    void SetRange(const Pair& range)
    {
      StartObject();
        AddEntry("type", "operation");
        AddEntry("name", "setRange");
        StartArray("range");
          writer->Int(range.first);
          writer->Int(range.second);
        EndArray();
      EndObject();
    }



    void StartArray(const String& key)
    {
      writer->Key(key);
      writer->StartArray();
    }
    void StarArray() { writer->StartArray(); }
    void EndArray() { writer->EndArray(); }

    void StartObject(const String& key = "")
    {
      if (!key.empty()) writer->Key(key);
      writer->StartObject();
    }
    void EndObject() { writer->EndObject(); }


    void Comment(const String& message, const String& extent = "")
    {
      writer->StartObject();

      AddEntry("type", "comment");
      AddEntry("message", message);
      if (currentLevel != 0) { writer->Key("level"); writer->Int(currentLevel); }
      if (extent != "") { AddEntry("extent", extent); }

      writer->EndObject();
    }

    int GetCurrentLevel() const { return currentLevel; }

    // Specifications
    void Add(bool value)          { writer->Bool(value); }
    void Add(double value)        { writer->Double(value); }
    void Add(char value)          { writer->String(String(1, value)); }
    void Add(int value)           { writer->Int(value); }
    void Add(int64_t value)       { writer->Int64(value); }
    void Add(const String& value) { writer->String(value); }
    void Add(unsigned value)      { writer->Uint(value); }
    void Add(uint64_t value)      { writer->Uint64(value); }

  private:
    int currentLevel;
    std::unique_ptr<Stream> stream; // Stream wrapper
    std::unique_ptr<Writer> writer; // Writer used to fill the stream
  };
}

#endif // MODULE_LOGGER_LOGGER_HXX
