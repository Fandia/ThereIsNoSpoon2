#include <vector>
#include <array>
#include <deque>
#include <unordered_set>
#include <cassert>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <cstdint>
#include <climits>

using namespace std;

constexpr unsigned char kMaxBridges = 2;
constexpr signed char kEmptyCell = -(kMaxBridges + 1);
constexpr unsigned char kMaxNeighbours = 8;
enum class ActionType : signed char { add_1 = 1, add_2 = 2, remove_1 = -1, remove_2 = -2, check = 0 };
enum class NeighbourType : unsigned char { top, bottom, left, right };
using SizeType = unsigned int;

struct Position {
  SizeType row;
  SizeType col;
};

struct Cell;
struct Neighbours {
  Cell* left;
  Cell* right;
  Cell* top;
  Cell* bottom;

  unsigned char NeighboursCount() {
    unsigned char count = 0;
    !left ? count : count++;
    !right ? count : count++;
    !top ? count : count++;
    !bottom ? count : count++;
    return count;
  }
};

struct Bridges {
  signed char left = 0;
  signed char right = 0;
  signed char top = 0;
  signed char bottom = 0;

  signed char GetSum() {
    return left + right + top + bottom;
  }

  unsigned char NeighboursCount() {
    unsigned char count = 0;
    left > 0 ? ++count : count;
    right > 0 ? ++count : count;
    top > 0 ? ++count : count;
    bottom > 0 ? ++count : count;
    return count;
  }
};

struct Cell {
  Neighbours neighbours;
  Position position;
  Bridges bridges;
  signed char capacity;
  bool in_group = false;
};

class Hashiwokakero {
private:
  SizeType width_ = 0;
  SizeType height_ = 0;
  vector<Cell> cells_;
  vector<Cell*> islands_;
  vector<Cell*> nonresolved_;
  deque<Cell*> bfs_queue_;

  auto GetCell(SizeType row, SizeType col) {
    assert(row < height_ && col < width_);
    return &cells_[row * width_ + col];
  }

  void DoBridges(Cell* from, Bridges bridges) {
    const auto do_bridge = [this, from](Cell* neighbour, signed char& from_bridge, 
      signed char& neighbour_bridge, signed char bridge_inc){
      from_bridge += bridge_inc;
      neighbour_bridge += bridge_inc;
      from->capacity -= bridge_inc;
      neighbour->capacity -= bridge_inc;
      Obstacles(from->position, neighbour->position, (ActionType)bridge_inc);
    };
    if (from->neighbours.bottom && bridges.bottom)
      do_bridge(from->neighbours.bottom, from->bridges.bottom, from->neighbours.bottom->bridges.top, bridges.bottom);
    if (from->neighbours.left && bridges.left)
      do_bridge(from->neighbours.left, from->bridges.left, from->neighbours.left->bridges.right, bridges.left);
    if (from->neighbours.right && bridges.right)
      do_bridge(from->neighbours.right, from->bridges.right, from->neighbours.right->bridges.left, bridges.right);
    if (from->neighbours.top && bridges.top)
      do_bridge(from->neighbours.top, from->bridges.top, from->neighbours.top->bridges.bottom, bridges.top);
  }

  // Add or remove obstacles betweel cells, else checking obstacles existence
  bool Obstacles(Position from, Position to, ActionType action) {
    int h_range = to.col - from.col;
    int v_range = to.row - from.row;
    auto h_range_abs = abs(h_range);
    auto v_range_abs = abs(v_range);
    if (h_range_abs > 1) {
      signed char h_it = (signed char)(h_range / h_range_abs);
      for (SizeType col = from.col + h_it; col != to.col; col += h_it)
        if (action == ActionType::check) {
          if (GetCell(from.row, col)->position.col)
            return true;
        }
        else
          GetCell(from.row, col)->position.row += (signed char)action;
    }
    if (v_range_abs > 1) {
      signed char v_it = (signed char)(v_range / v_range_abs);
      for (SizeType row = from.row + v_it; row != to.row; row += v_it)
        if (action == ActionType::check) {
          if (GetCell(row, from.col)->position.row)
            return true;
        }
        else
          GetCell(row, from.col)->position.col += (signed char)action;
    }
    return false;
  }

