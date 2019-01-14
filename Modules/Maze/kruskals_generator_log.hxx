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
#ifndef MODULE_DS_MAZE_KRUSKALS_LOG_HXX
#define MODULE_DS_MAZE_KRUSKALS_LOG_HXX

#include <Logger/algorithm.hxx>
#include <Logger/grid.hxx>
#include <Logger/set.hxx>

// STD includes
#include <memory>
#include <list>
#include <random>

namespace hul
{
  namespace maze
  {
  class KruskalsGenerator
  {
  public:
    struct CellInfoBucket : CellInfoBase
    { uint32_t bucketId; };  // Precision should be above than 2 * (uint8_t * uint8_t)

    typedef Grid<CellInfoBucket> Maze;
    typedef Maze::Cell Cell;
    typedef Maze::Edge Edge;
    typedef Maze::Point Point;

    static const String GetName() { return "Kruskals Maze Generator"; }
    static const String GetType() { return "algorithm"; }
    static const String GetVersion() { return "2.0.0"; }

    ///
    static std::unique_ptr<Maze> Build
      (Ostream& os, const uint8_t width, const uint8_t height, const uint8_t seed = 0)
    {
      auto builder = std::unique_ptr<KruskalsGenerator>(new KruskalsGenerator(os));
      return builder->Write(width, height, seed);
    }

    ///
    static std::unique_ptr<Maze> Build
      (std::shared_ptr<Logger> logger, const uint8_t width, const uint8_t height, const uint8_t seed = 0)
    { return Write(logger, width, height, seed); }

  private:
    KruskalsGenerator(Ostream& os) : logger(std::shared_ptr<Logger>(new Logger(os))) {}
    KruskalsGenerator operator=(KruskalsGenerator&); // Not Implemented

    ///
    std::unique_ptr<Maze>
      Write(const uint8_t width, const uint8_t height, const uint8_t seed)
    { return Write(this->logger, width, height, seed); }

    ///
    static std::unique_ptr<Maze> Write
      (std::shared_ptr<Logger> logger, const uint8_t width, const uint8_t height, const uint8_t seed)
    {
      logger->Start();                        // Start Logging Procedure

      Algo_Traits<KruskalsGenerator>::Build(*logger.get());       // Write description
      WriteInfo(*logger.get());                                   // Write Info
      WriteParameters(*logger.get(), width, height, seed);        // Write parameters
      auto maze = WriteComputation(logger, width, height, seed);  // Write computation
      maze->Log("output");                                        // Write outputs

      logger->End();                          // Close Logging Procedure

      return maze;
    }

    ///
    static void WriteInfo(Logger& logger)
    {
      logger.StartObject("info");
        logger.Add("isWallBuilding", false);
      logger.EndObject();
    }

    ///
    static void WriteParameters
      (Logger& logger, const uint8_t width, const uint8_t height, const uint8_t seed)
    {
      logger.StartArray("parameters");
        logger.AddValue("width", width);
        logger.AddValue("height", height);
        logger.AddValue("seed", seed);
      logger.EndArray();
    }

    ///
    static std::unique_ptr<Maze> WriteComputation
      (std::shared_ptr<Logger> logger, const uint8_t width, const uint8_t height, const uint8_t seed)
    {
      if (width < 1 || height < 1)
      {
        logger->Comment("Invalid Parameters: sequence size too small to be processed: abort.");
        return nullptr;
      }

      auto maze(std::unique_ptr<Maze>(new Maze(logger, width, height))); // Initialize Maze
      std::mt19937 mt(seed);                     // Initialize random generator (Mersenne Twister)
      Set<std::shared_ptr<Edge>> edges(logger);  // Big sac with all possible edges
      std::vector<std::vector<std::shared_ptr<Cell>>> bucketCells; // List of all buckets

      // Fill big sac of edges and set the bucket ids to each cell
      uint32_t nodeId = 0;
      bucketCells.resize(height * width); // (height - 1) * width + (width - 1) * height);
      for (auto x = 0; x < maze->Width(); ++x)
        for (auto y = 0; y < maze->Height(); ++y, ++nodeId)
        {
          (*maze)[x][y]->info.bucketId = nodeId;
          bucketCells[nodeId].push_back((*maze)[x][y]);

          // Insert Right edge
          if (x + 1 < maze->Width())
            edges.insert(std::make_shared<Edge>((*maze)[x][y], (*maze)[x+1][y]), true);
          // Insert Bottom edge
          if (y + 1 < maze->Height())
            edges.insert(std::make_shared<Edge>((*maze)[x][y], (*maze)[x][y+1]), true);
        }

      logger->StartArray("locals");
        edges.LogStats();
      logger->EndArray();

      // Start computation
      logger->StartArray("logs");

      // Compute each edge
      logger->Comment("Create buckets for each cell and a set with all possible connecting edges.");
      logger->StartLoop("While the set of edges is not empty randomly get an edge (connecting two cells):");
      logger->Comment("If cells are not part of the same bucket; connect them and merge their buckets.");
      while (!edges.empty())
      {
        auto edgeIt = edges.begin();
        std::advance(edgeIt, mt() % edges.size());
        maze->SelectEdge((*edgeIt)->first, (*edgeIt)->second);

        // Convenient variables
        const auto firstBucket = (*edgeIt)->first->info.bucketId;
        const auto secondBucket = (*edgeIt)->second->info.bucketId;

        // Add connection and merge buckets if not already in the same one
        if (firstBucket != secondBucket)
        {
          maze->Connect((*edgeIt)->first, (*edgeIt)->second);
          MergeBucket(bucketCells, firstBucket, secondBucket);
        }

        // Remove computed cell from the set
        edges.erase(edgeIt);
      }

      logger->EndLoop();
      logger->EndArray();

      // Add Statistical informations
      logger->StartObject("stats");
        edges.LogStats("memory");
      logger->EndObject();

      return maze;
    }

    /// Merge two buckets of node together and update node bucket Id.
    static void MergeBucket(std::vector<std::vector<std::shared_ptr<Cell>>>& buckets,
                            uint32_t bucketIdA, uint32_t bucketIdB)
    {
      assert(bucketIdA != bucketIdB);

      // Set new bucket id for each melding cell
      for (auto it = buckets[bucketIdB].begin(); it != buckets[bucketIdB].end(); ++it)
      {
        (*it)->info.bucketId = bucketIdA;
        buckets[bucketIdA].push_back(*it);
      }

      buckets[bucketIdB].clear();
    }

    std::shared_ptr<Logger> logger; // logger used to fill the stream
  };
  }
}

#endif // MODULE_DS_MAZE_KRUSKALS_LOG_HXX
