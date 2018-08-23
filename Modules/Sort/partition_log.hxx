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
#ifndef MODULE_SORT_PARTITION_LOG_HXX
#define MODULE_SORT_PARTITION_LOG_HXX

#include <Logger/algorithm.hxx>
#include <Logger/command.hxx>
#include <Logger/vector.hxx>

namespace hul
{
  namespace sort
  {
  /// @class Partition
  ///
  template <typename IT, typename Compare = std::less<typename std::iterator_traits<IT>::value_type>>
  class Partition
  {
  // Specification to get h_iterator (if normal iterator subBuild bubble using h_iterator as template)
  typedef CompareWrap<IT, Compare> CompareF;

  public:
    static const String GetName() { return "Partition"; }
    static const String GetVersion() { return "1.0.0"; }
    static const String GetType() { return "algorithm"; }

    /// Instantiate a new json writer using the stream passed as
    /// argument, run and write algorithm computation information.
    ///
    /// @return stream reference filled up with Partition object information,
    ///         error object information in case of failure.
    static Ostream& Build(Ostream& os, const IT& begin, const IT& pivot, const IT& end)
    {
      auto builder = std::unique_ptr<Partition>(new Partition(os));
      builder->Write(begin, pivot, end);

      return os;
    }

    /// Instantiate a new json writer using the stream passed as
    /// argument, run and write algorithm computation information.
    ///
    /// @return true in case of success, false otherwise.
    //void operator()(Logger& logger, const IT& begin, const IT& end)
    static IT Build(Logger& logger, const IT& begin, const IT& pivot, const IT& end)
    { return Write(logger, begin, pivot, end); }

  private:
    Partition(Ostream& os) : logger(std::unique_ptr<Logger>(new Logger(os))) {}
    Partition operator=(Partition&) {} // Not Implemented


    IT Write(const IT& begin, const IT& pivot, const IT& end)
    { return Write(*this->logger, begin, pivot, end); }


    ///
    static IT Write(Logger& logger, const IT& begin, const IT& pivot, const IT& end)
    {
      logger.Start();                        // Start Logging Procedure

      Algo_Traits<Partition>::Build(logger);                  // Write description
      WriteParameters(logger, begin, pivot, end);             // Write parameters
      auto it = WriteComputation(logger, begin, pivot, end);  // Write computation

      logger.End();                          // Close Logging Procedure

      return it;
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


    static IT WriteComputation(Logger& logger, const IT& begin, const IT& pivot, const IT& end)
    {
      if (std::distance(begin, pivot) < 1 && std::distance(pivot, end) < 1)
      {
        if (logger.GetCurrentLevel() == 0)
        {
          logger.Comment("Sequence too small to be procesed: already partitionned.");
          logger.Return(pivot.String());
        }
        return pivot;
      }

      // Locals
      logger.StartArray("locals");
        auto curIt = IT(begin, "current", true);  // Put the current pointer at the beginning
        auto storeIt = IT(curIt, "store", true);  // Put the store pointer at the beginning
        auto lastIt = IT(end - 1, "last", true);  // Take the last element iterator
      logger.EndArray();


      logger.StartArray("logs");
      logger.Comment("keep the pivot value and put the pivot itself at the end for convenience.");
      if (pivot.GetIndex() != lastIt.GetIndex())
        Swap()(logger, pivot, lastIt);

      logger.StartLoop("Put each value <= pivot on the left side of the store pointer:");
      for (; curIt != lastIt; ++curIt)
        if (CompareF()(curIt, lastIt))
        {
          logger.Comment(curIt.String() + " < " + lastIt.String() +
                         " : swap it with the store and increment store pointer.");
          Swap()(logger, curIt, storeIt);
          ++storeIt;
        }
        else { logger.Comment(curIt.String() + " >= " + lastIt.String() + " : Ignore elements."); }
      logger.EndLoop();

      logger.Comment("Replace the pivot at its right position");
      if (storeIt.GetIndex() != lastIt.GetIndex())
        Swap()(logger, storeIt, lastIt);
      else
        storeIt = lastIt;

      logger.Return(storeIt.String());
      logger.EndArray();

      // Statistics
      if (logger.GetCurrentLevel() == 0)
      {
        logger.StartArray("stats");
          logger.AddStats(curIt, true);
          logger.AddStats(storeIt);
          logger.AddStats(lastIt);
        logger.EndArray();
      }

      return storeIt;
    }

    // Unique as created only at execution as a RAII ressource
    std::unique_ptr<Logger> logger; // Logger used to fill the stream
  };
  }
}

#endif // MODULE_SORT_PARTITION_LOG_HXX
