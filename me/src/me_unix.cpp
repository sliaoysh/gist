#include "processor_unix.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

vector<fun::client::InstrumentId> read_instrument_id(const char* inst_file_path)
{
    ifstream inst_file(inst_file_path);
    string str;
    inst_file >> str;
    int inst;
    vector<fun::client::InstrumentId> instruments;

    while (inst_file >> inst)
    {
        instruments.push_back(inst);
    }
    return instruments;
}

int main(int argc, const char* argv[])
{
    if (argc != 4)
    {
        cout << "Usage: me unix_domain_socket_name -i [instrument_file]" << endl;
        return 1;
    }

    const char* sock_name = argv[1];
    const char* inst_file_path = argv[3];

    vector<fun::client::InstrumentId> instruments = read_instrument_id(inst_file_path);
    processor pro{sock_name, instruments};
    std::cout << "start..." << std::endl;

    try
    {
        pro.run();
    }
    catch(...)
    {
        cout << "end" << endl;
    }
    return 0;
}
