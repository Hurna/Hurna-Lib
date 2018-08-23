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
#ifndef MODULE_LOGGER_VECTOR_HXX
#define MODULE_LOGGER_VECTOR_HXX

#include <Logger/logger.hxx>

// STD includes
#include <iterator>
#include <memory>
#include <vector>

namespace hul
{
  /// @class Vector
  /// Wrap std::vector adding log operations writing and statisticals informations
  ///
  template <typename T>
  class Vector
  {
    public:
    static const String GetType() { return "array"; }
    static const String GetVersion() { return "1.0.0"; }

    // Vector and Vector::h_iterator Statistics
    struct Stats
    {
      Stats() : nbAccess(0), nbCompares(0), nbIterations(0), nbItCopy(0), nbSwaps(0) {}
      int nbAccess;
      int nbCompares;
      int nbIterations;
      int nbItCopy;
      int nbSwaps;
    };

    explicit Vector(Ostream& os, std::initializer_list<T> init = {}) :
      logger(std::shared_ptr<Logger>(new Logger(os))),
      data(init) {}

    explicit Vector(std::shared_ptr<Logger> logger, std::initializer_list<T> init = {}) :
      logger(logger),
      data(init) {}

    explicit Vector(std::shared_ptr<Logger> logger, const std::vector<T>& vector) :
      logger(logger),
      data(vector) {}

      class h_iterator : public std::iterator<std::random_access_iterator_tag, T>
      {
        public:
          ///
          /// \brief h_iterator
          /// \param it
          /// \param owner
          /// \param index
          /// \param name
          ///
          h_iterator(typename std::vector<T>::iterator it,
                     Vector<T>* owner,
                     const int index,
                     const std::string& name) :
            it(it), owner(owner), index(index), name(name), comment(""), logOperations(false), stats() {}

          ///
          /// \brief h_iterator
          /// \param it
          /// \param name
          /// \param logOperations
          ///
          h_iterator(const h_iterator& it, const std::string& name="", bool logOperations = false) : it(it.it)
          {
            // Owner and index
            this->owner = it.owner;
            this->index = it.index;

            // Not copied fields
            this->name = name;
            this->logOperations = logOperations;

            // Log
            if (logOperations) this->Log();
          }

          ///
          /// \brief operator *
          /// \return
          ///
          T& operator *() const
          {
            // Stats
            ++this->stats.nbAccess;
            if (this->logOperations) this->owner->AddAccess();

            return *this->it;
          }

          ///
          /// \brief operator ++
          /// \return
          ///
          const h_iterator &operator ++()
          {
            // Increment
            ++this->it;
            ++this->index;

            // Stats & Log
            ++this->stats.nbIterations;
            if (this->logOperations)
            {
              this->owner->AddIteration();
              this->LogNewIndex();
            }

            return *this;
          }

          ///
          /// \brief operator ++
          /// \return
          ///
          h_iterator operator ++(int)
          {
            // Copy and Increment
            h_iterator copy(*this);
            ++this->it;
            ++this->index;

            // Stats & Log
            ++this->stats.nbIterations;
            if (this->logOperations)
            {
              this->owner->AddIteration();
              this->LogNewIndex();
            }

            return copy;
          }

          ///
          /// \brief operator --
          /// \return
          ///
          const h_iterator &operator --()
          {
            // Increment
            --this->it;
            --this->index;

            // Stats & Log
            ++this->stats.nbIterations;
            if (this->logOperations)
            {
              this->owner->AddIteration();
              this->LogNewIndex();
            }

            return *this;
          }

          ///
          /// \brief operator --
          /// \return
          ///
          h_iterator operator --(int)
          {
            // Copy and Increment
            h_iterator copy(*this);
            --this->it;
            --this->index;

            // Stats & Log
            ++this->stats.nbIterations;
            if (this->logOperations)
            {
              this->owner->AddIteration();
              this->LogNewIndex();
            }

            return copy;
          }

