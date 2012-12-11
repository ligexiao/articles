#include <chrono>

#include "graphs.hpp"

// chrono typedefs

using std::chrono::milliseconds;
using std::chrono::microseconds;

using Clock = std::chrono::high_resolution_clock;

// Number of repetitions of each test

static const std::size_t REPEAT = 7;
  
// variadic policy runner

template<class Container>
inline static void run(Container &, std::size_t){
    //End of recursion
}

template<template<class> class Test, template<class> class ...Rest, class Container>
inline static void run(Container &container, std::size_t size){
    Test<Container>::run(container, size);
    run<Rest...>(container, size);
}

// benchmarking procedure

template<typename Container,
         typename DurationUnit,
         template<class> class CreatePolicy,
         template<class> class ...TestPolicy>
void bench(const std::string& type, const std::initializer_list<int> &sizes){
    // create an element to copy so the temporary creation
    // and initialization will not be accounted in a benchmark
    typename Container::value_type value;
    for(auto size : sizes) {
        Clock::duration duration;

        for(std::size_t i=0; i<REPEAT; ++i) {
            auto container = CreatePolicy<Container>::make(size);

            Clock::time_point t0 = Clock::now();
            
            run<TestPolicy...>(container, size);

            Clock::time_point t1 = Clock::now();
            duration += t1 - t0;
        }

        graphs::new_result(type, std::to_string(size), std::chrono::duration_cast<DurationUnit>(duration).count() / REPEAT);
    }
}

template<template<class> class Benchmark>
void bench_types(){
    //Recursion end
}

template<template<class> class Benchmark, typename T, typename ...Types>
void bench_types(){  
    Benchmark<T>::run();
    bench_types<Benchmark, Types...>();
}