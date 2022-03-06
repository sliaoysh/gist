#include <iostream>
using namespace std;

enum class op
{
    ptr_inc,     // >
    ptr_dec,     // <
    data_inc,    // +
    data_dec,    // -
    write,       // .
    read,        // ,
    jmp_ifz,     // [, jump if zero
    jmp,         // ], unconditional jump
};

template <size_t InstructionCapacity>
struct program
{
    size_t inst_count;
    op     inst[InstructionCapacity];
    size_t inst_jmp[InstructionCapacity];
};

template <size_t N>
constexpr auto parse(const char (&str)[N])
{
    program<N> result{};

    size_t jump_stack[N] = {};
    size_t jump_stack_top = 0;

    for (auto ptr = str; *ptr; ++ptr)
    {
        if (*ptr ==  '>')
            result.inst[result.inst_count++] = op::ptr_inc;
        else if (*ptr ==  '<')
            result.inst[result.inst_count++] = op::ptr_dec;
        else if (*ptr ==  '+')
            result.inst[result.inst_count++] = op::data_inc;
        else if (*ptr ==  '-')
            result.inst[result.inst_count++] = op::data_dec;
        else if (*ptr ==  '.')
            result.inst[result.inst_count++] = op::write;
        else if (*ptr ==  ',')
            result.inst[result.inst_count++] = op::read;
        else if (*ptr == '[')
        {
            jump_stack[jump_stack_top++] = result.inst_count;
            result.inst[result.inst_count++] = op::jmp_ifz;
        }
        else if (*ptr == ']')
        {
            auto open = jump_stack[--jump_stack_top];
            auto close = result.inst_count++;

            result.inst[close] = op::jmp;
            result.inst_jmp[close] = open;

            result.inst_jmp[open] = close + 1;
        }
    }
    return result;
}

template <const auto& Program, std::size_t InstPtr = 0>
constexpr void execute(unsigned char* data_ptr)
{
    if constexpr (InstPtr >= Program.inst_count)
    {
        // Execution is finished.
        return;
    }
    else if constexpr (Program.inst[InstPtr] == op::ptr_inc)
    {
        ++data_ptr;
        return execute<Program, InstPtr + 1>(data_ptr);
    }
    else if constexpr (Program.inst[InstPtr] == op::ptr_dec)
    {
        --data_ptr;
        return execute<Program, InstPtr + 1>(data_ptr);
    }
    else if constexpr (Program.inst[InstPtr] == op::data_inc)
    {
        ++*data_ptr;
        return execute<Program, InstPtr + 1>(data_ptr);
    }
    else if constexpr (Program.inst[InstPtr] == op::data_dec)
    {
        --*data_ptr;
        return execute<Program, InstPtr + 1>(data_ptr);
    }
    else if constexpr (Program.inst[InstPtr] == op::write)
    {
        std::putchar(*data_ptr);
        return execute<Program, InstPtr + 1>(data_ptr);
    }
    else if constexpr (Program.inst[InstPtr] == op::read)
    {
        *data_ptr = static_cast<char>(std::getchar());
        return execute<Program, InstPtr + 1>(data_ptr);
    }
    else if constexpr (Program.inst[InstPtr] == op::jmp_ifz)
    {
        if (*data_ptr == 0)
            return execute<Program, Program.inst_jmp[InstPtr]>(data_ptr);
        else
            return execute<Program, InstPtr + 1>(data_ptr);
    }
    else if constexpr (Program.inst[InstPtr] == op::jmp)
    {
        return execute<Program, Program.inst_jmp[InstPtr]>(data_ptr);
    }
}

int main()
{
    // `x = getchar(); y = x + 3; putchar(y);`
    static constexpr auto add3 = parse(",>+++<[->+<]>.");

    unsigned char memory[1024] = {};
    execute<add3>(memory);

    return 0;
}