          ///
          /// \brief operator =
          /// \param other
          /// \return
          ///
          h_iterator& operator =(const h_iterator& other)
          {
            // Copy
            this->it = other.it;
            this->index = other.index;
            this->owner = other.owner;

            // Stats & Log
            if (this->logOperations)
            {
              this->owner->AddItCopy();
              this->LogNewIndex();
            }

            return *this;
          }

          ///
          /// \brief operator +
          /// \param delta
          /// \return
          ///
          h_iterator operator +(const long int& delta) const
          {
            // Copy
            h_iterator copy(*this);
            copy.it += delta;
            copy.index += delta;

            // Stats & Log
            ++this->stats.nbIterations;
            if (this->logOperations) this->owner->AddIteration();

            return copy;
          }

          ///
          /// \brief operator +=
          /// \param delta
          /// \return
          ///
          h_iterator& operator +=(const long int& delta)
          {
            // Increment
            this->it += delta;
            this->index += delta;

            // Stats & Log
            ++this->stats.nbIterations;
            if (this->logOperations)
            {
              this->owner->AddIteration();
              this->LogNewIndex();
            }

            return *this;
          }

          ///
          /// \brief operator -
          /// \param delta
          /// \return
          ///
          h_iterator operator -(const long int& delta) const
          {
            // Copy
            h_iterator copy(*this);
            copy.it -= delta;
            copy.index -= delta;

            // Stats & Log
            ++this->stats.nbIterations;
            if (this->logOperations) this->owner->AddIteration();

            return copy;
          }

          ///
          /// \brief operator -=
          /// \param delta
          /// \return
          ///
          h_iterator& operator -=(const long int& delta)
          {
            // Decrement
            this->it -= delta;
            this->index -= delta;

            // Stats & Log
            ++this->stats.nbIterations;
            if (this->logOperations)
            {
              this->owner->AddIteration();
              this->LogNewIndex();
            }

            return *this;
          }

          // Do not involve logging operation
          long int operator -(const h_iterator& delta) const { return it - delta.it; } // std::distance
          bool operator ==(const h_iterator &other) const { return it == other.it; }
          bool operator !=(const h_iterator &other) const { return it != other.it; }
          bool operator  <(const h_iterator &other) const { return it  < other.it; }
          bool operator  >(const h_iterator &other) const {  return it  > other.it; }
          bool operator <=(const h_iterator &other) const { return it <= other.it; }
          bool operator >=(const h_iterator &other) const { return it >= other.it; }
          T operator [](const long int& delta) const { return *(it + delta); }


          // Accessor
          int GetIndex() const { return index; }
          int GetName() const { return name; }

          // Owner Access
          std::string GetOwnerRef() const { return owner->GetRef(); }
          std::string GetOwnerType() const { return Vector::GetType(); }


          // Log itself
          void Log(bool isConst=false) const
          {
            auto logger = this->owner->GetLogger();
            logger->StartObject();

            // Add iterator information
            logger->AddEntry("type", "iterator");
            logger->AddEntry("name", this->name);
            logger->AddEntry("ref", this->GetOwnerRef());
            logger->AddEntry("data", this->index);
            if (isConst) logger->AddEntry("const", 1);
            if (!this->comment.empty()) logger->AddEntry("comment", comment);

            logger->EndObject();
          }

          void LogNewIndex() const
          {
            if (!this->logOperations)
              return;

            auto logger = this->owner->GetLogger();
            logger->StartObject();

            // Add iterator information
            logger->AddEntry("type", "operation");
            logger->AddEntry("name", "Set");
            logger->AddEntry("ref", this->name);
            logger->AddEntry("data", this->index);

            logger->EndObject();
          }

          void LogOwnerStats() const { this->owner->LogStats(); }

