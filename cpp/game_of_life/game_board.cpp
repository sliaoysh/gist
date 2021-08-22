#include "game_board.h"

#include <algorithm>
#include <array>
#include <iterator>
#include <string_view>
#include <utility>

game_board::game_board(std::vector<std::string>&& initial_state, const char life)
        : board_(std::move(initial_state)),
          neighbor_ctr_(board_.size(), std::vector<int>(board_[0].size())),
          life_{life}
{
}

void game_board::evolve()
{
    count_neighbor();
    update_board();
}

void game_board::count_neighbor()
{
    clear_neighbor_ctr();

    for (size_t i = 1; i < neighbor_ctr_.size()-1; ++i)
    {
        for (size_t j = 1; j < neighbor_ctr_[0].size()-1; ++j)
        {
            if (board_[i][j] != ' ')
            {
                update_neighbor_ctr(i, j);
            }
        }
    }
}

void game_board::update_neighbor_ctr(const int row, const int col)
{
    constexpr std::array<std::pair<int,int>,8> offset = {{{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}}};
    neighbor_ctr_[row][col] += 10;
    for (const auto& [i, j] : offset)
    {
        neighbor_ctr_[row+i][col+j] += 1;
    }
}

void game_board::update_board()
{
    for (size_t i = 1; i < neighbor_ctr_.size()-1; ++i)
    {
        for (size_t j = 1; j < neighbor_ctr_[0].size()-1; ++j)
        {
            if (neighbor_ctr_[i][j] == 3 || neighbor_ctr_[i][j] == 12 || neighbor_ctr_[i][j] == 13)
            {
                board_[i][j] = life_;
            }
            else
            {
                board_[i][j] = ' ';
            }
        }
    }
}

void game_board::clear_neighbor_ctr()
{
    for (auto& v : neighbor_ctr_)
    {
        std::fill(begin(v), end(v), 0);
    }
}

void game_board::output(std::ostream& os, const char delim) const
{
    std::for_each(next(cbegin(board_)), next(cbegin(board_), board_.size()-1),
                  [&os, delim](const auto& line)
                  {
                        os << std::string_view{next(cbegin(line)), next(cbegin(line), line.size()-1)}
                           << delim;
                  });
}
