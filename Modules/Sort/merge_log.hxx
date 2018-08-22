/*===========================================================================================================
 *
 * SHA - Simple Hybesis Algorithms
 *
 * Copyright (c) Michael Jeulin-Lagarrigue
 *
 *  Licensed under the MIT License, you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://github.com/michael-jeulinl/Simple-Hybesis-Algorithms/blob/master/LICENSE
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations under the License.
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 *=========================================================================================================*/
#ifndef MODULE_SORT_MERGE_LOG_HXX
#define MODULE_SORT_MERGE_LOG_HXX

#include <aggregate_in_place_log.hxx>

namespace hul
{
  namespace sort
  {
  /// @class Merge
  ///
  template <typename IT,
            typename Compare = std::less_equal<typename std::iterator_traits<IT>::value_type>,
            typename Aggregator = AggregateInPlace<IT, Compare>>
  class Merge
  {
  public:
    static const String GetName() { return "Merge Sort"; }
    static const String GetVersion() { return "1.0.0"; }
    static const String GetType() { return "algorithm"; }

    /// Instantiate a new json writer using the stream passed as
    /// argument, run and write algorithm computation information.
    ///
    /// @return stream reference filled up with Merge object information,
    ///         error object information in case of failure.
    static Ostream& Build(Ostream& os, const IT& begin, const IT& end)
    {
      std::unique_ptr<Merge> builder = std::unique_ptr<Merge>(new Merge(os));
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
    Merge(Ostream& os) : logger(std::unique_ptr<Logger>(new Logger(os))) {}
    Merge operator=(Merge&) {} // Not Implemented

    void Write(const IT& begin, const IT& end) { Write(*this->logger, begin, end); }

    ///
    static void Write(Logger& logger, const IT& begin, const IT& end)
    {
      logger.Start();                        // Start Logging Procedure

      Algo_Traits<Merge>::Build(logger);    // Write description
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
        const auto middle = IT(begin + size / 2, "middle", true);
      logger.EndArray();


      logger.StartArray("logs");
      Merge::Build(logger, begin, IT(middle, "end"));
      Merge::Build(logger, IT(middle, "begin"), end);

      // Merge the two pieces
      Aggregator::Build(logger, begin, middle, end);

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

#endif // MODULE_SORT_MERGE_HXX
