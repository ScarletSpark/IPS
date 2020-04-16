#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_max.h>
#include <cilk/reducer_min.h>
#include <cilk/reducer_vector.h>
#include <chrono>
#include <iostream>
#include <vector>
#include <cilk/reducer_vector.h>
#include <iomanip> 

using namespace std::chrono;

/// ‘ункци€ ReducerMaxTest() определ€ет максимальный элемент массива,
/// переданного ей в качестве аргумента, и его позицию
/// mass_pointer - указатель исходный массив целых чисел
/// size - количество элементов в массиве
void ReducerMaxTest(int *mass_pointer, const long size)
{
	cilk::reducer<cilk::op_max_index<long, int>> maximum;
	cilk_for(long i = 0; i < size; ++i)
	{
		maximum->calc_max(i, mass_pointer[i]);
	}
	printf("\nMaximal element = %d has index = %d\n",
		maximum->get_reference(), maximum->get_index_reference());
}

void ReducerMinTest(int *mass_pointer, const long size)
{
	cilk::reducer<cilk::op_min_index<long, int>> minimum;
	cilk_for(long i = 0; i < size; ++i)
	{
		minimum->calc_min(i, mass_pointer[i]);
	}
	printf("Minimal element = %d has index = %d\n",
		minimum->get_reference(), minimum->get_index_reference());
}

/// ‘ункци€ ParallelSort() сортирует массив в пор€дке возрастани€
/// begin - указатель на первый элемент исходного массива
/// end - указатель на последний элемент исходного массива
void ParallelSort(int *begin, int *end)
{
	if (begin != end) 
	{
		--end;
		int *middle = std::partition(begin, end, std::bind2nd(std::less<int>(), *end));
		std::swap(*end, *middle); 
		cilk_spawn ParallelSort(begin, middle);
		ParallelSort(++middle, ++end);
		cilk_sync;
	}
}

void CompairForAndCilk_For(size_t sz)
{
	std::vector<int> k;
	auto ft1 = high_resolution_clock::now();
	for (size_t i = 0; i < sz; ++i)
	{
		k.push_back(rand() % 20000 + 1);
	}
	auto ft2 = high_resolution_clock::now();
	duration<double> f_duration = (ft2 - ft1);
	std::cout << std::setw(30) << "Duration of FOR is: " << f_duration.count() << " seconds" << std::endl;

	cilk::reducer<cilk::op_vector<int>> red_vec;
	ft1 = high_resolution_clock::now();
	cilk_for(size_t i = 0; i < sz; ++i)
	{
		red_vec->push_back(rand() % 20000 + 1);
	}
	ft2 = high_resolution_clock::now();
	f_duration = (ft2 - ft1);
	std::cout << std::setw(30) << "Duration of CILK_FOR is: " << f_duration.count() << " seconds" << std::endl;

}


int main()
{
	std::cout << std::setiosflags(std::ios::left);
	
	srand((unsigned)time(0));

	// устанавливаем количество работающих потоков = 4
	__cilkrts_set_param("nworkers", "4");

	long i;
	const long mass_size = 1000000;
	int *mass_begin, *mass_end;
	int *mass = new int[mass_size]; 

	for(i = 0; i < mass_size; ++i)
	{
		mass[i] = (rand() % 25000) + 1;
	}
	
	mass_begin = mass;
	mass_end = mass_begin + mass_size;
	ReducerMaxTest(mass, mass_size);
	ReducerMinTest(mass, mass_size);

	auto t1 = high_resolution_clock::now();
	ParallelSort(mass_begin, mass_end);
	auto t2 = high_resolution_clock::now();
	duration<double> duration = (t2 - t1);
	std::cout << "\nSort for array of size " << mass_size << std::endl;
	std::cout << "\nDuration is: " << duration.count() << " seconds" << std::endl;
	ReducerMaxTest(mass, mass_size);
	ReducerMinTest(mass, mass_size);

	std::cout << "\nArray size: 10" << std::endl;
	CompairForAndCilk_For(10);
	std::cout << "\nArray size: 50" << std::endl;
	CompairForAndCilk_For(50);
	std::cout << "\nArray size: 100" << std::endl;
	CompairForAndCilk_For(100);
	std::cout << "\nArray size: 500" << std::endl;
	CompairForAndCilk_For(500);
	std::cout << "\nArray size: 1000" << std::endl;
	CompairForAndCilk_For(1000);
	std::cout << "\nArray size: 10000" << std::endl;
	CompairForAndCilk_For(10000);
	std::cout << "\nArray size: 100000" << std::endl;
	CompairForAndCilk_For(100000);
	std::cout << "\nArray size: 1000000" << std::endl;
	CompairForAndCilk_For(1000000);

	delete[]mass;
	return 0;
}