  void SetNeighbours() {
    cerr << "Setting neighbours..." << endl;
    for (auto island : islands_)
      for (unsigned char neighbour_type = 0; neighbour_type < 4; neighbour_type++) {
        int h_limit = -1;
        int v_limit = -1;
        signed char h_it = 0;
        signed char v_it = 0;
        int row = island->position.row;
        int col = island->position.col;
        Cell** neighbour_cell = nullptr;
        switch ((NeighbourType)neighbour_type) {
          case NeighbourType::top:
            v_it = -1;
            neighbour_cell = &island->neighbours.top;
            break;
          case NeighbourType::bottom:
            v_limit = height_;
            v_it = 1;
            neighbour_cell = &island->neighbours.bottom;
            break;
          case NeighbourType::left:
            h_it = -1;
            neighbour_cell = &island->neighbours.left;
            break;
          case NeighbourType::right:
            h_limit = width_;
            h_it = 1;
            neighbour_cell = &island->neighbours.right;
            break;
          default:
            break;
        }
        for (row += v_it, col += h_it; row != v_limit && col != h_limit; row += v_it, col += h_it) {
          auto current_neighbour = GetCell((SizeType)row, (SizeType)col);
          if (current_neighbour->capacity != kEmptyCell) {
            *neighbour_cell = current_neighbour;
            break;
          }
        }
      }
    cerr << "Neighbours have been set" << endl;
  }

  Bridges GetPossibleBridges(Cell* from) {
    Bridges possible_bridges = { 0 };
    if (from->neighbours.bottom && !Obstacles(from->position, from->neighbours.bottom->position, ActionType::check))
      possible_bridges.bottom = 
        min(from->capacity, min(from->neighbours.bottom->capacity, (signed char)(kMaxBridges - from->bridges.bottom)));
    if (from->neighbours.left && !Obstacles(from->position, from->neighbours.left->position, ActionType::check))
      possible_bridges.left =
        min(from->capacity, min(from->neighbours.left->capacity, (signed char)(kMaxBridges - from->bridges.left)));
    if (from->neighbours.right && !Obstacles(from->position, from->neighbours.right->position, ActionType::check))
      possible_bridges.right =
        min(from->capacity, min(from->neighbours.right->capacity, (signed char)(kMaxBridges - from->bridges.right)));
    if (from->neighbours.top && !Obstacles(from->position, from->neighbours.top->position, ActionType::check))
      possible_bridges.top =
        min(from->capacity, min(from->neighbours.top->capacity, (signed char)(kMaxBridges - from->bridges.top)));
    assert(possible_bridges.bottom >= 0 && possible_bridges.left >= 0 && possible_bridges.right >= 0 && possible_bridges.top >= 0);
    return possible_bridges;
  }

  bool HeuristicSolver() {
    cerr << "Heuristic solver started..." << endl;
    bool continue_loop = false;
    do {
      continue_loop = false;
      for (auto island : islands_)
        if (island->capacity != 0) {
          auto possible_bridges = GetPossibleBridges(island);
          assert(*(unsigned int*)&possible_bridges);
          auto possible_neighbours = possible_bridges.NeighboursCount();
          auto bridges_sum = possible_bridges.GetSum();
          if (bridges_sum == island->capacity || (island->capacity == 1 && possible_neighbours == 1)) {
            DoBridges(island, possible_bridges);
            continue_loop = true;
          } else if ((bridges_sum - 1 == island->capacity) && (island->capacity < possible_neighbours) && (island->capacity != 1)) {
            Bridges one_bridges = { 1, 1, 1, 1 };
            one_bridges.bottom = min(one_bridges.bottom, possible_bridges.bottom);
            one_bridges.left = min(one_bridges.left, possible_bridges.left);
            one_bridges.right = min(one_bridges.right, possible_bridges.right);
            one_bridges.top = min(one_bridges.top, possible_bridges.top);
            DoBridges(island, one_bridges);
            continue_loop = true;
          }
        }
    } while (continue_loop);

    // Check if heuristics are solved puzzle
    for (auto island : islands_)
      if (island->capacity != 0)
        nonresolved_.emplace_back(island);
    if (nonresolved_.empty())
        cerr << "Heuristic solver success" << endl;
    else
        cerr << "Heuristic solver failed" << endl;
    return nonresolved_.empty();
  }

