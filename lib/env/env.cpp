#include "../../src/anole.hpp"

#include <vector>

using namespace std;
using namespace anole;

extern "C"
{
vector<String> _FUNCTIONS
{
    "__args"s
};

void __args(Size n)
{
    if (n != 0)
    {
        throw RuntimeError("args need no arguments");
    }

    auto raw_args = Context::get_args();

    auto args = Allocator<Object>::alloc<ListObject>();
    for (auto arg : raw_args)
    {
        args->append(Allocator<Object>::alloc<StringObject>(String(arg)));
    }

    Context::current()->push(args);
}
}
