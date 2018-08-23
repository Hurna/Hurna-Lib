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
#ifndef MODULE_SORT_BUBBLE_LOG_HXX
#define MODULE_SORT_BUBBLE_LOG_HXX

#include <Logger/algorithm.hxx>
#include <Logger/command.hxx>
#include <Logger/vector.hxx>

namespace hul
{
  namespace sort
  {
  /// @class Bubble
  ///
  ///
  ///
  template <typename IT, typename Compare = std::greater<typename std::iterator_traits<IT>::value_type>>
  class Bubble
  {
  // Specification to get h_iterator (if normal iterator subBuild bubble using h_iterator as template)
  typedef CompareWrap<IT, Compare> CompareF;

  public:
    static const String GetName() { return "Bubble Sort"; }
    static const String GetVersion() { return "1.0.0"; }
    static const String GetType() { return "algorithm"; }

    // @TODO both specialization for hurna iterator that log themself otherwise create temporary
    // hurna vector to run computation with logging capability (allows to log from classic iterators)

    /// Instantiate a new json writer using the stream passed as
    /// argument, run and write algorithm computation information.
    ///
    /// @return stream reference filled up with AggregateInPlaceLog object information,
    ///         error object information in case of failure.
    static Ostream& Build(Ostream& os, const IT& begin, const IT& end)
    {
      auto builder = std::unique_ptr<Bubble>(new Bubble(os));
      builder->Write(begin, end);

      return os;
    }

    ///
    /// \brief Build
    /// \param logger
    /// \param begin
    /// \param end
    ///
    /// //void operator()(Logger& logger, const IT& begin, const IT& end)
    static void Build(Logger& logger, const IT& begin, const IT& end)
    { Write(logger, begin, end); }

  private:
    Bubble(Ostream& os) : logger(std::unique_ptr<Logger>(new Logger(os))) {}
    Bubble operator=(Bubble&) {} // Not Implemented

    void Write(const IT& begin, const IT& end) { Write(*this->logger, begin, end); }

    ///
    static void Write(Logger& logger, const IT& begin, const IT& end)
    {
      logger.Start();                        // Start Logging Procedure

      Algo_Traits<Bubble>::Build(logger);    // Write description
      WriteParameters(logger, begin, end);   // Write parameters
      WriteComputation(logger, begin, end);  // Write computation

      logger.End();                          // Close Logging Procedure
    }

    ///
    static void WriteParameters(Logger& logger, const IT& begin, const IT& end)
    {
      logger.StartArray("parameters");
      if (logger.GetCurrentLevel() > 0) // Only iterators
      {
        logger.AddObject(begin, true);
        logger.AddObject(end, true);
      }
      else { logger.AddDataDetails(begin, end, true); } // All data
      logger.EndArray();
    }

    ///
    static void WriteComputation(Logger& logger, const IT& begin, const IT& end)
    {
      const auto size = static_cast<const int>(std::distance(begin, end));
      if (size < 2)
      {
        logger.Comment("Sequence too small to be procesed: already sorted.");
        logger.Return("void");
        return;
      }

      // Locals
      logger.StartArray("locals");
        int endIdx = -1;
        bool hasSwapped;
        auto curIt = IT(begin, "current", true);
        auto nextIt = IT(curIt + 1, "next", true);
      logger.EndArray();

      // Computation
      logger.StartArray("logs");
      logger.StartLoop("Bubble biggest element at the end and restart to [end-i] until sorted:");
      for (auto it = begin; it < end - 1; ++it, --endIdx)
      {
        hasSwapped = false;

        // Notify new search space
        auto range = std::make_pair(0, size + endIdx);
        logger.SetRange(range);
        logger.StartLoop("Bubble up biggest value within " + RangeToString(range) + ":");
        for (curIt = begin, nextIt = curIt + 1; curIt < end + endIdx; ++curIt, ++nextIt)
        {
          if (CompareF()(curIt, nextIt))
          {
            logger.Comment(curIt.String() + " > " + nextIt.String() + " : Bubble up.");
            Swap()(logger, curIt, nextIt);
            hasSwapped = true;
          }
          else { logger.Comment(curIt.String() + " <= " + nextIt.String() + " : Ignore element."); }
        }
        logger.EndLoop();

        if (!hasSwapped)
        {
          logger.Comment("No swap occured: sequence is sorted.");
          break;
        }
      }
      logger.EndLoop();

      logger.Return("void");
      logger.EndArray();

      // Statistics
      if (logger.GetCurrentLevel() == 0)
      {
        logger.StartArray("stats");
          logger.AddStats(curIt, true);
          logger.AddStats(nextIt);
        logger.EndArray();
      }
    }

    // Unique as created only at execution as a RAII ressource
    std::unique_ptr<Logger> logger;
  };
  }
}

#endif // MODULE_SORT_BUBBLE_LOG_HXX