  // Recursive backtracking
  bool TryRoute(size_t nonresolved_idx) {
    auto island = nonresolved_[nonresolved_idx];
    if (!island->capacity) {
      if (nonresolved_idx == nonresolved_.size() - 1)
        return CheckSingleGroup();
      else
        return TryRoute(++nonresolved_idx);
    }
    auto possible_bridges = GetPossibleBridges(island);
    if (!*(unsigned int*)&possible_bridges)
      return false;

    Bridges bridges_attempt = { 0, 0, 0, 0 };

    const auto do_attempt = [this, island, nonresolved_idx, &bridges_attempt](signed char& attempt_bridge) {
      bridges_attempt = { 0 };
      attempt_bridge = 1;
      DoBridges(island, bridges_attempt);
      if (!TryRoute(nonresolved_idx)) {
        attempt_bridge = -1;
        DoBridges(island, bridges_attempt);
        return false;
      } else {
        return true;
      }
    };

    bool success = false;
    if (possible_bridges.bottom)
      success = do_attempt(bridges_attempt.bottom);
    if (!success && possible_bridges.left)
      success = do_attempt(bridges_attempt.left);
    if (!success && possible_bridges.right)
      success = do_attempt(bridges_attempt.right);
    if (!success && possible_bridges.top)
      success = do_attempt(bridges_attempt.top);
    return success;
  }

  bool CheckSingleGroup() {
    bfs_queue_.clear();
    bfs_queue_.emplace_back(islands_.front());
    unsigned int group_size = 0;
    while (!bfs_queue_.empty()) {
      auto island = bfs_queue_.front();
      bfs_queue_.pop_front();
      if (island->in_group)
        continue;
      island->in_group = true;

      if (island->neighbours.bottom && !island->neighbours.bottom->in_group && island->bridges.bottom)
        bfs_queue_.emplace_back(island->neighbours.bottom);
      if (island->neighbours.left && !island->neighbours.left->in_group && island->bridges.left)
        bfs_queue_.emplace_back(island->neighbours.left);
      if (island->neighbours.right && !island->neighbours.right->in_group && island->bridges.right)
        bfs_queue_.emplace_back(island->neighbours.right);
      if (island->neighbours.top && !island->neighbours.top->in_group && island->bridges.top)
        bfs_queue_.emplace_back(island->neighbours.top);

      group_size++;
    }

    for (auto& island : islands_)
      island->in_group = false;

    return group_size == islands_.size();
  }

public:
  Hashiwokakero() {
	// the number of cells on the X axis
	cin >> width_; cin.ignore();
	cerr << width_ << endl;
	// the number of cells on the Y axis
	cin >> height_; cin.ignore();
	cerr << height_ << endl;

	assert(width_ <= USHRT_MAX && height_ <= USHRT_MAX);
	
    size_t cells_count = height_ * width_;
	cells_.resize(cells_count);
    islands_.reserve(cells_count);
    nonresolved_.reserve(cells_count);

	for (SizeType i = 0; i < height_; i++) {
	  string line; // width characters, each either a number or a '.'
	  getline(cin, line);
	  cerr << line << endl;
      for (SizeType j = 0; j < width_; j++) {
        auto cell = GetCell(i, j);
        if (line[j] == '.')
          cell->capacity = kEmptyCell; // empty space
        else {
          unsigned char capacity = (uint8_t)(line[j]) - 48;
          cell->capacity = capacity;
          cell->position = { i, j };
          islands_.emplace_back(cell);
        }
      }
	}

    SetNeighbours();
  }

  bool Solve() {
    cerr << "Solving started..." << endl;
    if (HeuristicSolver())
        return true;
    else {
        cerr << "Backtracking started..." << endl;
        return TryRoute(0);
    }
  }

  void CodinGamePrint() {
    unordered_set<Cell*> from;
    const auto print_connection = [](Position from, Position to, unsigned char bridges) {
      if (bridges)
        cout << (int)from.col << ' ' << (int)from.row << ' ' << 
          (int)to.col << ' ' << (int)to.row << ' ' << (int)bridges << endl;
    };
    for (auto island : islands_) {
      from.emplace(island);
      if (island->neighbours.bottom && !from.count(island->neighbours.bottom))
        print_connection(island->position, island->neighbours.bottom->position, island->bridges.bottom);
      if (island->neighbours.left && !from.count(island->neighbours.left))
        print_connection(island->position, island->neighbours.left->position, island->bridges.left);
      if (island->neighbours.right && !from.count(island->neighbours.right))
        print_connection(island->position, island->neighbours.right->position, island->bridges.right);
      if (island->neighbours.top && !from.count(island->neighbours.top))
        print_connection(island->position, island->neighbours.top->position, island->bridges.top);
    }
  }
};

int main() {
  Hashiwokakero solver;
  if (solver.Solve())
    solver.CodinGamePrint();
  return 0;
}
