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
#ifndef MODULE_SEARCH_BINARY_LOG_HXX
#define MODULE_SEARCH_BINARY_LOG_HXX

#include <Logger/algorithm.hxx>
#include <Logger/vector.hxx>

namespace hul
{
  namespace search
  {
  /// @class Binary
  ///
  template <typename IT, typename Equal = std::equal_to<typename std::iterator_traits<IT>::value_type>>
  class Binary
  {
  typedef typename std::iterator_traits<IT>::value_type T;

  public:
    static const String GetName() { return "Binary Search"; }
    static const String GetVersion() { return "1.0.0"; }
    static const String GetType() { return "algorithm"; }

    ///
    static Ostream& Build(Ostream& os, const IT& begin, const IT& end, const T& key)
    {
      auto builder = std::unique_ptr<Binary>(new Binary(os));
      builder->Write(begin, end, key);

      return os;
    }

    ///
    static IT Build(Logger& logger, const IT& begin, const IT& end, const T& key)
    { return Write(logger, begin, end, key); }

  private:
    Binary(Ostream& os) : logger(std::unique_ptr<Logger>(new Logger(os))) {}
    Binary operator=(Binary&) {} // Not Implemented

    IT Write(const IT& begin, const IT& end, const T& key) { return Write(*this->logger, begin, end, key); }

    ///
    static IT Write(Logger& logger, const IT& begin, const IT& end, const T& key)
    {
      logger.Start();                        // Start Logging Procedure

      Algo_Traits<Binary>::Build(logger);         // Write description
      WriteParameters(logger, begin, end, key);   // Write parameters
      auto it = WriteComputation(logger, begin, end, key);  // Write computation

      logger.End();                          // Close Logging Procedure

      return it;
    }

    ///
    static void WriteParameters(Logger& logger, const IT& begin, const IT& end, const T& key)
    {
      logger.StartArray("parameters");
      if (logger.GetCurrentLevel() > 0) // Only iterators
      {
        logger.AddObject(begin, true);
        logger.AddObject(end, true);
        logger.AddValue("key", key);
      }
      else                              // All data
      {
        logger.AddDataDetails(begin, end, true);
        logger.AddValue("key", key);
      }
      logger.EndArray();
    }

    ///
    static IT WriteComputation(Logger& logger, const IT& begin, const IT& end, const T& key)
    {
      const auto size = static_cast<const int>(std::distance(begin, end));
      if (size < 2)
      {
        if (logger.GetCurrentLevel() == 0)
        {
          logger.Comment("Sequence too small to be procesed: already sorted.");
          logger.Return("void");
        }
        return end;
      }

      // Locals
      logger.StartArray("locals");
        bool found = false;
        auto lowIt = begin;
        auto highIt = end;
        auto curIt = IT(end, "current", true);
      logger.EndArray();

      // Computation
      logger.StartArray("logs");
      logger.StartLoop();
      while (lowIt < highIt)
      {
        curIt = lowIt + (highIt.GetIndex() - lowIt.GetIndex()) / 2;
        logger.Comment("Select middle element: " + curIt.String());

        if (Equal()(key, *curIt))
        {
          found = true;
          logger.Comment("Key {" + ToString(key) + "} Found at index [" + ToString(curIt.GetIndex()) + "]");
          break;
        }
        else if (key > *curIt)
        {
          lowIt = curIt + 1;
          logger.Comment("Key{" + ToString(key) + "} > " + curIt.String() + ": search in upper sequence.");
        }
        else
        {
          highIt = curIt;
          logger.Comment("Key{" + ToString(key) + "} < " + curIt.String() + ": search in lower sequence.");
        }

        // Notify new search space
        logger.SetRange(std::make_pair(lowIt.GetIndex(), highIt.GetIndex()));
      }
      logger.EndLoop();

      if (!found) logger.Comment("Key {" + ToString(key) + "} was not found.");
      logger.Return((found) ? curIt.String() : end.String());
      logger.EndArray();

      // Statistics
      if (logger.GetCurrentLevel() == 0)
      {
        logger.StartArray("stats");
          logger.AddStats(curIt, true);
        logger.EndArray();
      }

      return (found) ? curIt : end;
    }

    // Unique as created only at execution as a RAII ressource
    std::unique_ptr<Logger> logger; // Logger used to fill the stream
  };
  }
}

#endif // MODULE_SEARCH_BINARY_LOG_HXX
