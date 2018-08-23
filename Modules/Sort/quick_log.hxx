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
#ifndef MODULE_SORT_QUICK_LOG_HXX
#define MODULE_SORT_QUICK_LOG_HXX

#include <partition_log.hxx>
#include <picker_log.hxx>

namespace hul
{
  namespace sort
  {
  /// @class Quick
  ///
  template <typename IT,
            typename Compare = std::less<typename std::iterator_traits<IT>::value_type>,
            typename Picker = picker::ThreeMedian<IT>>
  class Quick
  {
  typedef Partition<IT, Compare> PartitionT;

  public:
    static const String GetName() { return "Quick Sort"; }
    static const String GetVersion() { return "1.0.0"; }
    static const String GetType() { return "algorithm"; }

    ///
    static Ostream& Build(Ostream& os, const IT& begin, const IT& end)
    {
      auto builder = std::unique_ptr<Quick>(new Quick(os));
      builder->Write(begin, end);

      return os;
    }

    ///
    static void Build(Logger& logger, const IT& begin, const IT& end) { Write(logger, begin, end); }

  private:
    Quick(Ostream& os) : logger(std::unique_ptr<Logger>(new Logger(os))) {}
    Quick operator=(Quick&) {} // Not Implemented

    void Write(const IT& begin, const IT& end) { Write(*this->logger, begin, end); }

    ///
    static void Write(Logger& logger, const IT& begin, const IT& end)
    {
      logger.Start();                        // Start Logging Procedure

      Algo_Traits<Quick>::Build(logger);     // Write description
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
        if (logger.GetCurrentLevel() == 0)
        {
          logger.Comment("Sequence too small to be procesed: already sorted.");
          logger.Return("void");
        }
        return;
      }

      // Locals
      logger.StartArray("locals");
        const auto pick = Picker()(begin, end);
        auto pivot = IT(pick.first, "pivot", true);
      logger.EndArray();

      // Computation
      logger.StartArray("logs");
        logger.Comment(pick.second);
        logger.Comment("Proceed partition");
        auto newPivot = PartitionT::Build(logger, begin, pivot, end);

        logger.Comment("Recurse on left-side from partition");
        Quick::Build(logger, begin, IT(newPivot, "end"));
        logger.Comment("Recurse on right-side from partition");
        Quick::Build(logger, IT(newPivot + 1, "begin"), end);

        logger.Return("void");
      logger.EndArray();

      // Statistics
      if (logger.GetCurrentLevel() == 0)
      {
        logger.StartArray("stats");
          logger.AddStats(begin, true);
        logger.EndArray();
      }
    }

    // Unique as created only at execution as a RAII ressource
    std::unique_ptr<Logger> logger;
  };
  }
}

#endif // MODULE_SORT_QUICK_HXX
