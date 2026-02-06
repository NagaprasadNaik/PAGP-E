#include <iostream>
#include <cmath>
#include <cstring>
#include <omp.h>
using namespace std;

double t = 0.0;

// ---------------------------------------------------------
// Strike function – marks composite numbers
// ---------------------------------------------------------
inline long Strike(bool composite[], long i, long stride, long limit)
{
    for (; i <= limit; i += stride)
        composite[i] = true;
    return i;
}

// ---------------------------------------------------------
// Cache–UNFRIENDLY sieve
// ---------------------------------------------------------
long CacheUnfriendlySieve(long n)
{
    long count = 0;
    long m = (long)sqrt((double)n);

    bool* composite = new bool[n + 1];
    memset(composite, 0, n);

    t = omp_get_wtime();

    for (long i = 2; i <= m; i++)
    {
        if (!composite[i])
        {
            ++count;
            Strike(composite, 2 * i, i, n);   // Bad cache access
        }
    }

    for (long i = m + 1; i <= n; i++)
        if (!composite[i]) ++count;

    t = omp_get_wtime() - t;
    delete[] composite;

    return count;
}

// ---------------------------------------------------------
// Cache–FRIENDLY sieve (Blocked Sieve Method)
// ---------------------------------------------------------
long CacheFriendlySieve(long n)
{
    long count = 0;
    long m = (long)sqrt((double)n);

    bool* composite = new bool[n + 1];
    memset(composite, 0, n);

    long* factor = new long[m];
    long* striker = new long[m];

    long n_factor = 0;
    t = omp_get_wtime();

    // Preprocess small primes
    for (long i = 2; i <= m; i++)
    {
        if (!composite[i])
        {
            ++count;
            striker[n_factor] = Strike(composite, 2 * i, i, m);
            factor[n_factor++] = i;
        }
    }

    // Blocked sieve
    for (long window = m + 1; window <= n; window += m)
    {
        long limit = min(window + m - 1, n);

        for (long k = 0; k < n_factor; k++)
            striker[k] = Strike(composite, striker[k], factor[k], limit);

        for (long i = window; i <= limit; i++)
            if (!composite[i]) ++count;
    }

    t = omp_get_wtime() - t;

    delete[] striker;
    delete[] factor;
    delete[] composite;

    return count;
}

// ---------------------------------------------------------
// MAIN
// ---------------------------------------------------------
int main()
{
    cout << "\nRunning Cache-Unfriendly Sieve...\n";
    long count = CacheUnfriendlySieve(100000000);
    cout << "Primes = " << count << endl;
    cout << "Time (unfriendly): " << t << " sec\n\n";

    cout << "Running Cache-Friendly Sieve...\n";
    count = CacheFriendlySieve(100000000);
    cout << "Primes = " << count << endl;
    cout << "Time (friendly): " << t << " sec\n";

    return 0;
}
