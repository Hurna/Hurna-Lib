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
#ifndef MODULE_LOGGER_SET_HXX
#define MODULE_LOGGER_SET_HXX

#include <Logger/logger.hxx>

// STD includes
#include <memory>
#include <set>

namespace hul
{
  /// @class Set
  /// Wrap std::Set adding log operations writing and statisticals informations
  ///
  /// @todo templating operations for pointers and value
  template <typename T>
  class Set
  {
  public:
    static const String GetName() { return "Set"; }
    static const String GetType() { return "data_structure"; }
    static const String GetVersion() { return "1.0.0"; }

    // Statistics
    struct Stats
    {
      Stats() : maxSize(0), nbAccess(0), nbErase(0), nbInsert(0), nbSwap(0) {}
      int maxSize;    // Keep maximal Set size during the process
      int nbAccess;
      int nbErase;
      int nbInsert;
      int nbSwap;
    };

    explicit Set(Ostream& os, const String& name = "") :
      logger(std::shared_ptr<Logger>(new Logger(os))), name(name), id(ToString(static_cast<void*>(this))) {}

    explicit Set(std::shared_ptr<Logger> logger, const String& name = "") :
      logger(logger), name(name), id(ToString(static_cast<void*>(this))) {}

    void clear() noexcept {
      this->data.clear();

      logger->StartObject();
        logger->AddEntry("type", "operation");
        logger->AddEntry("name", "Clear");
        logger->AddEntry("ref", this->id);
      logger->EndObject();
    }

    bool empty() const { return this->data.empty(); }

    typename std::set<T>::iterator erase(const typename std::set<T>::iterator it)
    {
      auto nextIt = this->data.erase(it);
      ++this->stats.nbErase;

      logger->StartObject();
        logger->AddEntry("type", "operation");
        logger->AddEntry("name", "Erase");
        logger->AddEntry("ref", this->id);
        logger->AddEntry("id", (*it)->id);
      logger->EndObject();

      return nextIt;
    }

    T top() const { ++this->stats.nbAccess; return this->data.top(); }

    typename std::pair<typename std::set<T>::iterator, bool> insert(const T& value, bool mute = false)
    {
      auto insertion = this->data.insert(value);
      ++this->stats.nbInsert;
      this->stats.maxSize = std::max(this->stats.maxSize, static_cast<int>(this->data.size()));

      if (!mute && insertion.second == true)
      {
        logger->StartObject();
          logger->AddEntry("type", "operation");
          logger->AddEntry("name", "Insert");
          logger->AddEntry("ref", this->id);
          logger->AddEntry("id", value->id);
        logger->EndObject();
      }

      return insertion;
    }

    template <class InputIterator> void insert(InputIterator first, InputIterator last, bool mute = false)
    {
      if (!mute)
      {
        logger->StartObject();
          logger->AddEntry("type", "operation");
          logger->AddEntry("name", "Insert");
          logger->AddEntry("ref", this->id);
          logger->StartArray("ids");
            for (auto it = first; it != last; ++it)
              if (this->data.find(*it) == this->data.end()) // Log only real insertion
                logger->Add((*it)->id);
          logger->EndArray();
        logger->EndObject();
      }

      this->data.insert(first, last);
      this->stats.nbInsert += std::distance(first, last);
      this->stats.maxSize = std::max(this->stats.maxSize, static_cast<int>(this->data.size()));
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
        logger->AddEntry("nbErase", this->stats.nbErase);
        logger->AddEntry("nbInsert", this->stats.nbInsert);
        logger->AddEntry("nbSwaps", this->stats.nbSwap);

      // Finish object
      logger->EndObject();
    }

    // Accessors
    int GetId() const { return this->id; }

    // Preserve normal iterator accesses
    typename std::set<T>::iterator begin() { return this->data.begin(); }
    typename std::set<T>::iterator end() { return this->data.end(); }
    typename std::set<T>::iterator rbegin() { return this->data.rbegin(); }
    typename std::set<T>::iterator rend() { return this->data.rend(); }
    const typename std::set<T>::iterator cbegin() const { return this->data.cbegin(); }
    const typename std::set<T>::iterator cend() const { return this->data.cend(); }
    const typename std::set<T>::iterator crbegin() const { return this->data.crbegin(); }
    const typename std::set<T>::iterator crend() const { return this->data.crend(); }

  private:
    Set operator=(Set&);            // Not Implemented
    std::shared_ptr<Logger> logger; // Logger
    std::set<T> data;               // Set wrapper
    const String name;              // Identifiant name
    const String id;                // Internal identifiant (memory address)
    mutable Stats stats;            // Computation statistics
  };
}
#endif // MODULE_LOGGER_SET_HXX
