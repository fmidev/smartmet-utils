#include <sqlite3.h>

int main()
{
    static_assert(SQLITE_VERSION_NUMBER >= 3030001, "Too old sqlite3");
    return 0;
}
