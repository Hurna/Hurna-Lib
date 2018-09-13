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
    { this->connectedCells.push_back(cell); }
    std::vector<std::weak_ptr<Cell>>& GetConnections()
    { return this->connectedCells; }

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

    void Write(Writer& writer)
    {
      writer.StartObject();
        writer.Key("x");
        writer.Int(this->x);
        writer.Key("y");
        writer.Int(this->y);
        writer.Key("connectedCells");
        writer.StartArray();
          //for (auto it = )
        writer.EndArray();
      writer.EndObject();
    }

  private:
    uint8_t x;
    uint8_t y;

    int rootDistance; // @todo cf. above
    bool isVisited;   // @todo cf. above
    std::vector<std::weak_ptr<Cell>> connectedCells;
  };

  typedef std::shared_ptr<Cell> CellShared;
  typedef std::weak_ptr<Cell> CellWeak;
  typedef std::vector<std::vector<CellShared>> MazeMatrixShared;

  /// @class MazeSidewinderLog
  ///
  class MazeSidewinderLog
  {
    public:
      static const String GetName() { return "Sidewinder Maze Generator"; }

      // Assert correct JSON construction.
      ~MazeSidewinderLog() { assert(this->writer->IsComplete()); }

      /// Instantiate a new json writer using the stream passed as
      /// argument, run and write algorithm computation information.
      ///
      /// @return stream reference filled up with MazeSidewinderLog object information,
      ///         error object information in case of failure.
      static Ostream& Build(Ostream& os, const uint8_t width, const uint8_t height, const uint8_t seed = 0)
      {
        auto builder = std::unique_ptr<MazeSidewinderLog>(new MazeSidewinderLog(os));
        builder->Write(width, height, seed);

        return os;
      }

      /// Use json writer passed as parameter to write iterator information.
      ///
      /// @return stream reference filled up with MazeSidewinderLog object information,
      ///         error information in case of failure.
      static Writer& Build(Writer& writer, const uint8_t width, const uint8_t height, const uint8_t seed = 0)
      {
        Write(writer, width, height, seed);

        return writer;
      }

    private:
      MazeSidewinderLog(Ostream& os) : stream(std::unique_ptr<Stream>(new Stream(os))),
                                       writer(std::unique_ptr<Writer>(new Writer(*this->stream))) {}
      MazeSidewinderLog operator=(MazeSidewinderLog&) = delete; // Not Implemented

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
        writer.EndArray();

        return true;
      }

      ///
      static bool WriteComputation(Writer& writer,
                                   const uint8_t width, const uint8_t height, const uint8_t seed)
      {
        // Init Matrix
        std::vector<std::vector<std::shared_ptr<Cell>>> mazeMatrix;
        mazeMatrix.resize(width);
        for (uint8_t x = 0; x < width; ++x)
        {
          mazeMatrix[x].reserve(height);
          for (uint8_t y = 0; y < height; ++y)
            mazeMatrix[x].push_back(std::shared_ptr<Cell>(new Cell(x, y)));
        }

        // Initialize run set
        std::set<CellShared> runSet;

        /// LOG LOCALS
        writer.Key("locals");
        writer.StartArray();
          /// LOG STACK
          writer.StartObject();
            writer.Key("name");
            writer.String("runSet");
            writer.Key("type");
            writer.String("set");
            writer.Key("dataType");
            writer.String("Cell");
          writer.EndObject();
        writer.EndArray();

        // While there is node to be handled
        writer.Key("logs");
        writer.StartArray();

        /// LOG COMPUTATION
        Comment::Build(writer, "Initialize random generator based on Mersenne Twister with seed: " +
          std::to_string(seed));
        std::mt19937 mt(seed);

        Comment::Build(writer, "First row can only be a single passage, crave it to the east: ");
        for (uint8_t x = 0; x + 1 < width; ++x)
        {
          mazeMatrix[x][0]->AddConnection(mazeMatrix[x + 1][0]);
          mazeMatrix[x + 1][0]->AddConnection(mazeMatrix[x][0]);

          /// LOG CONNECT
          writer.StartObject();
            writer.Key("type");
            writer.String("operation");
            writer.Key("name");
            writer.String("ConnectEdge");
            writer.Key("cells");
            writer.StartArray();
              writer.StartArray();
                writer.Int(x);
                writer.Int(0);
              writer.EndArray();
              writer.StartArray();
                writer.Int(x + 1);
                writer.Int(0);
              writer.EndArray();
           writer.EndArray();
          writer.EndObject();
        }

        Comment::Build(writer, "Initialize an empty “run” set to keep track of the current line path.");
        Comment::Build(writer, "Scan grid line by line starting with the cell[0,1]: ");
        Comment::Build(writer, "Add current cell to the path and randomly carve east or not.", 1);
        Comment::Build(writer, "If a passage was carved, continue line scan.", 1);
        Comment::Build(writer, "Otherwise randomly choose a cell in the run set and carve north, " +
          std::string("empty the run set and continue line scan."), 1);
        for (uint8_t y = 1; y < height; ++y)
        {
          for (uint8_t x = 0; x < width; ++x)
          {
            runSet.insert(mazeMatrix[x][y]);

            // Randomly carve to east
            if (mt() % 2 == 0 && x + 1 < width)
            {
              mazeMatrix[x][y]->AddConnection(mazeMatrix[x + 1][y]);
              mazeMatrix[x + 1][y]->AddConnection(mazeMatrix[x][y]);

              /// LOG CONNECT
              writer.StartObject();
                writer.Key("type");
                writer.String("operation");
                writer.Key("name");
                writer.String("ConnectEdge");
                writer.Key("cells");
                writer.StartArray();
                  writer.StartArray();
                    writer.Int(x);
                    writer.Int(y);
                  writer.EndArray();
                  writer.StartArray();
                    writer.Int(x + 1);
                    writer.Int(y);
                  writer.EndArray();
               writer.EndArray();
              writer.EndObject();
            }
            // Carve to the north randomly from current run set
            else
            {
              auto curCellIt = runSet.begin();
              std::advance(curCellIt, mt() % runSet.size());

              (*curCellIt)->AddConnection(mazeMatrix[(*curCellIt)->GetX()][(*curCellIt)->GetY() - 1]);
              mazeMatrix[(*curCellIt)->GetX()][(*curCellIt)->GetY() - 1]->AddConnection(*curCellIt);

              /// LOG CONNECT
              writer.StartObject();
                writer.Key("type");
                writer.String("operation");
                writer.Key("name");
                writer.String("ConnectEdge");
                writer.Key("cells");
                writer.StartArray();
                  writer.StartArray();
                    writer.Int((*curCellIt)->GetX());
                    writer.Int((*curCellIt)->GetY());
                  writer.EndArray();
                  writer.StartArray();
                    writer.Int((*curCellIt)->GetX());
                    writer.Int((*curCellIt)->GetY() - 1);
                  writer.EndArray();
               writer.EndArray();
              writer.EndObject();

              runSet.clear();
            }
          }

          runSet.clear();
        }

        Operation::Return<bool>(writer, true);
        writer.EndArray();

        return true;
      }

      std::unique_ptr<Stream> stream; // Stream wrapper
      std::unique_ptr<Writer> writer; // Writer used to fill the stream
  };
}

#endif // MODULE_DS_MAZE_SIDEWINDER_LOG_HXX
