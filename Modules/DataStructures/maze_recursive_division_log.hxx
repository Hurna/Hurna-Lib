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
#include <Logger/array.hxx>
#include <Logger/comment.hxx>
#include <Logger/operation.hxx>
#include <Logger/typedef.hxx>
#include <Logger/value.hxx>

// STD includes
#include <chrono>
#include <memory>
#include <queue>
#include <random>
#include <set>
#include <vector>

#include <iostream>

namespace HUL_Logger
{
  class Cell {
  public:
    Cell() : x(0), y(0), rootDistance(0), isVisited(false) {}
    Cell(uint8_t x, uint8_t y) : x(x), y(y), isVisited(false) {}

    uint8_t GetX() const { return this->x; }
    uint8_t GetY() const { return this->y; }

    // Add a connection
    //
    void AddConnection(std::shared_ptr<Cell> cell)
    { this->connectedCells.insert(cell); }
    std::set<std::weak_ptr<Cell>>& GetConnections()
    { return this->connectedCells; }
    void DisconnectCell(const std::shared_ptr<Cell>& cell)
    { this->connectedCells.erase(cell); }

    // @todo this information should not be part the cell info itself
    static const std::shared_ptr<Cell>& Visite(const std::shared_ptr<Cell>& cell, bool visite = true)
    {
      cell->isVisited = visite;
      return cell;
    }
    bool IsVisited() const { return this->isVisited; }

    // @todo this information should not be part the Cell info itself
    int GetRootDistance() const { return this->rootDistance; }
    void SetRootDistance(const int distance) { this->rootDistance = distance; }

  private:
    uint8_t x;
    uint8_t y;

    int rootDistance;       // @todo cf. above
    bool isVisited;         // @todo cf. above
    std::set<std::weak_ptr<Cell>> connectedCells;
  };

  bool operator<(const std::weak_ptr<Cell>& cellA, const std::weak_ptr<Cell>& cellB)
  { return (cellA.lock()->GetX() < cellB.lock()->GetX() ||
           (cellA.lock()->GetX() == cellB.lock()->GetX() && cellA.lock()->GetY() < cellB.lock()->GetY())); }

  typedef std::shared_ptr<Cell> CellShared;
  typedef std::weak_ptr<Cell> CellWeak;
  typedef std::vector<std::vector<CellShared>> MazeMatrixShared;

  /// @class MazeRecursiveDivisionLog
  ///
  class MazeRecursiveDivisionLog
  {
    public:
      static const String GetName() { return "Recursive Division Maze Generator"; }

      // Assert correct JSON construction.
      ~MazeRecursiveDivisionLog() { assert(this->writer->IsComplete()); }

      /// Instantiate a new json writer using the stream passed as
      /// argument, run and write algorithm computation information.
      ///
      /// @return stream reference filled up with MazeRecursiveDivisionLog object information,
      ///         error object information in case of failure.
      static Ostream& Build(Ostream& os, const uint8_t width, const uint8_t height, const uint8_t seed = 0)
      {
        auto builder = std::unique_ptr<MazeRecursiveDivisionLog>(new MazeRecursiveDivisionLog(os));
        builder->Write(width, height, seed);

        return os;
      }

      /// Use json writer passed as parameter to write iterator information.
      ///
      /// @return stream reference filled up with MazeRecursiveDivisionLog object information,
      ///         error information in case of failure.
      static Writer& Build(Writer& writer, const uint8_t width, const uint8_t height, const uint8_t seed = 0)
      {
        Write(writer, width, height, seed);

        return writer;
      }

    private:
      MazeRecursiveDivisionLog(Ostream& os) : stream(std::unique_ptr<Stream>(new Stream(os))),
                                              writer(std::unique_ptr<Writer>(new Writer(*this->stream))) {}
      MazeRecursiveDivisionLog operator=(MazeRecursiveDivisionLog&) = delete; // Not Implemented

      bool Write(const uint8_t width, const uint8_t height, const uint8_t seed)
      { return Write(*this->writer, width, height, seed); }

