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
#ifndef MODULE_MAZE_BIN_GEN_LOG_HXX
#define MODULE_MAZE_BIN_GEN_LOG_HXX

#include <Logger/algorithm.hxx>
#include <Logger/array.hxx>
#include <Logger/comment.hxx>
#include <Logger/operation.hxx>
#include <Logger/typedef.hxx>
#include <Logger/value.hxx>

// STD includes
#include <memory>
#include <queue>
#include <random>
#include <stack>
#include <vector>

#include <iostream>

namespace hul
{
  namespace maze
  {
  class Cell {
  public:
    Cell() : x(0), y(0) {}
    Cell(uint8_t x, uint8_t y) : x(x), y(y) {}

    uint8_t GetX() const { return this->x; }
    uint8_t GetY() const { return this->y; }

    void AddConnection(std::shared_ptr<Cell> cell)
    { this->connectedCells.push_back(cell); }

    std::vector<std::weak_ptr<Cell>>& GetConnections()
    { return this->connectedCells; }

  private:
    uint8_t x;
    uint8_t y;

    std::vector<std::weak_ptr<Cell>> connectedCells;
  };

  typedef std::shared_ptr<Cell> CellShared;
  typedef std::vector<std::vector<CellShared>> MazeMatrixShared;

  /// @class MazeBinaryTreeLog
  ///
  class BinaryTree
  {
    public:
      static const String GetName() { return "Binary Tree Maze Generator"; }
      static const String GetVersion() { return "1.0.0"; }
      static const String GetType() { return "algorithm"; }

      ///
      static Ostream& Build(Ostream& os, const uint8_t width, const uint8_t height, const uint8_t seed = 0)
      {
        auto builder = std::unique_ptr<BinaryTree>(new MazeBinaryTreeLog(os));
        builder->Write(width, height, seed);

        return os;
      }

      ///
      static std::unique_ptr<Grid>&
      Build(Writer& writer, const uint8_t width, const uint8_t height, const uint8_t seed = 0)
      {
        Write(writer, width, height, seed);

        return writer;
      }

    private:
      BinaryTree(Ostream& os) : stream(std::unique_ptr<Stream>(new Stream(os))),
                                writer(std::unique_ptr<Writer>(new Writer(*this->stream))) {}
      BinaryTree operator=(BinaryTree&) = delete; // Not Implemented

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
          // Direction
        writer.EndArray();

        return true;
      }

      ///
      static bool WriteComputation(Writer& writer,
                                   const uint8_t width, const uint8_t height, const uint8_t seed)
      {
        // Init Matrix
        MazeMatrixShared mazeMatrix;
        mazeMatrix.resize(width);
        for (uint8_t x = 0; x < width; ++x)
        {
          mazeMatrix[x].reserve(height);
          for (uint8_t y = 0; y < height; ++y)
            mazeMatrix[x].push_back(CellShared(new Cell(x, y)));
        }

        /// LOG COMPUTATION
        writer.Key("logs");
        writer.StartArray();

        // Initialize random generator based on Mersenne Twister algorithm
        Comment::Build(writer, "Initialize Mersenne Twister random generator  with seed: " + ToString(seed));
        std::mt19937 mt(seed);

        Comment::Build(writer,
          "For each existing cell in the grid, randomly carve a passage either east or south : ");
        for (uint8_t y = 0; y < height; ++y)
        {
          for (uint8_t x = 0; x < width; ++x)
          {
            // Get available neighbours
            auto curCell = mazeMatrix[x][y];
            auto curNeighbours = GetAvailableNeighbours(mazeMatrix, *curCell.get());

            /// LOG SELECT
            writer.StartObject();
              writer.Key("type");
              writer.String("operation");
              writer.Key("name");
              writer.String("SelectCell");
              writer.Key("cell");
              writer.StartArray();
                writer.Int(curCell->GetX());
                writer.Int(curCell->GetY());
              writer.EndArray();
            writer.EndObject();

            if (curNeighbours.empty())
              continue;

            // Randomly select a node to be processed
            auto randIdx = mt() % curNeighbours.size();
            curCell->AddConnection(curNeighbours[randIdx]);
            curNeighbours[randIdx]->AddConnection(curCell);

            /// LOG CONNECT
            writer.StartObject();
              writer.Key("type");
              writer.String("operation");
              writer.Key("name");
              writer.String("ConnectCells");
              writer.Key("cells");
              writer.StartArray();
                writer.StartArray();
                  writer.Int(curCell->GetX());
                  writer.Int(curCell->GetY());
                writer.EndArray();
                writer.StartArray();
                  writer.Int(curNeighbours[randIdx]->GetX());
                  writer.Int(curNeighbours[randIdx]->GetY());
                writer.EndArray();
              writer.EndArray();
            writer.EndObject();
          }
        }

        Operation::Return<bool>(writer, true);
        writer.EndArray();

        return true;
      }

      ///
      /// @return neighbours that has not been visited yet.
      static std::vector<std::shared_ptr<Cell>> GetAvailableNeighbours(
            const std::vector<std::vector<std::shared_ptr<Cell>>>& mazeMatrix,
            const Cell& cell)
      {
        std::vector<std::shared_ptr<Cell>> neighbours;

        const auto curX = cell.GetX();
        const auto curY = cell.GetY();

        // South East Direction - Push left if available
        if (curX > 0) neighbours.push_back(mazeMatrix[curX - 1][curY]);
        // South East Direction - Push bottom if available
        if (curY > 0) neighbours.push_back(mazeMatrix[curX][curY - 1]);

        return neighbours;
      }

      std::unique_ptr<Stream> stream; // Stream wrapper
      std::unique_ptr<Writer> writer; // Writer used to fill the stream
  };
  }
}

#endif // MODULE_MAZE_BIN_GEN_LOG_HXX
