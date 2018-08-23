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
#ifndef MODULE_SORT_KTH_ORDER_STATISTIC_LOG_HXX
#define MODULE_SORT_KTH_ORDER_STATISTIC_LOG_HXX

#include <Sort/partition_log.hxx>
#include <Sort/picker_log.hxx>

namespace hul
{
  namespace search
  {
  /// @class KthOrderStatistic
  ///
  template <typename IT,
            typename Compare = std::less<typename std::iterator_traits<IT>::value_type>,
            typename Picker = picker::First<IT>>
  class KthOrderStatistic
  {
  typedef sort::Partition<IT, Compare> PartitionT;

  public:
    static const String GetName() { return "Kth Order Statistic"; }
    static const String GetVersion() { return "1.0.0"; }
    static const String GetType() { return "algorithm"; }

    ///
    static Ostream& Build(Ostream& os, const IT& begin, const IT& end, unsigned int k)
    {
      auto builder = std::unique_ptr<KthOrderStatistic>(new KthOrderStatistic(os));
      builder->Write(begin, end, k);

      return os;
    }

    ///
    static IT Build(Logger& logger, const IT& begin, const IT& end, unsigned int k)
    { return Write(logger, begin, end, k); }

  private:
    KthOrderStatistic(Ostream& os) : logger(std::unique_ptr<Logger>(new Logger(os))) {}
    KthOrderStatistic operator=(KthOrderStatistic&) {} // Not Implemented

    void Write(const IT& begin, const IT& end, unsigned int k) { Write(*this->logger, begin, end, k); }

    ///
    static IT Write(Logger& logger, const IT& begin, const IT& end, unsigned int k)
    {
      logger.Start(); // Start Logging Procedure

      Algo_Traits<KthOrderStatistic>::Build(logger);           // Write description
      WriteParameters(logger, begin, end, k);                  // Write parameters
      auto kth_it = WriteComputation(logger, begin, end, k);   // Write computation

      logger.End(); // Close Logging Procedure

      return kth_it;
    }

    ///
    static void WriteParameters(Logger& logger, const IT& begin, const IT& end, unsigned int k)
    {
      logger.StartArray("parameters");
      if (logger.GetCurrentLevel() > 0) // Only iterators
      {
        logger.AddObject(begin, true);
        logger.AddObject(end, true);
        logger.AddValue("k", k);
      }
      else // All data
      {
        logger.AddDataDetails(begin, end, true);
        logger.AddValue("k", k);
      }
      logger.EndArray();
    }

    ///
    static IT WriteComputation(Logger& logger, const IT& begin, const IT& end, unsigned int k)
    {
      if (k >= static_cast<const int>(std::distance(begin, end)))
      {
        if (logger.GetCurrentLevel() == 0)
        {
          logger.Comment("Sequence smaller than the order statistic searched: return end.");
          logger.Return(end.String());
        }

        return end;
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
        pivot = PartitionT::Build(logger, begin, pivot, end);

        // Get the index of the pivot on the subsequence
        const auto index = std::distance(begin, pivot);
        if (index == k)
        {
          logger.Comment("The new pivot index is equal to k, K'th order statistic found: [" +
                         ToString(index) + "]{" + std::to_string(*pivot) + "}.");
        }
        else if (index > k)
        {
          logger.Comment("Index of the new pivot [" + ToString(index) + "] > k [" +
                         ToString(k) + "] : Recurse on left-side.");
          pivot = KthOrderStatistic::Build(logger, begin, IT(pivot, "end"), k);
        }
        else
        {
          logger.Comment("Index of the new pivot [" + ToString(index) + "] < k [" +
                         ToString(k) + "] : Recurse on righ-side.");
          pivot = KthOrderStatistic::Build(logger, IT(pivot + 1, "begin"), end, k - index);
        }

        logger.Return(pivot.String());
      logger.EndArray();

      // Statistics
      if (logger.GetCurrentLevel() == 0)
      {
        logger.StartArray("stats");
          logger.AddStats(begin, true);
        logger.EndArray();
      }

      return pivot;
    }

    // Unique as created only at execution as a RAII ressource
    std::unique_ptr<Logger> logger;
  };
  }
}

#endif // MODULE_SORT_KTH_ORDER_STATISTIC_LOG_HXX
