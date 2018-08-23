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
#include <Logger/array.hxx>
#include <Logger/comment.hxx>
#include <Logger/operation.hxx>
#include <Logger/typedef.hxx>
#include <Logger/value.hxx>

// STD includes
#include <algorithm>
#include <memory>
#include <random>
#include <set>
#include <vector>

namespace HUL_Logger
{
  class Cell {
  public:
    Cell() : x(0), y(0), isVisited(false) {}
    Cell(uint8_t x, uint8_t y, bool isVisited) : x(x), y(y), isVisited(isVisited) {}

    uint8_t GetX() const { return this->x; }
    uint8_t GetY() const { return this->y; }

    // @todo this information should not be part the cell info itself
    static const std::shared_ptr<Cell>& Visite(const std::shared_ptr<Cell>& cell)
    {
      cell->isVisited = true;
      return cell;
    }
    bool IsVisited() const { return this->isVisited; }

    // Add a connection
    //
    void AddCellConnection(std::shared_ptr<Cell> cell)
    { connectedCells.push_back(cell); }

    // @todo this information should not be part the cell info itself
    int GetRootDistance() const { return this->rootDistance; }
    void SetRootDistance(const int distance) { this->rootDistance = distance; }

  private:
    uint8_t x;
    uint8_t y;

    bool isVisited;   // @todo cf. above
    int rootDistance; // @todo cf. above

    std::vector<std::weak_ptr<Cell>> connectedCells;
  };

  /// @class MazePrimsLog
  ///
  class MazePrimsLog
  {
    public:
      static const String GetName() { return "Prims Maze Generator"; }

      // Assert correct JSON construction.
      ~MazePrimsLog() { assert(this->writer->IsComplete()); }

      /// Instantiate a new json writer using the stream passed as
      /// argument, run and write algorithm computation information.
      ///
      /// @return stream reference filled up with MazePrimsLog object information,
      ///         error object information in case of failure.
      static Ostream& Build(Ostream& os, const uint8_t width, const uint8_t height,
                            const std::pair<uint8_t, uint8_t>& startCell = std::make_pair(0,0),
                            const uint8_t seed = 0)
      {
        std::unique_ptr<MazePrimsLog> builder = std::unique_ptr<MazePrimsLog>(new MazePrimsLog(os));
        builder->Write(width, height, startCell, seed);

        return os;
      }

      /// Use json writer passed as parameter to write iterator information.
      ///
      /// @return stream reference filled up with MazePrimsLog object information,
      ///         error information in case of failure.
      static Writer& Build(Writer& writer, const uint8_t width, const uint8_t height,
                           const std::pair<uint8_t, uint8_t>& startCell = std::make_pair(0,0),
                           const uint8_t seed = 0)
      {
        Write(writer, width, height, startCell, seed);

        return writer;
      }

    private:
      MazePrimsLog(Ostream& os) : stream(std::unique_ptr<Stream>(new Stream(os))),
                             writer(std::unique_ptr<Writer>(new Writer(*this->stream))) {}
      MazePrimsLog operator=(MazePrimsLog&) {} // Not Implemented

      bool Write(const uint8_t width, const uint8_t height,
                 const std::pair<uint8_t, uint8_t>& startCell, const uint8_t seed)
      { return Write(*this->writer, width, height, startCell, seed); }

      static bool Write(Writer& writer, const uint8_t width, const uint8_t height,
                        const std::pair<uint8_t, uint8_t>& startCell, const uint8_t seed)
      {
        // Do not compute if data incorrect
        if (width < 1 || height < 1)
        {
          Comment::Build(writer, "Sequence size too small to be processed.", 0);
          Operation::Return<bool>(writer, true);
          return true;
        }

        // Do not compute if incorrect starting cell
        if (startCell.first >= width || startCell.second >= height)
        {
          Comment::Build(writer, "Starting cell out of the maze: abort.", 0);
          Operation::Return<bool>(writer, true);
          return true;
        }

        writer.StartObject();

        // Write description
        writer.Key("type");
        writer.String("DataStructure");
        writer.Key("structure");
        writer.StartObject();
          writer.Key("type");
          writer.String("2DGrid");
          writer.Key("width");
          writer.Int(width);
          writer.Key("height");
          writer.Int(height);
        writer.EndObject();
        writer.Key("name");
        writer.String(GetName());
        WriteParameters(writer, width, height, startCell, seed);   // Write parameters
        WriteComputation(writer, width, height, startCell, seed);  // Write computation

        writer.EndObject();

        return true;
      }

