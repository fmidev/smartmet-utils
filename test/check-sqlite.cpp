#include <sqlite3.h>

int main()
{
    static_assert(SQLITE_VERSION_NUMBER >= 3026000, "Too old sqlite3");
    return 0;
}