      static bool Write(Writer& writer, const uint8_t width, const uint8_t height, const uint8_t seed)
      {
        // Do not compute if data incorrect
        if (width < 1 || height < 1)
        {
          Comment::Build(writer, "Sequence size too small to be processed.", 0);
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
          writer.Key("isWallBuilding");
          writer.Bool(true);
        writer.EndObject();
        writer.Key("name");
        writer.String(GetName());
        WriteParameters(writer, width, height, seed);   // Write parameters
        WriteComputation(writer, width, height, seed);  // Write computation

        writer.EndObject();

        return true;
      }

      ///
      static bool WriteParameters(Writer& writer,
                                  const uint8_t width, const uint8_t height, const uint8_t seed)
      {
        writer.Key("parameters");
        writer.StartArray();
          Value<uint8_t>::Build(writer, "width", width);
          Value<uint8_t>::Build(writer, "height", height);
          Value<uint8_t>::Build(writer, "seed", seed);
        writer.EndArray();

        return true;
      }

      static bool WriteSubComputation(
          Writer& writer, std::mt19937& mt,
          MazeMatrixShared& mazeMatrix, const Cell& offSetCell, const uint8_t width, uint8_t height)
      {
        if (width < 2 || height < 2)
          return true;

        // Cut on the biggest side at random position
        // todo rand 2 for which way
        bool isHorizontalCut = (width <= height);
        uint8_t wallIdx = (isHorizontalCut) ? (mt() % (height - 1)) + 1 : (mt() % (width - 1)) + 1;
        uint8_t pathIdx = (isHorizontalCut) ? (mt() % (width - 1)) + 1 : (mt() % (height - 1)) + 1;

        /// LOG WALL BUILDING
        writer.StartObject();
          writer.Key("type");
          writer.String("operation");
          writer.Key("name");
          writer.String("BuildWall");
          writer.Key("offsetCell");
          writer.StartArray();
            writer.Int(offSetCell.GetX());
            writer.Int(offSetCell.GetY());
          writer.EndArray();
          writer.Key("direction");
          writer.String((isHorizontalCut) ? "horizontal" : "vertical");
          writer.Key("range");
          writer.StartArray();
            writer.Int(width);
            writer.Int(height);
          writer.EndArray();
          writer.Key("wallIdx");
          writer.Int((isHorizontalCut) ? offSetCell.GetY() + wallIdx : offSetCell.GetX() + wallIdx);
          writer.Key("pathIdx");
          writer.Int((isHorizontalCut) ? offSetCell.GetX() + pathIdx : offSetCell.GetY() + pathIdx);
        writer.EndObject();

        // Recurse on sub areas
        if (isHorizontalCut)
        {
          // Disconnect all cells separated by the wall
          for (auto x = 0; x < width; ++x)
          {
            if (x == pathIdx)
              continue;

            mazeMatrix[offSetCell.GetX() + x][offSetCell.GetY() + wallIdx]->DisconnectCell(
              mazeMatrix[offSetCell.GetX() + x][offSetCell.GetY() + wallIdx - 1]);

            mazeMatrix[offSetCell.GetX() + x][offSetCell.GetY() + wallIdx - 1]->DisconnectCell(
              mazeMatrix[offSetCell.GetX() + x][offSetCell.GetY() + wallIdx]);
          }

          // Top area
          WriteSubComputation(writer, mt, mazeMatrix, offSetCell, width, wallIdx);
          // Bottom area
          WriteSubComputation(writer, mt, mazeMatrix,
                              *mazeMatrix[offSetCell.GetX()][offSetCell.GetY() + wallIdx],
                              width, height - wallIdx);
        } else
        {
          // Disconnect all cells separated by the wall
          for (uint8_t y = 0; y < height; ++y)
          {
            if (y == pathIdx)
              continue;

            mazeMatrix[offSetCell.GetX() + wallIdx][y + offSetCell.GetY()]->DisconnectCell(
              mazeMatrix[offSetCell.GetX() + wallIdx - 1][y + offSetCell.GetY()]);

            mazeMatrix[offSetCell.GetX() + wallIdx - 1][y + offSetCell.GetY()]->DisconnectCell(
              mazeMatrix[offSetCell.GetX() + wallIdx][y + offSetCell.GetY()]);
          }

          // Left area
          WriteSubComputation(writer, mt, mazeMatrix, offSetCell, wallIdx, height);
          // Right area
          WriteSubComputation(writer, mt, mazeMatrix,
                              *mazeMatrix[offSetCell.GetX() + wallIdx][offSetCell.GetY()],
                              width - wallIdx, height);
        }

        return true;
      }

      ///
      /// @brief WriteComputation
      /// @param writer
      /// @param width
      /// @param height
      /// @param seed
      /// @return
      ///
      static bool WriteComputation(Writer& writer, const uint8_t width, const uint8_t height, uint8_t seed)
      {
        // Init Matrix - Each cell are connected at the beginning of the algorithm
        MazeMatrixShared mazeMatrix;
        mazeMatrix.resize(width);
        for (uint8_t x = 0; x < width; ++x)
        {
          mazeMatrix[x].reserve(height);
          for (uint8_t y = 0; y < height; ++y)
          {
            mazeMatrix[x].push_back(std::shared_ptr<Cell>(new Cell(x, y)));

            if (x > 0)
            {
              mazeMatrix[x - 1][y]->AddConnection(mazeMatrix[x][y]);
              mazeMatrix[x][y]->AddConnection(mazeMatrix[x - 1][y]);
            }
            if (y > 0)
            {
              mazeMatrix[x][y - 1]->AddConnection(mazeMatrix[x][y]);
              mazeMatrix[x][y]->AddConnection(mazeMatrix[x][y - 1]);
            }
          }
        }

        // Initialize random generator based on Mersenne Twister algorithm
        // @todo use seed / random generator parameter (also usefull for testing purpose)
        std::mt19937 mt(seed);

        // While there is node to be handled
        writer.Key("logs");
        writer.StartArray();

        /// LOG COMPUTATION
        WriteSubComputation(writer, mt, mazeMatrix, *(mazeMatrix[0][0].get()), width, height);

        Operation::Return<bool>(writer, true);
        writer.EndArray();

        return true;
      }

      std::unique_ptr<Stream> stream; // Stream wrapper
      std::unique_ptr<Writer> writer; // Writer used to fill the stream
  };
}

#endif // MODULE_DS_MAZE_RECURSIVE_DIVISION_LOG_HXX
