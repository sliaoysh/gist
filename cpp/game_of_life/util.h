#include <string>
#include <utility>
#include <vector>

std::vector<std::string> read_config(const char* config_path);
std::vector<std::string> create_init_state(const std::vector<std::string>& config, size_t rows, size_t cols, char life);
std::pair<size_t,size_t> get_terminal_size();
void print_usage();
