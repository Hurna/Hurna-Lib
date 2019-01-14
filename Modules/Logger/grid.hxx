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
#ifndef MODULE_LOGGER_GRID_HXX
#define MODULE_LOGGER_GRID_HXX

// STD
#include <set>

#include <Logger/vector.hxx>

namespace hul
{
  struct CellInfoBase
  {
    CellInfoBase() : isVisited(false), rootDistance(-1) {}
    bool isVisited;
    int rootDistance;
  };


  /// @class Grid
  /// Wrap std::grid adding log operations writing and statisticals informations
  ///
  template <typename CellInfo = CellInfoBase>
  class Grid
  {
  public:
    static const String GetName() { return "grid"; }
    static const String GetType() { return "data_structure"; }
    static const String GetVersion() { return "1.0.0"; }

    explicit Grid
      (Ostream& os, uint8_t width, uint8_t height, bool isConnected = false, const String& name = "") :
      logger(std::shared_ptr<Logger>(new Logger(os))),
      data(logger), name(name), id(ToString(static_cast<void*>(this)))
    { Init(width, height, isConnected); }

    explicit Grid(std::shared_ptr<Logger> logger,
                  uint8_t width, uint8_t height,  bool isConnected = false, const String& name = "") :
      logger(logger), data(logger), name(name), id(ToString(static_cast<void*>(this)))
    { Init(width, height, isConnected); }

    // Members
    const String name;  // Identifiant name
    const String id;    // Internal identifiant (memory address)

    struct Point
    {
      Point(uint8_t x = 0, uint8_t y = 0) : x(x), y(y) {}
      uint8_t x;
      uint8_t y;

      void Log(Logger& logger, const String& name = "") const
      {
        logger.StartObject();
          logger.AddEntry("type", "grid::point");
          if (!name.empty()) logger.AddEntry("name", name);
          logger.AddEntry("x", this->x);
          logger.AddEntry("y", this->y);
        logger.EndObject();
      }

      void LogInfo(Logger& logger, const String& name = "") const
      {
        logger.StartObject(name);
          logger.AddEntry("x", this->x);
          logger.AddEntry("y", this->y);
        logger.EndObject();
      }
    };

    class Cell {
    public:
      static const String GetName() { return "cell"; }

      Cell(uint8_t x, uint8_t y) : id(ToString(static_cast<void*>(this))), x(x), y(y) {}

      const String id; // Internal identifiant (memory address)
      const uint8_t x; // X coordinate
      const uint8_t y; // Y coordinate

      std::set<String> connectedCells;  // Directed connections from this cell
      CellInfo info;                    // Use to store extra information

    private:
      Cell operator=(Cell&); // Not Implemented
    };

    class Edge {
    public:
      static const String GetName() { return "edge"; }

      Edge(std::shared_ptr<Cell> first, std::shared_ptr<Cell> second) :
        id(ToString(static_cast<void*>(this))), first(first), second(second) {}

      const String id; // Internal identifiant (memory address)
      const std::shared_ptr<Cell> first; // X coordinate
      const std::shared_ptr<Cell> second; // Y coordinate

    private:
      Edge operator=(Edge&); // Not Implemented
    };

    struct Stats
    {
      Stats() : maxDistance(0), nbConnexion(0) {}
      int maxDistance;  // Keep maximal distance from the root cell
      int nbConnexion;
    };

    void Connect(const std::shared_ptr<Cell> root, const std::shared_ptr<Cell> cell, bool mute = false)
    {
      root->connectedCells.insert(cell->id);
      cell->connectedCells.insert(root->id);
      this->stats.nbConnexion += 2;

      if (!mute)
      {
        logger->StartObject();
          logger->AddEntry("type", "operation");
          logger->AddEntry("name", "Connect");
          logger->AddEntry("root", root->id);
          logger->StartArray("cells");
            logger->Add(cell->id);
          logger->EndArray();
        logger->EndObject();
      }
    }

    void Connect(const std::shared_ptr<Cell> cell, const std::vector<std::shared_ptr<Cell>>& neighbours)
    {
      if (neighbours.size() < 1)
        return;

      logger->StartObject();
        logger->AddEntry("type", "operation");
        logger->AddEntry("name", "Connect");
        logger->AddEntry("root", cell->id);
        logger->StartArray("cells");
          for (auto it = neighbours.begin(); it != neighbours.end(); ++it)
          {
            logger->Add((*it)->id);
            cell->connectedCells.insert((*it)->id);
            (*it)->connectedCells.insert(cell->id);
            this->stats.nbConnexion += 2;
          }
        logger->EndArray();
      logger->EndObject();
    }

    void Disconnect(const std::shared_ptr<Cell> root, const std::shared_ptr<Cell> cell, bool mute = false)
    {
      root->connectedCells.erase(cell->id);
      cell->connectedCells.erase(root->id);
      this->stats.nbConnexion -= 2;

      if (!mute)
      {
        logger->StartObject();
          logger->AddEntry("type", "operation");
          logger->AddEntry("name", "Disconnect");
          logger->AddEntry("root", root->id);
          logger->StartArray("cells");
            logger->Add(cell->id);
          logger->EndArray();
        logger->EndObject();
      }
    }

