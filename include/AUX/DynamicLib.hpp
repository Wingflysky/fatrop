#ifndef DYNAMICLIBINCLUDED
#define DYNAMICLIBINCLUDED
#include <dlfcn.h>
#include <string>
using namespace std;
namespace fatrop
{
    class DLLoader
    {
    public:
        DLLoader(const string &filename)
        {
            handle = dlopen(&filename[0], RTLD_LAZY);
            if (handle == 0)
            {
                printf("Cannot open f.so, error %s\n", dlerror());
            }
        }
        ~DLLoader()
        {
            free(handle);
        }
        void *handle;
    };
};     // namespace fatrop
#endif // DYNAMICLIBINCLUDED