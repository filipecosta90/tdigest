#include <benchmark/benchmark.h>
#include "tdigest.h"
#include <math.h>
#include <random>

#ifdef _WIN32
#pragma comment(lib, "Shlwapi.lib")
#ifdef _DEBUG
#pragma comment(lib, "benchmarkd.lib")
#else
#pragma comment(lib, "benchmark.lib")
#endif
#endif

int64_t min_value = 1;
int64_t min_compression = 100;
int64_t max_compression = 500;
int64_t step_compression_unit = 100;

static void generate_arguments_pairs(benchmark::internal::Benchmark *b)
{
    for (int64_t compression = min_compression; compression <= max_compression; compression += step_compression_unit)
    {
        b = b->ArgPair((double)compression, INT64_C(10000000));
    }
}

static void BM_td_add_uniform_dist(benchmark::State &state)
{
    const double compression = state.range(0);
    const int64_t stream_size = state.range(1);
    td_histogram_t *mdigest = td_new(compression);
    std::vector<double> input;
    input.resize(stream_size, 0);
    std::mt19937_64 rng;
    rng.seed(std::random_device()());
    std::uniform_real_distribution<double> dist(0, 1);

    for (double &i : input)
    {
        i = dist(rng);
    }

    while (state.KeepRunning())
    {
        for (int i = 0; i < stream_size; ++i)
        {
            td_add(mdigest, input[i], 1);
        }
        td_compress(mdigest);
        // read/write barrier
        benchmark::ClobberMemory();
        state.SetItemsProcessed(stream_size);
        // Set the counter as a thread-average quantity. It will
        // be presented divided by the number of threads ( in our case just one thread ).
        state.counters["Centroid_Count"] = benchmark::Counter(td_centroid_count(mdigest), benchmark::Counter::kAvgThreads);
        state.counters["Total_Compressions"] = benchmark::Counter(mdigest->total_compressions, benchmark::Counter::kAvgThreads);
    }
}


static void BM_td_add_lognormal_dist(benchmark::State &state)
{
    const double compression = state.range(0);
    const int64_t stream_size = state.range(1);
    td_histogram_t *mdigest = td_new(compression);
    std::vector<double> input;
    input.resize(stream_size, 0);
    std::mt19937_64 rng;
    rng.seed(std::random_device()());
    std::lognormal_distribution<double> dist(1,0.5);

    for (double &i : input)
    {
        i = dist(rng);
    }

    while (state.KeepRunning())
    {
        for (int i = 0; i < stream_size; ++i)
        {
            td_add(mdigest, input[i], 1);
        }
        td_compress(mdigest);
        // read/write barrier
        benchmark::ClobberMemory();
        state.SetItemsProcessed(stream_size);
        // Set the counter as a thread-average quantity. It will
        // be presented divided by the number of threads ( in our case just one thread ).
        state.counters["Centroid_Count"] = benchmark::Counter(td_centroid_count(mdigest), benchmark::Counter::kAvgThreads);
        state.counters["Total_Compressions"] = benchmark::Counter(mdigest->total_compressions, benchmark::Counter::kAvgThreads);
    }
}

// Register the functions as a benchmark
BENCHMARK(BM_td_add_uniform_dist)->Apply(generate_arguments_pairs);
BENCHMARK(BM_td_add_lognormal_dist)->Apply(generate_arguments_pairs);

BENCHMARK_MAIN();