    void DisconnectCol(const Point& origin, const u_int8_t idx, const u_int8_t height, const uint8_t pathIdx)
    {
      for (auto y = 0; y < height; ++y)
      {
        if (y == pathIdx)
          continue;

        this->Disconnect(this->data[origin.x + idx][origin.y + y],
                         this->data[origin.x + idx + 1][origin.y + y], true);
      }

      logger->StartObject();
        logger->AddEntry("type", "operation");
        logger->AddEntry("name", "DisconnectCol");
        origin.LogInfo(*this->logger.get(), "origin");
        logger->AddEntry("size", height);
        logger->AddEntry("idx", idx);
        logger->AddEntry("pathIdx", pathIdx);
      logger->EndObject();
    }

    void DisconnectRow(const Point& origin, const u_int8_t idx, const u_int8_t width, const uint8_t pathIdx)
    {
      for (auto x = 0; x < width; ++x)
      {
        if (x == pathIdx)
          continue;

        this->Disconnect(this->data[origin.x + x][origin.y + idx],
                         this->data[origin.x + x][origin.y + idx + 1], true);
      }

      logger->StartObject();
        logger->AddEntry("type", "operation");
        logger->AddEntry("name", "DisconnectRow");
        origin.LogInfo(*this->logger.get(), "origin");
        logger->AddEntry("size", width);
        logger->AddEntry("idx", idx);
        logger->AddEntry("pathIdx", pathIdx);
      logger->EndObject();
    }

    ///
    /// \brief LogInfo
    ///
    void Log(const String& name = "") const
    {
      logger->StartObject(name);
        logger->AddEntry("type", GetType());
        logger->AddEntry("dataType", Cell::GetName());
        logger->AddEntry("name", GetName());
        logger->AddEntry("id", this->id);
        logger->AddEntry("alias", this->name);

        logger->StartObject("info");
          logger->AddEntry("width", Width());
          logger->AddEntry("height", Height());
          logger->AddEntry("maxDistance", this->stats.maxDistance);
        logger->EndObject();

        logger->StartObject("stats");
          logger->AddEntry("nbCell", Width() * Height());
          logger->AddEntry("nbConnexion", this->stats.nbConnexion);
        logger->EndObject();

        // Log list of cells as a map using the id as key
        logger->StartObject("cells");
          for (uint8_t x = 0; x < Width(); ++x)
            for (uint8_t y = 0; y < Height(); ++y)
            {
              auto curCell = data[x][y];
              logger->StartObject(curCell->id);
                logger->AddEntry("x", curCell->x);
                logger->AddEntry("y", curCell->y);
                logger->AddEntry("rootDistance", curCell->info.rootDistance);
                logger->StartArray("connectedCells");
                  for (auto it = curCell->connectedCells.begin(); it != curCell->connectedCells.end(); ++it)
                    logger->Add(*it);
                logger->EndArray();
              logger->EndObject();
            }

        logger->EndObject();
      logger->EndObject();
    }

    ///
    /// \brief LogInfo
    ///
    void LogStats(const String& name = "") const
    {
      logger->StartObject(name);
        logger->AddEntry("type", GetType());
        logger->AddEntry("dataType", Cell::GetName());
        logger->AddEntry("name", GetName());
        logger->AddEntry("id", this->id);
        logger->AddEntry("alias", this->name);

        logger->StartObject("stats");
          logger->AddEntry("nbCell", Width() * Height());
          logger->AddEntry("nbConnexion", this->stats.nbConnexion);
        logger->EndObject();
      logger->EndObject();
    }

    // @todo delete once H.urna::stack structure available
    std::shared_ptr<Cell> Select(const std::shared_ptr<Cell> cell) const
    {
      logger->StartObject();
        logger->AddEntry("type", "operation");
        logger->AddEntry("name", "SelectCell");
        logger->AddEntry("id", cell->id);
        logger->AddEntry("rootDistance", cell->info.rootDistance);
      logger->EndObject();

      this->stats.maxDistance = std::max(this->stats.maxDistance, cell->info.rootDistance);

      return cell;
    }

    void SelectEdge(const std::shared_ptr<Cell> first, const std::shared_ptr<Cell> second) const
    {
      logger->StartObject();
        logger->AddEntry("type", "operation");
        logger->AddEntry("name", "SelectEdge");
        logger->AddEntry("first", first->id);
        logger->AddEntry("second", second->id);
      logger->EndObject();
    }

    uint8_t Width() const { return data.size(); }
    uint8_t Height() const { return (data.size() > 0) ? data[0].size() : 0; }

    // Accessors
    std::shared_ptr<Logger> GetLogger() { return this->logger; }
    Vector<std::shared_ptr<Cell>>& operator[] (size_t n) { return this->data[n]; }
    const Vector<std::shared_ptr<Cell>>& operator[] (size_t n) const { return this->data[n]; }

  private:
    Grid operator=(Grid&);                      // Not Implemented
    std::shared_ptr<Logger> logger;             // Logger
    Vector<Vector<std::shared_ptr<Cell>>> data; // Grid wrapper
    mutable Stats stats;                        // Computation statistics

    void Init(uint8_t width, uint8_t height, bool isConneccted)
    {
      // Generate the Grid
      this->data.reserve(width);
      for (uint8_t x = 0; x < width; ++x)
      {
        this->data.push_back(Vector<std::shared_ptr<Cell>>(this->logger));
        this->data[x].reserve(height);
        for (uint8_t y = 0; y < height; ++y)
        {
          this->data[x].push_back(std::shared_ptr<Cell>(new Cell(x, y)));

          // Connect West
          if (isConneccted && x > 0)
            this->Connect(this->data[x][y], this->data[x-1][y], true);

          // Connect North
          if (isConneccted && y > 0)
            this->Connect(this->data[x][y], this->data[x][y-1], true);
        }
      }
    }
  };
}
#endif // MODULE_LOGGER_GRID_HXX
