#include "util.h"

#include <fstream>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

std::vector<std::string> read_config(const char* const config_path)
{
    std::ifstream config_file(config_path);
    if (!config_file.is_open())
    {
        throw "Error opeing config file";
    }

    std::vector<std::string> config;
    std::string line;

    while (std::getline(config_file, line))
    {
        config.push_back(std::move(line));
    }
    return config;
}

std::vector<std::string> create_init_state(const std::vector<std::string>& config, const size_t rows, const size_t cols, const char life)
{
    std::vector<std::string> init_state(rows+2, std::string(cols+2, ' '));

    for (size_t i = 0; i < config.size() && i < rows; ++i)
    {
        for (size_t j = 0; j < config[i].size() && j < cols; ++j)
        {
            if (config[i][j] != ' ')
            {
                init_state[i+1][j+1] = life;
            }
        }
    }
    return init_state;
}

std::pair<size_t,size_t> get_terminal_size()
{
    winsize terminal_size;
    const int ret = ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal_size);
    if (ret)
    {
        throw "Error get_terminal_size";
    }
    return {terminal_size.ws_row, terminal_size.ws_col};
}

void print_usage()
{
    std::cout << "Usage: game_of_life config_file" << std::endl;
}