      ///
      static bool WriteParameters(Writer& writer, const uint8_t width, const uint8_t height,
                                  const std::pair<uint8_t, uint8_t>& startCell, const uint8_t seed)
      {
        writer.Key("parameters");
        writer.StartArray();
          Value<uint8_t>::Build(writer, "width", width);
          Value<uint8_t>::Build(writer, "height", height);
          Value<uint8_t>::Build(writer, "seed", seed);
          writer.StartObject();
          writer.Key("type");
          writer.String("GridPosition");
          writer.Key("name");
          writer.String("startCell");
          writer.Key("data");
            writer.StartArray();
              writer.Int(startCell.first);
              writer.Int(startCell.second);
            writer.EndArray();
         writer.EndObject();
        writer.EndArray();

        return true;
      }

      ///
      static bool WriteComputation(Writer& writer, const uint8_t width, const uint8_t height,
                                   const std::pair<uint8_t, uint8_t>& startCell, const uint8_t seed)
      {
        // Init Matrix
        std::vector<std::vector<std::shared_ptr<Cell>>> mazeMatrix;
        mazeMatrix.resize(width);
        for (uint8_t x = 0; x < width; ++x)
        {
          mazeMatrix[x].reserve(height);
          for (uint8_t y = 0; y < height; ++y)
            mazeMatrix[x].push_back(std::shared_ptr<Cell>(new Cell(x, y, false)));
        }

        /// LOG LOCALS
        writer.Key("locals");
        writer.StartArray();
          /// LOG STACK
          writer.StartObject();
            writer.Key("name");
            writer.String("pathSet");
            writer.Key("type");
            writer.String("set");
            writer.Key("dataType");
            writer.String("Cell");
            writer.Key("cells");
            writer.StartArray();
              writer.StartArray();
                writer.Int(startCell.first);
                writer.Int(startCell.second);
              writer.EndArray();
            writer.EndArray();
          writer.EndObject();
        writer.EndArray();

        // While there is node to be handled
        writer.Key("logs");
        writer.StartArray();

        // @todo use starting cell
        std::set<std::shared_ptr<Cell>> pathSet;
        mazeMatrix[startCell.first][startCell.second]->SetRootDistance(0);
        pathSet.insert(Cell::Visite(mazeMatrix[startCell.first][startCell.second]));

        Comment::Build(writer, "Initialize random generator based on Mersenne Twister with seed: " +
                       std::to_string(seed));
        std::mt19937 mt(seed);

        /// LOG SELECT
        writer.StartObject();
          writer.Key("type");
          writer.String("operation");
          writer.Key("name");
          writer.String("SelectCell");
          writer.Key("cell");
          writer.StartArray();
            writer.Int(startCell.first);
            writer.Int(startCell.second);
          writer.EndArray();
          writer.Key("rootDistance");
          writer.Int(0);
        writer.EndObject();

        int maxDistance = 0;
        int maxSetSize = 0;
        Comment::Build(writer, "While the set of node is not empty: ");
        Comment::Build(writer, "Randomly select a cell and randomly connect it to the maze.", 1);
        Comment::Build(writer, "Add all its unvisted neighbours to the set.", 1);
        while (!pathSet.empty())
        {
          // Select randomly a path to extend
          auto curCellIt = pathSet.begin();
          std::advance(curCellIt, mt() % pathSet.size());
          Cell::Visite(*curCellIt);

          // Randomly connect it to a previous visited cell - neighboor already visited
          auto curNeighboursPath = GetAvailableNeighbours(mazeMatrix, *(*curCellIt).get(), true);
          if (!curNeighboursPath.empty())
          {
            auto randIdx = mt() % curNeighboursPath.size();
            (*curCellIt)->SetRootDistance(curNeighboursPath[randIdx]->GetRootDistance() + 1);
            maxDistance = std::max(maxDistance, (*curCellIt)->GetRootDistance());

            /// LOG SELECT
            writer.StartObject();
              writer.Key("type");
              writer.String("operation");
              writer.Key("name");
              writer.String("SelectCell");
              writer.Key("cell");
              writer.StartArray();
                writer.Int((*curCellIt)->GetX());
                writer.Int((*curCellIt)->GetY());
              writer.EndArray();
              writer.Key("rootDistance");
              writer.Int((*curCellIt)->GetRootDistance());
            writer.EndObject();

            // Connect them
            (*curCellIt)->AddCellConnection(curNeighboursPath[randIdx]);
            curNeighboursPath[randIdx]->AddCellConnection(*curCellIt);

            /// LOG CONNECT
            writer.StartObject();
              writer.Key("type");
              writer.String("operation");
              writer.Key("name");
              writer.String("ConnectCells");
              writer.Key("cells");
              writer.StartArray();
                writer.StartArray();
                  writer.Int((*curCellIt)->GetX());
                  writer.Int((*curCellIt)->GetY());
                writer.EndArray();
                writer.StartArray();
                  writer.Int(curNeighboursPath[randIdx]->GetX());
                  writer.Int(curNeighboursPath[randIdx]->GetY());
                writer.EndArray();
              writer.EndArray();
            writer.EndObject();
          }

          // Get available neighbours - not already in the maze and add them to the set
          auto curNeighbours = GetAvailableNeighbours(mazeMatrix, *(*curCellIt).get(), false);
          for (uint8_t i = 0; i < curNeighbours.size(); ++i)
          {
            auto insertion = pathSet.insert(curNeighbours[i]);

            /// LOG PUSH
            if (insertion.second == true)
            {
              writer.StartObject();
                writer.Key("type");
                writer.String("operation");
                writer.Key("name");
                writer.String("Push");
                writer.Key("ref");
                writer.String("pathSet");
                writer.Key("cell");
                writer.StartArray();
                  writer.Int(curNeighbours[i]->GetX());
                  writer.Int(curNeighbours[i]->GetY());
                writer.EndArray();
              writer.EndObject();
            }
          }

          // Remove current cell from the processing set
          maxSetSize = std::max(maxSetSize, static_cast<int>(pathSet.size()));
          pathSet.erase(curCellIt);

          /// LOG POP
          writer.StartObject();
            writer.Key("type");
            writer.String("operation");
            writer.Key("name");
            writer.String("Pop");
            writer.Key("ref");
            writer.String("pathSet");
          writer.EndObject();
        }

        Operation::Return<bool>(writer, true);
        writer.EndArray();

        // Add Statistical informations
        writer.Key("stats");
        writer.StartObject();
          // Max Distance
          writer.Key("maxDistance");
          writer.Int(maxDistance);

          // Memory
          writer.Key("memory");
          writer.StartObject();
            writer.Key("type");
            writer.String("set");
            writer.Key("object");
            writer.String("cell");
            writer.Key("maxSize");
            writer.Int(maxSetSize);
            writer.Key("initialSize");
            writer.Int(1);
          writer.EndObject();
        writer.EndObject();

        return true;
      }


