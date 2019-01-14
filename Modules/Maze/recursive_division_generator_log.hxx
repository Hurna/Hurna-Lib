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
#ifndef MODULE_DS_MAZE_RECURSIVE_DIVISION_LOG_HXX
#define MODULE_DS_MAZE_RECURSIVE_DIVISION_LOG_HXX

#include <Logger/algorithm.hxx>
#include <Logger/grid.hxx>

// STD includes
#include <memory>
#include <random>

namespace hul
{
  namespace maze
  {
  class RecursiveDivisionGenerator
  {
  public:
    typedef Grid<CellInfoBase> Maze;
    typedef Maze::Cell Cell;
    typedef Maze::Point Point;

    static const String GetName() { return "Recursive Division Maze Generator"; }
    static const String GetType() { return "algorithm"; }
    static const String GetVersion() { return "2.0.0"; }

    ///
    static std::unique_ptr<Maze> Build
      (Ostream& os, const uint8_t width, const uint8_t height, const uint8_t seed = 0)
    {
      auto builder = std::unique_ptr<RecursiveDivisionGenerator>(new RecursiveDivisionGenerator(os));
      return builder->Write(width, height, seed);
    }

    ///
    static std::unique_ptr<Maze> Build
      (std::shared_ptr<Logger> logger, const uint8_t width, const uint8_t height, const uint8_t seed = 0)
    { return Write(logger, width, height, seed); }

  private:
    RecursiveDivisionGenerator(Ostream& os) : logger(std::shared_ptr<Logger>(new Logger(os))) {}
    RecursiveDivisionGenerator operator=(RecursiveDivisionGenerator&); // Not Implemented

    ///
    std::unique_ptr<Maze> Write(const uint8_t width, const uint8_t height, const uint8_t seed)
    { return Write(this->logger, width, height, seed); }

    ///
    static std::unique_ptr<Maze> Write
      (std::shared_ptr<Logger> logger, const uint8_t width, const uint8_t height, const uint8_t seed)
    {
      logger->Start();                        // Start Logging Procedure

      Algo_Traits<RecursiveDivisionGenerator>::Build(*logger.get());     // Write description
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
        logger.Add("isWallBuilding", true);
      logger.EndObject();
    }

    ///
    static void WriteParameters(Logger& logger, const uint8_t width, const uint8_t height, const uint8_t seed)
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

      auto maze(std::unique_ptr<Maze>(new Maze(logger, width, height, true))); // Initialize Maze
      std::mt19937 mt(seed);  // Initialize random generator (Mersenne Twister)

      logger->StartArray("logs");
        Compute(mt, *maze, Point(0,0), width, height);
      logger->EndArray();

      return maze;
    }

    static void Compute
      (std::mt19937& mt, Maze& maze, const Point& origin, const uint8_t width, uint8_t height)
    {
      if (width < 2 || height < 2)
        return;

      // Build a wall within the room, whether vertical or horizontal, at a random position
      // Open a gate within the built wall
      bool isHorizontalCut = (mt() % 2 == 0) ? true : false; //(width <= height);
      uint8_t wallIdx = (isHorizontalCut) ? mt() % (height - 1) : mt() % (width - 1);
      uint8_t pathIdx = (isHorizontalCut) ? mt() % width : mt() % height;

      // Build wall and Recurse on sub areas (Top and Bottom)
      if (isHorizontalCut)
      {
        maze.DisconnectRow(origin, wallIdx, width, pathIdx);
        Compute(mt, maze, origin, width, wallIdx + 1);
        Compute(mt, maze, Point(origin.x, origin.y + wallIdx + 1),  width, height - wallIdx - 1);
      }
      // Recurse on sub areas (Left and Right)
      else
      {
        maze.DisconnectCol(origin, wallIdx, height, pathIdx);
        Compute(mt, maze, origin, wallIdx + 1, height);
        Compute(mt, maze, Point(origin.x + wallIdx + 1, origin.y),  width - wallIdx - 1, height);
      }
    }

    std::shared_ptr<Logger> logger; // logger used to fill the stream
  };
  }
}

#endif // MODULE_DS_MAZE_RECURSIVE_DIVISION_LOG_HXX