          void LogStats() const
          {
            auto logger = this->owner->GetLogger();

            // StartBuild - Main information
            logger->StartObject();
            logger->AddEntry("type", "iterator");
            logger->AddEntry("name", this->name);
            logger->AddEntry("ref", this->GetOwnerRef());
            if (!this->comment.empty()) logger->AddEntry("comment", this->comment);

            // Stats
            logger->AddEntry("nbAccess", this->stats.nbAccess);
            logger->AddEntry("nbCompares", this->stats.nbCompares);
            logger->AddEntry("nbIterations", this->stats.nbIterations);
            //logger->AddEntry("nbItCopy", this->stats.nbItCopy);
            logger->AddEntry("nbSwaps", this->stats.nbSwaps);

            // Finish object
            logger->EndObject();
          }

          void LogOperations(bool logOperations) { this->logOperations = logOperations; }

          std::string String() const { return name + "[" + ToString(index) + "]{" + ToString(*it) + "}"; }


          // Statistics
          void AddCompare(bool propagateOwner) const
          {
            ++this->stats.nbCompares;
            if (propagateOwner) this->owner->AddCompare();
          }


          void AddSwap(bool propagateOwner) const
          {
            ++this->stats.nbSwaps;
            if (propagateOwner) this->owner->AddSwap();
          }

        private:
          typename std::vector<T>::iterator it;  // Iterator
          Vector<T>* owner;                      // Cannot be null as long as the iterator is valid
          int index;                             // Iterator position
          std::string name;                      // Iterator name
          std::string comment;                   // Add a comment to the variable (may be useful as caption)
          bool logOperations;                    // Whether or not log occuring operation (default to false)

          // Stats
          mutable Stats stats;
      };

      // Preserve normal iterator accesses
      typename std::vector<T>::iterator begin() { return this->data.begin(); }
      typename std::vector<T>::iterator end() { return this->data.end(); }

      const typename std::vector<T>::iterator cbegin() const { return this->data.cbegin(); }
      const typename std::vector<T>::iterator cend() const { return this->data.cend(); }

      void push_back (const T& val) { this->data.push_back(val); }
      void reserve (size_t n) { this->data.reserve(n); }
      const size_t size() const { return this->data.size(); }


      // Observale iterator
      h_iterator h_begin() { return h_iterator(data.begin(), this, static_cast<int>(0), "begin"); }
      h_iterator h_end() { return h_iterator(data.end(), this, static_cast<int>(data.size()), "end"); }

      ///
      /// \brief Log
      ///
      void Log() const
      {
        // StartBuild - Main information
        logger->StartObject();
          logger->AddEntry("type", GetType());
          logger->AddEntry("name", this->GetRef());

          // Add data
          logger->StartArray("data");
            for (auto it = begin; it != end; ++it) logger->Add(*it);
          logger->EndArray();

        // Finish object
        logger->EndObject();
      }

      ///
      /// \brief LogStats
      ///
      void LogStats() const
      {
        // StartBuild - Main information
        logger->StartObject();
          logger->AddEntry("type", GetType());
          logger->AddEntry("name", this->GetRef());

          // Stats
          logger->AddEntry("nbAccess", stats.nbAccess);
          logger->AddEntry("nbCompares", stats.nbCompares);
          logger->AddEntry("nbIterations", stats.nbIterations);
          logger->AddEntry("nbItCopy", this->stats.nbItCopy);
          logger->AddEntry("nbSwaps", stats.nbSwaps);

        // Finish object
        logger->EndObject();
      }

      void AddAccess() const { ++stats.nbAccess; }
      void AddCompare() const { ++stats.nbCompares; }
      void AddIteration() const { ++stats.nbIterations; }
      void AddItCopy() const { ++stats.nbItCopy; }
      void AddSwap() const { ++stats.nbSwaps; }

      // Accessors
      std::shared_ptr<Logger> GetLogger() { return logger; }
      std::string GetRef() const { return "sequence"; } /// @todo *this to string

    private:
      Vector operator=(Vector&) {}    // Not Implemented
      std::shared_ptr<Logger> logger; // Logger
      std::vector<T> data;            // Vector wrapper
      mutable Stats stats;            // Computation statistics
  };
}
#endif // MODULE_LOGGER_VECTOR_HXX
