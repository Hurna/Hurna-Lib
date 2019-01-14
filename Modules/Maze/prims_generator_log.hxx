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
#ifndef MODULE_DS_MAZE_PRIMS_LOG_HXX
#define MODULE_DS_MAZE_PRIMS_LOG_HXX

#include <Logger/algorithm.hxx>
#include <Logger/grid.hxx>
#include <Logger/set.hxx>

// STD includes
#include <memory>
#include <random>

namespace hul
{
  namespace maze
  {
  class PrimsGenerator
  {
  public:
    typedef Grid<CellInfoBase> Maze;
    typedef Maze::Cell Cell;
    typedef Maze::Point Point;

    static const String GetName() { return "Prims Maze Generator"; }
    static const String GetType() { return "algorithm"; }
    static const String GetVersion() { return "2.0.0"; }

    ///
    static std::unique_ptr<Maze> Build(Ostream& os, const uint8_t width, const uint8_t height,
                                        const Point& startPoint = Point(0,0), const uint8_t seed = 0)
    {
      auto builder = std::unique_ptr<PrimsGenerator>(new PrimsGenerator(os));
      return builder->Write(width, height, startPoint, seed);
    }

    ///
    static std::unique_ptr<Maze> Build(std::shared_ptr<Logger> logger,
                                       const uint8_t width, const uint8_t height,
                                       const Point& startPoint = Point(0,0), const uint8_t seed = 0)
    { return Write(logger, width, height, startPoint, seed); }

  private:
    PrimsGenerator(Ostream& os) : logger(std::shared_ptr<Logger>(new Logger(os))) {}
    PrimsGenerator operator=(PrimsGenerator&); // Not Implemented

    ///
    std::unique_ptr<Maze>
      Write(const uint8_t width, const uint8_t height, const Point& startPoint, const uint8_t seed)
    { return Write(this->logger, width, height, startPoint, seed); }

    ///
    static std::unique_ptr<Maze> Write(std::shared_ptr<Logger> logger,
                                       const uint8_t width, const uint8_t height,
                                       const Point& startPoint, const uint8_t seed)
    {
      logger->Start();                        // Start Logging Procedure

      Algo_Traits<PrimsGenerator>::Build(*logger.get());                      // Write description
      WriteInfo(*logger.get());                                               // Write Info
      WriteParameters(*logger.get(), width, height, startPoint, seed);        // Write parameters
      auto maze = WriteComputation(logger, width, height, startPoint, seed);  // Write computation
      maze->Log("output");                                                    // Write outputs

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
    static void WriteParameters(Logger& logger, const uint8_t width, const uint8_t height,
                                const Point& startPoint, const uint8_t seed)
    {
      logger.StartArray("parameters");
        logger.AddValue("width", width);
        logger.AddValue("height", height);
        logger.AddValue("seed", seed);
        startPoint.Log(logger, "startCell");
      logger.EndArray();
    }

    ///
    static std::unique_ptr<Maze>
      WriteComputation(std::shared_ptr<Logger> logger,
                       const uint8_t width, const uint8_t height,
                       const Point& startPoint, const uint8_t seed)
    {
      if (width < 1 || height < 1 || startPoint.x >= width || startPoint.y >= height)
      {
        logger->Comment("Invalid Parameters: sequence size too small to be processed: abort.");
        return nullptr;
      }

      auto maze(std::unique_ptr<Maze>(new Maze(logger, width, height))); // Initialize Maze
      std::mt19937 mt(seed);                      // Initialize random generator (Mersenne Twister)
      Set<std::shared_ptr<Cell>> pathSet(logger); // Keep track of possible paths to expand

      logger->StartArray("locals");
        pathSet.Log();
      logger->EndArray();

      // Start computation
      logger->StartArray("logs");
      logger->Comment("Create a set to keep track of possible cell to expand.");
      (*maze)[startPoint.x][startPoint.y]->info.rootDistance = 0;
      pathSet.insert((*maze)[startPoint.x][startPoint.y]);
      maze->Select((*maze)[startPoint.x][startPoint.y]);

      logger->StartLoop("While there is cell within the set:");
      logger->Comment("Randomly select a cell and randomly connect it to the maze.");
      logger->Comment("Then add all its unvisted neighbours to the set.");
      while (!pathSet.empty())
      {
        auto cellIt = pathSet.begin();
        std::advance(cellIt, mt() % pathSet.size());
        (*cellIt)->info.isVisited = true;

        // Randomly connect it to a cell that is already part of the mze
        auto neighbours = GetNeighbours(*maze, *(*cellIt).get(), true);
        if (!neighbours.empty())
        {
          auto randIdx = mt() % neighbours.size();
          (*cellIt)->info.rootDistance = neighbours[randIdx]->info.rootDistance + 1;
          maze->Select(*cellIt);
          maze->Connect(*cellIt, neighbours[randIdx]);
        }

        // Add available neighbours and remove current cell
        neighbours = GetNeighbours(*maze, *(*cellIt).get(), false);
        pathSet.insert(neighbours.begin(), neighbours.end());
        pathSet.erase(cellIt);
      }
      logger->EndLoop();
      logger->EndArray();

      // Add Statistical informations
      logger->StartObject("stats");
        pathSet.LogStats("memory");
      logger->EndObject();

      return maze;
    }

    ///
    /// @return neighbours that has not been visited yet.
    static std::vector<std::shared_ptr<Cell>>
      GetNeighbours(const Maze& maze, const Cell& cell, const bool visited)
    {
      std::vector<std::shared_ptr<Cell>> neighbour;

      const auto x = cell.x;
      const auto y = cell.y;

      // Push left if available
      if (static_cast<int>(x - 1) >= 0 &&
          ((visited) ? maze[x-1][y]->info.isVisited : !maze[x-1][y]->info.isVisited))
        neighbour.push_back(maze[x-1][y]);
      // Push bottom if available
      if (static_cast<int>(y - 1) >= 0 &&
          ((visited) ? maze[x][y-1]->info.isVisited : !maze[x][y-1]->info.isVisited))
        neighbour.push_back(maze[x][y-1]);
      // Push top if available
      if (x + 1 < maze.Width() &&
          ((visited) ? maze[x+1][y]->info.isVisited : !maze[x+1][y]->info.isVisited))
        neighbour.push_back(maze[x+1][y]);
      // Push right if available
      if (y + 1 < maze.Height() &&
          ((visited) ? maze[x][y+1]->info.isVisited : !maze[x][y+1]->info.isVisited))
        neighbour.push_back(maze[x][y+1]);

      return neighbour;
    }

    std::shared_ptr<Logger> logger; // logger used to fill the stream
  };
  }
}

#endif // MODULE_DS_MAZE_PRIMS_LOG_HXX
