#include <lemon/abi.h>
#include <lemon/lnode.hpp>

using namespace lemon;

LEMON_API int start_lemon_service(const lemon_conf * conf)
{
    try
    {
        lnode node(conf);
        node.run();
    }
    catch (const std::exception &)
    {
        return 1;
    }

    return 0;
}
