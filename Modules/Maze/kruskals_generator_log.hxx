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
#include <Logger/array.hxx>
#include <Logger/comment.hxx>
#include <Logger/operation.hxx>
#include <Logger/typedef.hxx>
#include <Logger/value.hxx>

// STD includes
#include <algorithm>
#include <memory>
#include <queue>
#include <random>
#include <set>
#include <vector>

namespace HUL_Logger
{
  // Cell - Make it fully generic
  // CellInfo - Template parameter
  class Cell {
  public:
    Cell() : x(0), y(0), isVisited(false) {}
    Cell(uint8_t x, uint8_t y) : x(x), y(y), isVisited(false) {}

    uint8_t GetX() const { return this->x; }
    uint8_t GetY() const { return this->y; }

    void SetBucketId(uint32_t id) { this->bucketId = id; }  // Set Bucket connection cell
    uint32_t GetBucketId() const { return this->bucketId; } // Get bucket cell owner

    // @todo this information should not be part the cell info itself
    static const std::shared_ptr<Cell>& Visite(const std::shared_ptr<Cell>& cell)
    {
      cell->isVisited = true;
      return cell;
    }
    bool IsVisited() const { return this->isVisited; }

    void AddConnection(std::shared_ptr<Cell> cell)
    { this->connectedCells.push_back(cell); }
    std::vector<std::weak_ptr<Cell>>& GetConnections()
    { return this->connectedCells; }

  private:
    uint8_t x;
    uint8_t y;

    bool isVisited;         // @todo cf. above
    uint32_t bucketId;      // Precision should be above than 2 * (uint8_t * uint8_t)
    std::vector<std::weak_ptr<Cell>> connectedCells;
  };

  typedef std::shared_ptr<Cell> CellShared;
  typedef std::weak_ptr<Cell> CellWeak;
  typedef std::vector<std::vector<CellShared>> MazeMatrixShared;

  /// @class MazeKruskalsLog
  ///
  /// Static Distance Fill --> will be useful for Binary && sidewinder
  class MazeKruskalsLog
  {
    public:
      static const String GetName() { return "Kruskals Maze Generator"; }

      // Assert correct JSON construction.
      ~MazeKruskalsLog() { assert(this->writer->IsComplete()); }

      /// Instantiate a new json writer using the stream passed as
      /// argument, run and write algorithm computation information.
      ///
      /// @return stream reference filled up with MazeKruskalsLog object information,
      ///         error object information in case of failure.
      static Ostream& Build(Ostream& os, const uint8_t width, const uint8_t height, const uint8_t seed = 0)
      {
        std::unique_ptr<MazeKruskalsLog> builder = std::unique_ptr<MazeKruskalsLog>(new MazeKruskalsLog(os));
        builder->Write(width, height, seed);

        return os;
      }

      /// Use json writer passed as parameter to write iterator information.
      ///
      /// @return stream reference filled up with MazeKruskalsLog object information,
      ///         error information in case of failure.
      static Writer& Build(Writer& writer, const uint8_t width, const uint8_t height, const uint8_t seed = 0)
      {
        Write(writer, width, height, seed);

        return writer;
      }

    private:
      MazeKruskalsLog(Ostream& os) : stream(std::unique_ptr<Stream>(new Stream(os))),
                                     writer(std::unique_ptr<Writer>(new Writer(*this->stream))) {}
      MazeKruskalsLog operator=(MazeKruskalsLog&) {} // Not Implemented

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

        /// LOG LOCALS
        writer.Key("locals");
        writer.StartArray();
          /// LOG STACK
          writer.StartObject();
            writer.Key("name");
            writer.String("pathStack");
            writer.Key("type");
            writer.String("set");
            writer.Key("dataType");
            writer.String("Cell");
          writer.EndObject();
        writer.EndArray();

        /// Start Logs
        writer.Key("logs");
        writer.StartArray();

        Comment::Build(writer, "Initialize random generator based on Mersenne Twister algorithm.");
        std::mt19937 mt(seed);

        Comment::Build(writer, "Create buckets for each cell and a set with all possible connecting edges.");
        std::set<std::pair<std::shared_ptr<Cell>, std::shared_ptr<Cell>>> edges;
        std::vector<std::vector<std::shared_ptr<Cell>>> bucketCells;
        bucketCells.resize(height * width);//(height - 1) * width + (width - 1) * height); // #edges @TODO NOP #cells!
        {
          uint32_t nodeId = 0;
          for (auto itX = mazeMatrix.begin(); itX != mazeMatrix.end(); ++itX)
            for (auto itY = itX->begin(); itY != itX->end(); ++itY, ++nodeId)
            {
              (*itY)->SetBucketId(nodeId);
              bucketCells[nodeId].push_back(*itY);

              // Insert Right edge if on maze
              if ((*itY)->GetX() + 1 < mazeMatrix.size())
                edges.insert(std::make_pair(*itY, mazeMatrix[(*itY)->GetX() + 1][(*itY)->GetY()]));

              // Insert Bottom edge if on maze
              if ((*itY)->GetY() + 1 < itX->size())
                edges.insert(std::make_pair(*itY, mazeMatrix[(*itY)->GetX()][(*itY)->GetY() + 1]));
            }
        }

