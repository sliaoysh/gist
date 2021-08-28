#include "game_board.h"
#include "util.h"

#include <chrono>
#include <thread>

int main(int argc, char* argv[])
{
    constexpr char LIFE = 'O';
    using namespace std::chrono_literals;

    if (argc != 2)
    {
        print_usage();
        return 1;
    }

    const auto [rows, cols] = get_terminal_size();
    const std::vector<std::string> config = read_config(argv[1]);
    std::vector<std::string> initial_state = create_init_state(config, rows, cols, LIFE);
    game_board board(std::move(initial_state), LIFE);

    while (true)
    {
        std::cout << "\033[2J\033[0;0f";
        board.output(std::cout);
        std::cout << std::flush;
        board.evolve();
        std::this_thread::sleep_for(100ms);
    }
    return 0;
}
