#include "objects.hpp"

#include "../runtime/runtime.hpp"

namespace anole
{
ContObject::ContObject(SPtr<Context> resume)
  : Object(ObjectType::Continuation)
  , resume_(std::make_shared<Context>(*resume))
{
    // ...
}

SPtr<Context> ContObject::resume()
{
    return resume_;
}

void ContObject::call(Size n)
{
    if (n != 1)
    {
        throw RuntimeError("continuation need a argument");
    }
    auto retval = theCurrContext->pop_ptr();
    theCurrContext = std::make_shared<Context>(resume_);
    theCurrContext->push(retval);
    ++theCurrContext->pc();
}

void ContObject::collect(std::function<void(Context *)> func)
{
    func(resume_.get());
}
}
