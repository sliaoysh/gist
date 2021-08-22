#include <iostream>
#include <string>
#include <vector>

class game_board
{
public:
    explicit game_board(std::vector<std::string>&& initial_state, char life);
    void evolve();
    void output(std::ostream& os, char delim='\0') const;

private:
    void count_neighbor();
    void clear_neighbor_ctr();
    void update_neighbor_ctr(int row, int col);
    void update_board();

    std::vector<std::string> board_;
    std::vector<std::vector<int>> neighbor_ctr_;
    const char life_;
};