        // Compute each edge
        Comment::Build(writer, "While the set of edges is not empty randomly get an edge; connect cells" +
                       static_cast<std::string>(" and merge buckets if not already in the same one:"));
        while (!edges.empty())
        {
          auto edgeIt = edges.begin();
          std::advance(edgeIt, mt() % edges.size());

          // Convenient variables
          const auto cellAPosX = (*edgeIt).first->GetX();
          const auto cellAPosY = (*edgeIt).first->GetY();
          const auto cellABucketId = (*edgeIt).first->GetBucketId();
          const auto cellBPosX = (*edgeIt).second->GetX();
          const auto cellBPosY = (*edgeIt).second->GetY();
          const auto cellBBucketId = (*edgeIt).second->GetBucketId();

          // Log edge selection
          writer.StartObject();
            writer.Key("type");
            writer.String("operation");
            writer.Key("name");
            writer.String("SelectEdgePop");
            writer.Key("cells");
            writer.StartArray();
              writer.StartArray();
                writer.Int(cellAPosX);
                writer.Int(cellAPosY);
              writer.EndArray();
              writer.StartArray();
                writer.Int(cellBPosX);
                writer.Int(cellBPosY);
              writer.EndArray();
           writer.EndArray();
          writer.EndObject();

          if (cellABucketId != cellBBucketId)
          {
            /// LOG CONNECT
            writer.StartObject();
              writer.Key("type");
              writer.String("operation");
              writer.Key("name");
              writer.String("ConnectEdge");
              writer.Key("cells");
              writer.StartArray();
                writer.StartArray();
                  writer.Int(cellAPosX);
                  writer.Int(cellAPosY);
                writer.EndArray();
                writer.StartArray();
                  writer.Int(cellBPosX);
                  writer.Int(cellBPosY);
                writer.EndArray();
             writer.EndArray();
             writer.Key("sub");
             writer.String("MergeBuckets");
             writer.Key("buckets");
             writer.StartArray();
               writer.Int(cellABucketId);
               writer.Int(cellBBucketId);
            writer.EndArray();
            writer.EndObject();

            // Add bothway connection and merge buckets
            (*edgeIt).first->AddConnection((*edgeIt).second);
            (*edgeIt).second->AddConnection((*edgeIt).first);
            MergeBucket(bucketCells, cellABucketId, cellBBucketId);
          }

          // Remove computed cell from the set
          edges.erase(edgeIt);
        }

        Operation::Return<bool>(writer, true);
        writer.EndArray();

        // Add Statistical informations
        writer.Key("stats");
        writer.StartObject();
          // Memory
          writer.Key("memory");
          writer.StartObject();
            writer.Key("type");
            writer.String("set");
            writer.Key("object");
            writer.String("edge");
            writer.Key("maxSize");
            writer.Int((height - 1) * width + (width - 1) * height);
            writer.Key("initialSize");
            writer.Int((height - 1) * width + (width - 1) * height);
            //writer.Key("nbOperations");
            //writer.Int((height - 1) * width + (width - 1) * height);
          writer.EndObject();
        writer.EndObject();

        return true;
      }

      /// Merge two buckets of node together and update node bucket Id.
      /// @todo log?
      ///
      /// @brief MergeBucket
      /// @param buckets
      /// @param bucketIdA
      /// @param bucketIdB
      static void MergeBucket(MazeMatrixShared& buckets, uint32_t bucketIdA, uint32_t bucketIdB)
      {
        assert(bucketIdA != bucketIdB);

        // Set new bucket id for each melding cell
        for (auto it = buckets[bucketIdB].begin(); it != buckets[bucketIdB].end(); ++it)
        {
          (*it)->SetBucketId(bucketIdA);
          buckets[bucketIdA].push_back(*it);
        }

        buckets[bucketIdB].clear();
      }

      std::unique_ptr<Stream> stream; // Stream wrapper
      std::unique_ptr<Writer> writer; // Writer used to fill the stream
  };
}

#endif // MODULE_DS_MAZE_KRUSKALS_LOG_HXX