      ///
      /// @return neighbours that has not been visited yet.
      static std::vector<std::shared_ptr<Cell>> GetAvailableNeighbours(
            const std::vector<std::vector<std::shared_ptr<Cell>>>& mazeMatrix,
            const Cell& cell,
            const bool visited)
      {
        std::vector<std::shared_ptr<Cell>> neighbour;

        const auto curX = cell.GetX();
        const auto curY = cell.GetY();

        // Push left if available
        if (static_cast<int>(curX - 1) >= 0 &&
            ((visited) ? mazeMatrix[curX - 1][curY]->IsVisited() : !mazeMatrix[curX - 1][curY]->IsVisited()))
          neighbour.push_back(mazeMatrix[curX - 1][curY]);
        // Push bottom if available
        if (static_cast<int>(curY - 1) >= 0 &&
            ((visited) ? mazeMatrix[curX][curY - 1]->IsVisited() : !mazeMatrix[curX][curY - 1]->IsVisited()))
          neighbour.push_back(mazeMatrix[curX][curY - 1]);
        // Push top if available
        if (curX + 1 < mazeMatrix.size() &&
            ((visited) ? mazeMatrix[curX + 1][curY]->IsVisited() : !mazeMatrix[curX + 1][curY]->IsVisited()))
          neighbour.push_back(mazeMatrix[curX + 1][curY]);
        // Push right if available
        if (curY + 1 < mazeMatrix[curX].size() &&
            ((visited) ? mazeMatrix[curX][curY + 1]->IsVisited() : !mazeMatrix[curX][curY + 1]->IsVisited()))
          neighbour.push_back(mazeMatrix[curX][curY + 1]);

        return neighbour;
      }

      std::unique_ptr<Stream> stream; // Stream wrapper
      std::unique_ptr<Writer> writer; // Writer used to fill the stream
  };
}

#endif // MODULE_DS_MAZE_PRIMS_LOG_HXX
