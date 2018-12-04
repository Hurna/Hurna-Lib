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
#ifndef MODULE_LOGGER_STACK_HXX
#define MODULE_LOGGER_STACK_HXX

#include <Logger/logger.hxx>

// STD includes
#include <memory>
#include <stack>

namespace hul
{
  /// @class Stack
  /// Wrap std::Stack adding log operations writing and statisticals informations
  ///
  /// @todo templating operations for pointers and value
  template <typename T>
  class Stack
  {
  public:
    static const String GetName() { return "stack"; }
    static const String GetType() { return "data_structure"; }
    static const String GetVersion() { return "1.0.0"; }

    // Stack and Stack::h_iterator Statistics
    struct Stats
    {
      Stats() : maxSize(0), nbAccess(0), nbPop(0), nbPush(0), nbSwap(0) {}
      int maxSize;    // Keep maximal stack size during the process
      int nbAccess;
      int nbPop;
      int nbPush;
      int nbSwap;
    };

    explicit Stack(Ostream& os, const String& name = "") :
      logger(std::shared_ptr<Logger>(new Logger(os))), name(name), id(ToString(static_cast<void*>(this))) {}

    explicit Stack(std::shared_ptr<Logger> logger, const String& name = "") :
      logger(logger), name(name), id(ToString(static_cast<void*>(this))) {}

    // Function wrapping
    bool empty() const { return this->data.empty(); }

    void pop()
    {
      this->data.pop();
      ++this->stats.nbPop;

      logger->StartObject();
        logger->AddEntry("type", "operation");
        logger->AddEntry("name", "Pop");
        logger->AddEntry("ref", this->id);
      logger->EndObject();
    }

    T top() const { ++this->stats.nbAccess; return this->data.top(); }

    void push(const T& value)
    {
      this->data.push(value);
      ++this->stats.nbPush;
      this->stats.maxSize = std::max(this->stats.maxSize, static_cast<int>(this->data.size()));

      logger->StartObject();
        logger->AddEntry("type", "operation");
        logger->AddEntry("name", "Push");
        logger->AddEntry("ref", this->id);
        logger->AddEntry("id", value->id);
      logger->EndObject();
    }

    size_t size() const { return this->data.size(); }

    ///
    /// \brief Log
    ///
    void Log(const String& name = "") const
    {
      logger->StartObject(name);
        logger->AddEntry("type", GetType());
        logger->AddEntry("dataType", T::element_type::GetName());
        logger->AddEntry("name", GetName());
        logger->AddEntry("alias", this->name);
        logger->AddEntry("id", this->id);
        logger->StartArray("data");
        logger->EndArray();
      logger->EndObject();
    }

    ///
    /// \brief LogStats
    ///
    void LogStats(const String& name = "") const
    {
      // StartBuild - Main information
      logger->StartObject(name);
        logger->AddEntry("type", GetType());
        logger->AddEntry("dataType", T::element_type::GetName());
        logger->AddEntry("name", GetName());
        logger->AddEntry("alias", this->name);
        logger->AddEntry("id", this->id);

        // Stats
        logger->AddEntry("maxSize", this->stats.maxSize);
        logger->AddEntry("nbAccess", this->stats.nbAccess);
        logger->AddEntry("nbPop", this->stats.nbPop);
        logger->AddEntry("nbPush", this->stats.nbPush);
        logger->AddEntry("nbSwaps", this->stats.nbSwap);

      // Finish object
      logger->EndObject();
    }

    // Accessors
    int GetId() const { return this->id; }

  private:
    Stack operator=(Stack&);        // Not Implemented
    std::shared_ptr<Logger> logger; // Logger
    std::stack<T> data;             // Stack wrapper
    const String name;              // Identifiant name
    const String id;               // Internal identifiant (memory address)
    mutable Stats stats;            // Computation statistics
  };
}
#endif // MODULE_LOGGER_Stack_HXX
