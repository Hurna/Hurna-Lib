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
#ifndef MODULE_SORT_AGGREGATE_IN_PLACE_LOG_HXX
#define MODULE_SORT_AGGREGATE_IN_PLACE_LOG_HXX

#include <Logger/algorithm.hxx>
#include <Logger/command.hxx>
#include <Logger/vector.hxx>

namespace hul
{
  namespace sort
  {
  /// @class AggregateInPlace
  ///
  template <typename IT, typename Compare = std::less_equal<typename std::iterator_traits<IT>::value_type>>
  class AggregateInPlace
  {
  // Specification to get h_iterator (if normal iterator subBuild bubble using h_iterator as template)
  typedef CompareWrap<IT, Compare> CompareF;

  public:
    static const String GetName() { return "Aggregate In Place"; }
    static const String GetVersion() { return "1.0.0"; }
    static const String GetType() { return "algorithm"; }

    /// Instantiate a new json writer using the stream passed as
    /// argument, run and write algorithm computation information.
    ///
    /// @return stream reference filled up with AggregateInPlace object information,
    ///         error object information in case of failure.
    static Ostream& Build(Ostream& os, const IT& begin, const IT& pivot, const IT& end)
    {
      auto builder = std::unique_ptr<AggregateInPlace>(new AggregateInPlace(os));
      builder->Write(begin, pivot, end);

      return os;
    }

    /// Instantiate a new json writer using the stream passed as
    /// argument, run and write algorithm computation information.
    ///
    /// @return true in case of success, false otherwise.
    //void operator()(Logger& logger, const IT& begin, const IT& end)
    static void Build(Logger& logger, const IT& begin, const IT& pivot, const IT& end)
    { Write(logger, begin, pivot, end); }

  private:
    AggregateInPlace(Ostream& os) : logger(std::unique_ptr<Logger>(new Logger(os))) {}
    AggregateInPlace operator=(AggregateInPlace&) {} // Not Implemented


    bool Write(const IT& begin, const IT& pivot, const IT& end)
    { return Write(*this->logger, begin, pivot, end); }


    ///
    static void Write(Logger& logger, const IT& begin, const IT& pivot, const IT& end)
    {
      logger.Start();                        // Start Logging Procedure

      Algo_Traits<AggregateInPlace>::Build(logger); // Write description
      WriteParameters(logger, begin, pivot, end);   // Write parameters
      WriteComputation(logger, begin, pivot, end);  // Write computation

      logger.End();                          // Close Logging Procedure
    }


    static void WriteParameters(Logger& logger, const IT& begin, const IT& pivot, const IT& end)
    {
      logger.StartArray("parameters");
      if (logger.GetCurrentLevel() > 0)           // Write only iterators
      {
        logger.AddObject(begin, true);
        logger.AddObject(pivot, true);
        logger.AddObject(end, true);
      }
      else // Write all data
      {
        logger.AddDataDetails(begin, end, true);
        logger.AddObject(pivot, true);
      }
      logger.EndArray();
    }


    static void WriteComputation(Logger& logger, const IT& begin, const IT& pivot, const IT& end)
    {
      if (std::distance(begin, pivot) < 1 || std::distance(pivot, end) < 1)
      {
        if (logger.GetCurrentLevel() == 0)
        {
          logger.Comment("Sequence too small to be procesed: already sorted.");
          logger.Return("void");
        }
        return;
      }

      // Locals
      logger.StartArray("locals");
        auto firstIt = IT(begin, "firstIt", true);
        auto secondIt = IT(pivot, "secondIt", true);
        auto secondItNext = IT(secondIt, "secondIt_next", true);
      logger.EndArray();

      // Use first half as receiver
      logger.StartArray("logs");
      logger.StartLoop(String("Swap, if greater, first part element with the pivot. ") +
                       String("Bubble up then the new pivot value at its right position:"));
      for(; firstIt < pivot; ++firstIt)
      {
        if (CompareF()(firstIt, pivot)) {
          logger.Comment(firstIt.String() + " <= " + pivot.String() + " : Ignore element.");
          continue;
        }

        logger.Comment(firstIt.String() + " <= " + pivot.String() + " : Swap firstIt with pivot.");
        Swap()(logger, firstIt, pivot);


        logger.StartLoop("Displace new pivot value in the right place by bubbling up:");
        secondIt = pivot;
        for (secondItNext = secondIt + 1; secondIt != end - 1; ++secondIt, ++secondItNext)
        {
          if (CompareF()(secondIt, secondItNext))
          {
            logger.Comment(secondIt.String() + " <= " + secondItNext.String() +
                           " : Element at its right place, break.");
            break;
          }

          logger.Comment(secondIt.String() + " > " + secondItNext.String() + " : Bubble up.");
          Swap()(logger, secondIt, secondItNext);
        }
        logger.EndLoop();
      }
      logger.EndLoop();

      logger.Return("void");
      logger.EndArray();

      // Statistics
      if (logger.GetCurrentLevel() == 0)
      {
        logger.StartArray("stats");
          logger.AddStats(firstIt, true);
          logger.AddStats(secondIt);
          logger.AddStats(secondItNext);
        logger.EndArray();
      }
    }

    // Unique as created only at execution as a RAII ressource
    std::unique_ptr<Logger> logger; // Logger used to fill the stream
  };
  }
}

#endif // MODULE_SORT_AGGREGATE_IN_PLACE_LOG_HXX
