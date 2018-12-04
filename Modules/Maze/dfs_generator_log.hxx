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
#ifndef MODULE_DS_MAZE_DFS_LOG_HXX
#define MODULE_DS_MAZE_DFS_LOG_HXX

#include <Logger/algorithm.hxx>
#include <Logger/command.hxx>
#include <Logger/grid.hxx>
#include <Logger/stack.hxx>

// STD includes
#include <memory>
#include <random>
#include <stack>

namespace hul
{
  namespace maze
  {
  class DFSGenerator
  {
  public:
    typedef Grid<CellInfoBase> Maze;
    typedef Maze::Cell Cell;
    typedef Maze::Point Point;
    //typedef Stack<std::shared_ptr<Cell>> StackCellPtr;

    static const String GetName() { return "Depth First Search Maze Generator"; }
    static const String GetType() { return "algorithm"; }
    static const String GetVersion() { return "1.0.0"; }

    ///
    static std::unique_ptr<Maze> Build(Ostream& os, const uint8_t width, const uint8_t height,
                                        const Point& startPoint = Point(0,0), const uint8_t seed = 0)
    {
      auto builder = std::unique_ptr<DFSGenerator>(new DFSGenerator(os));
      return builder->Write(width, height, startPoint, seed);
    }

    ///
    static std::unique_ptr<Maze> Build(std::shared_ptr<Logger> logger,
                                        const uint8_t width, const uint8_t height,
                                        const Point& startPoint = Point(0,0), const uint8_t seed = 0)
    { return Write(logger, width, height, startPoint, seed); }

  private:
    DFSGenerator(Ostream& os) : logger(std::shared_ptr<Logger>(new Logger(os))) {}
    DFSGenerator operator=(DFSGenerator&); // Not Implemented

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

      Algo_Traits<DFSGenerator>::Build(*logger.get());                        // Write description
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

      // The maze will own the logger as well
      auto maze(std::unique_ptr<Maze>(new Maze(logger, width, height)));
      std::mt19937 mt(seed);      // Initialize random generator based on Mersenne Twister algorithm

      logger->StartArray("locals");
        Stack<std::shared_ptr<Cell>> pathStack(logger, "pathStack"); // Keep track of the cell path
        pathStack.Log();
      logger->EndArray();

      // Start computation
      logger->StartArray("logs");
      logger->Comment("Create a stack to keep track of the path and add the starting cell within.");
      (*maze)[startPoint.x][startPoint.y]->info.rootDistance = 0;
      (*maze)[startPoint.x][startPoint.y]->info.isVisited = true;
      pathStack.push((*maze)[startPoint.x][startPoint.y]);


      logger->StartLoop("While there is cell within the stack:");
      logger->Comment("Take cell at the top and retrieve all its unvisited neighboors.");
      logger->Comment("Randomly select a cell to be processed");
      logger->Comment(String("For each available cell: connect and ") +
                      "push into the stack keeping the selected one to the top.");
      while (!pathStack.empty())
      {
        // Handle the Cell at the top of the stack
        auto cell = maze->Select(pathStack.top());
        pathStack.pop();

        // Get available neighbours and connect to them
        auto neighbours = GetAvailableNeighbours(*maze, *cell.get());

        // Add them to the stack and select one to be processed next
        if (!neighbours.empty())
        {
          // Randomly select a cell to be processed
          auto randIdx = mt() % neighbours.size();

          // For each available cell we make a connection and push it into the stack of being processed
          // Only the choosen item should be add to the top following a DFS strategy
          for (uint8_t i = 0; i < neighbours.size(); ++i)
          {
            neighbours[i]->info.isVisited = true;
            neighbours[i]->info.rootDistance = cell->info.rootDistance + 1;

            if (i != randIdx) pathStack.push(neighbours[i]);
          }
          pathStack.push(neighbours[randIdx]);

          // Connect cell with all neighbours
          maze->Connect(cell, neighbours);
        }
      }
      logger->EndLoop();
      logger->Return<bool>(true);
      logger->EndArray();

      // Add Statistical informations
      logger->StartObject("stats");
        pathStack.LogStats("memory");
      logger->EndObject();

      return maze;
    }

    ///
    /// @return neighbours that has not been visited yet.
    static std::vector<std::shared_ptr<Cell>> GetAvailableNeighbours(const Maze& maze, const Cell& cell)
    {
      std::vector<std::shared_ptr<Cell>> neighbours;
      neighbours.reserve(4);

      const auto x = cell.x;
      const auto y = cell.y;

      // Push left if available
      if (static_cast<int>(x - 1) >= 0 && !maze[x-1][y]->info.isVisited)
        neighbours.push_back(maze[x-1][y]);

      // Push bottom if available
      if (static_cast<int>(y - 1) >= 0 && !maze[x][y-1]->info.isVisited)
        neighbours.push_back(maze[x][y-1]);

      // Push top if available
      if (x + 1 < maze.Width() && !maze[x+1][y]->info.isVisited)
        neighbours.push_back(maze[x+1][y]);

      // Push right if available
      if (y + 1 < maze.Height() && !maze[x][y+1]->info.isVisited)
        neighbours.push_back(maze[x][y+1]);

      return neighbours;
    }

    std::shared_ptr<Logger> logger; // logger used to fill the stream
  };
  }
}

#endif // MODULE_DS_MAZE_DFS_LOG_HXX
