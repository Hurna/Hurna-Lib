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
#ifndef MODULE_DS_MAZE_SIDEWINDER_LOG_HXX
#define MODULE_DS_MAZE_SIDEWINDER_LOG_HXX

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
  class SidewinderGenerator
  {
  public:
    typedef Grid<CellInfoBase> Maze;
    typedef Maze::Cell Cell;

    static const String GetName() { return "Sidewinder Maze Generator"; }
    static const String GetType() { return "algorithm"; }
    static const String GetVersion() { return "2.0.0"; }

    ///
    static std::unique_ptr<Maze> Build
      (Ostream& os, const uint8_t width, const uint8_t height, const uint8_t seed = 0)
    {
      auto builder = std::unique_ptr<SidewinderGenerator>(new SidewinderGenerator(os));
      return builder->Write(width, height, seed);
    }

    ///
    static std::unique_ptr<Maze> Build
      (std::shared_ptr<Logger> logger, const uint8_t width, const uint8_t height, const uint8_t seed = 0)
    { return Write(logger, width, height, seed); }

  private:
    SidewinderGenerator(Ostream& os) : logger(std::shared_ptr<Logger>(new Logger(os))) {}
    SidewinderGenerator operator=(SidewinderGenerator&); // Not Implemented

    ///
    std::unique_ptr<Maze> Write(const uint8_t width, const uint8_t height, const uint8_t seed)
    { return Write(this->logger, width, height, seed); }

    ///
    static std::unique_ptr<Maze> Write
      (std::shared_ptr<Logger> logger, const uint8_t width, const uint8_t height, const uint8_t seed)
    {
      logger->Start();                        // Start Logging Procedure

      Algo_Traits<SidewinderGenerator>::Build(*logger.get());     // Write description
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

      auto maze(std::unique_ptr<Maze>(new Maze(logger, width, height))); // Initialize Maze
      std::mt19937 mt(seed);                      // Initialize random generator (Mersenne Twister)
      Set<std::shared_ptr<Cell>> runSet(logger);  // Keep track of possible paths to expand

      logger->StartArray("locals");
        runSet.Log();
      logger->EndArray();

      // Start computation
      logger->StartArray("logs");
      logger->Comment("Initialize an empty “run” set to keep track of the current line path.");
      logger->Comment("Scan grid line by line starting with the cell[0,0]: ");
      logger->Comment("Add current cell to the path and randomly carve east or not.");
      logger->Comment("If a passage was carved, continue line scan.");
      logger->Comment("Otherwise randomly choose a cell in the run set and carve north, " +
             ToString("empty the run set and continue line scan."));
      for (uint8_t y = 0; y < height; ++y)
      {
        for (uint8_t x = 0; x < width; ++x)
        {
          // Add current cell to the path (avoid useless insertion)
          if (y > 0)
            runSet.insert((*maze)[x][y]);

          // Randomly carve to east or north
          // First row can only be a single passage, crave it to the east
          if (x + 1 < width && (mt() % 2 == 0 || y == 0))
          {
            maze->Connect((*maze)[x][y], (*maze)[x+1][y]);
          }
          else if (y > 0)
          {
            auto cellIt = runSet.begin();
            std::advance(cellIt, mt() % runSet.size());

            maze->Connect(*cellIt, (*maze)[(*cellIt)->x][(*cellIt)->y - 1]);

            runSet.clear();
          }
        }

        runSet.clear();
      }

      logger->EndArray();

      // Add Statistical informations
      logger->StartObject("stats");
        runSet.LogStats("memory");
      logger->EndObject();

      return maze;
    }

    std::shared_ptr<Logger> logger; // logger used to fill the stream
  };
  }
}

#endif // MODULE_DS_MAZE_SIDEWINDER_LOG_HXX
