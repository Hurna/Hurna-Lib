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
#ifndef MODULE_DS_MAZE_BINARY_LOG_HXX
#define MODULE_DS_MAZE_BINARY_LOG_HXX

#include <Logger/algorithm.hxx>
#include <Logger/grid.hxx>

// STD includes
#include <memory>
#include <random>

namespace hul
{
  namespace maze
  {
  class BinaryGenerator
  {
  public:
    typedef Grid<CellInfoBase> Maze;
    typedef Maze::Cell Cell;

    static const String GetName() { return "Binary Tree Maze Generator"; }
    static const String GetType() { return "algorithm"; }
    static const String GetVersion() { return "2.0.0"; }

    ///
    static std::unique_ptr<Maze> Build(Ostream& os,
                                       const uint8_t width, const uint8_t height, const uint8_t seed = 0)
    {
      auto builder = std::unique_ptr<BinaryGenerator>(new BinaryGenerator(os));
      return builder->Write(width, height, seed);
    }

    ///
    static std::unique_ptr<Maze> Build(std::shared_ptr<Logger> logger,
                                       const uint8_t width, const uint8_t height, const uint8_t seed = 0)
    { return Write(logger, width, height, seed); }

  private:
    BinaryGenerator(Ostream& os) : logger(std::shared_ptr<Logger>(new Logger(os))) {}
    BinaryGenerator operator=(BinaryGenerator&); // Not Implemented

    ///
    std::unique_ptr<Maze>
      Write(const uint8_t width, const uint8_t height, const uint8_t seed)
    { return Write(this->logger, width, height, seed); }

    ///
    static std::unique_ptr<Maze> Write(std::shared_ptr<Logger> logger,
                                       const uint8_t width, const uint8_t height, const uint8_t seed)
    {
      logger->Start();                        // Start Logging Procedure

      Algo_Traits<BinaryGenerator>::Build(*logger.get());         // Write description
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
    static void WriteParameters(Logger& logger,
                                const uint8_t width, const uint8_t height, const uint8_t seed)
    {
      logger.StartArray("parameters");
        logger.AddValue("width", width);
        logger.AddValue("height", height);
        logger.AddValue("seed", seed);
      logger.EndArray();
    }

    ///
    static std::unique_ptr<Maze>
      WriteComputation(std::shared_ptr<Logger> logger,
                       const uint8_t width, const uint8_t height, const uint8_t seed)
    {
      if (width < 1 || height < 1)
      {
        logger->Comment("Invalid Parameters: sequence size too small to be processed: abort.");
        return nullptr;
      }

      // The maze will own the logger as well
      auto maze(std::unique_ptr<Maze>(new Maze(logger, width, height)));
      std::mt19937 mt(seed); // Initialize random generator based on Mersenne Twister algorithm

      // Start computation
      logger->StartArray("logs");
      logger->Comment("For each existing cell in the grid, randomly carve a passage either east or south :");
      for (uint8_t y = 0; y < height; ++y)
      {
        for (uint8_t x = 0; x < width; ++x)
        {
          // Get available neighbours
          auto cell = maze->Select((*maze)[x][y]);
          auto curNeighbours = GetAvailableNeighbours(*maze, *cell.get());

          if (curNeighbours.empty())
            continue;

          // Randomly select a node to be processed
          auto randIdx = mt() % curNeighbours.size();
          maze->Connect(cell, curNeighbours[randIdx]);
        }
      }

      logger->EndArray();

      // Add Statistical informations
      logger->StartObject("stats");
        maze->LogStats("memory");
      logger->EndObject();

      return maze;
    }

    ///
    /// @return North and West neighbours
    static std::vector<std::shared_ptr<Cell>> GetAvailableNeighbours(const Maze& maze, const Cell& cell)
    {
      std::vector<std::shared_ptr<Cell>> neighbours;

      const auto x = cell.x;
      const auto y = cell.y;

      // West Direction - Push left if available
      if (x > 0) neighbours.push_back(maze[x - 1][y]);
      // North Direction - Push top if available
      if (y > 0) neighbours.push_back(maze[x][y - 1]);

      return neighbours;
     }

    std::shared_ptr<Logger> logger; // logger used to fill the stream
  };
  }
}

#endif // MODULE_DS_MAZE_BINARY_LOG_HXX
