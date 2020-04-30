#include <vector>
#include <thread>
#include <stdio.h>
#include <exception>
#include <locale.h>
#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>
#include <chrono>

/// перечисление, определ€ющее как будет происходить вычисление
/// средних значений матрицы: по строкам или по столбцам
enum class eprocess_type
{
	by_rows = 0,
	by_cols
};

void InitMatrix( double** matrix, const size_t numb_rows, const size_t numb_cols )
{
	for ( size_t i = 0; i < numb_rows; ++i )
	{
		for ( size_t j = 0; j < numb_cols; ++j )
		{
			matrix[i][j] = rand() % 5 + 1;
		}
	}
}

/// ‘ункци€ PrintMatrix() печатает элементы матрицы <i>matrix</i> на консоль;
/// numb_rows - количество строк в исходной матрице <i>matrix</i>
/// numb_cols - количество столбцов в исходной матрице <i>matrix</i>
void PrintMatrix( double** matrix, const size_t numb_rows, const size_t numb_cols )
{
	printf( "Generated matrix:\n" );
	for ( size_t i = 0; i < numb_rows; ++i )
	{
		for ( size_t j = 0; j < numb_cols; ++j )
		{
			printf( "%d ", (int)matrix[i][j] );
		}
		printf( "\n" );
	}
}

/// ‘ункци€ FindAverageValues() находит средние значени€ в матрице <i>matrix</i>
/// по строкам, либо по столбцам в зависимости от значени€ параметра <i>proc_type</i>;
/// proc_type - признак, в зависимости от которого средние значени€ вычисл€ютс€ 
/// либо по строкам, либо по стобцам исходной матрицы <i>matrix</i>
/// matrix - исходна€ матрица
/// numb_rows - количество строк в исходной матрице <i>matrix</i>
/// numb_cols - количество столбцов в исходной матрице <i>matrix</i>
/// average_vals - массив, куда сохран€ютс€ вычисленные средние значени€
void FindAverageValues( eprocess_type proc_type, double** matrix, const size_t numb_rows, const size_t numb_cols, double* average_vals )
{
	auto ft1 = std::chrono::high_resolution_clock::now();
	switch ( proc_type )
	{
		case eprocess_type::by_rows:
		{
			for ( size_t i = 0; i < numb_rows; ++i )
			{
				double sum( 0.0 );
				for( size_t j = 0; j < numb_cols; ++j )
				{
					sum += matrix[i][j];
				}
				average_vals[i] = sum / numb_cols;
			}
			break;
		}
		case eprocess_type::by_cols:
		{
			for ( size_t j = 0; j < numb_cols; ++j )
			{
				double sum( 0.0 );
				for( size_t i = 0; i < numb_rows; ++i )
				{
					sum += matrix[i][j];
				}
				average_vals[j] = sum / numb_rows;
			}
			break;
		}
		default:
		{
			throw("Incorrect value for parameter 'proc_type' in function FindAverageValues() call!");
		}
	}
	auto ft2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = (ft2 - ft1);
	printf("\nDuration of function is %f seconds\n", duration.count());
}

void Optimized2FindAverageValues(eprocess_type proc_type, double** matrix, const size_t numb_rows, const size_t numb_cols, double* average_vals)
{
	auto ft1 = std::chrono::high_resolution_clock::now();
	switch (proc_type)
	{
	case eprocess_type::by_rows:
	{
		cilk_for (size_t i = 0; i < numb_rows; ++i)
		{
			cilk::reducer_opadd <double> sum(0.0);
			cilk_for (size_t j = 0; j < numb_cols; ++j)
			{
				sum += matrix[i][j];
			}
			average_vals[i] = sum.get_value() / numb_cols;
		}
		break;
	}
	case eprocess_type::by_cols:
	{
		cilk_for (size_t j = 0; j < numb_cols; ++j)
		{
			cilk::reducer_opadd <double> sum(0.0);
			cilk_for (size_t i = 0; i < numb_rows; ++i)
			{
				sum += matrix[i][j];
			}
			average_vals[j] = sum.get_value() / numb_rows;
		}
		break;
	}
	default:
	{
		throw("Incorrect value for parameter 'proc_type' in function FindAverageValues() call!");
	}
	}
	auto ft2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = (ft2 - ft1);
	printf("\nDuration of function is %f seconds\n", duration.count());
}

void Optimized1FindAverageValues(eprocess_type proc_type, double** matrix, const size_t numb_rows, const size_t numb_cols, double* average_vals)
{
	auto ft1 = std::chrono::high_resolution_clock::now();
	switch (proc_type)
	{
	case eprocess_type::by_rows:
	{
		cilk_for(size_t i = 0; i < numb_rows; ++i)
		{
			double sum(0.0);
			for(size_t j = 0; j < numb_cols; ++j)
			{
				sum += matrix[i][j];
			}
			average_vals[i] = sum / numb_cols;
		}
		break;
	}
	case eprocess_type::by_cols:
	{
		cilk_for(size_t j = 0; j < numb_cols; ++j)
		{
			double sum(0.0);
			for(size_t i = 0; i < numb_rows; ++i)
			{
				sum += matrix[i][j];
			}
			average_vals[j] = sum / numb_rows;
		}
		break;
	}
	default:
	{
		throw("Incorrect value for parameter 'proc_type' in function FindAverageValues() call!");
	}
	}
	auto ft2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = (ft2 - ft1);
	printf("\nDuration of function is %f seconds\n", duration.count());
}

/// ‘ункци€ PrintAverageVals() печатает элементы массива <i>average_vals</i> на консоль;
/// proc_type - признак, отвечающий за то, как были вычислены 
/// средние значени€ исходной матрицы по строкам или по столбцам
/// average_vals - массив, хран€щий средние значени€ исходной матрицы,
/// вычисленные по строкам или по столбцам
/// dimension - количество элементов в исходной массиве <i>average_vals</i>
void PrintAverageVals( eprocess_type proc_type, double* average_vals, const size_t dimension )
{
	switch ( proc_type )
	{
		case eprocess_type::by_rows:
		{
			printf( "\nAverage values in rows:\n" );
			for ( size_t i = 0; i < dimension; ++i )
			{
				printf( "Row %u: %lf\n", i, average_vals[i] );
			}
			break;
		}
		case eprocess_type::by_cols:
		{
			printf( "\nAverage values in columns:\n" );
			for ( size_t i = 0; i < dimension; ++i )
			{
				printf( "Column %u: %lf\n", i, average_vals[i] );
			}
			break;
		}
		default:
		{
			throw("Incorrect value for parameter 'proc_type' in function PrintAverageVals() call!");
		}
	}
}


int main()
{
	const unsigned ERROR_STATUS = -1;
	const unsigned OK_STATUS = 0;

	unsigned status = OK_STATUS;

	try
	{
		srand( (unsigned) time( 0 ) );

		const size_t numb_rows = 10000;
		const size_t numb_cols = 10000;

		double** matrix = new double*[numb_rows];
		for ( size_t i = 0; i < numb_rows; ++i )
		{
			matrix[i] = new double[numb_cols];
		}

		double* average_vals_in_rows = new double[numb_rows];
		double* average_vals_in_cols = new double[numb_cols];

		InitMatrix( matrix, numb_rows, numb_cols );

		//PrintMatrix( matrix, numb_rows, numb_cols );
		auto t1 = std::chrono::high_resolution_clock::now();
		std::thread first_thr( FindAverageValues, eprocess_type::by_rows, matrix, numb_rows, numb_cols, average_vals_in_rows );
		std::thread second_thr( FindAverageValues, eprocess_type::by_cols, matrix, numb_rows, numb_cols, average_vals_in_cols );

		first_thr.join();
		second_thr.join();
		
		auto t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = (t2 - t1);
		printf("\nDuration of threads is %f seconds\n", duration.count());

		t1 = std::chrono::high_resolution_clock::now();
		Optimized1FindAverageValues(eprocess_type::by_rows, matrix, numb_rows, numb_cols, average_vals_in_rows);
		Optimized1FindAverageValues(eprocess_type::by_cols, matrix, numb_rows, numb_cols, average_vals_in_cols);
		t2 = std::chrono::high_resolution_clock::now();
		duration = (t2 - t1);
		printf("\nDuration of function with 1 cilk_for is %f seconds\n", duration.count());

		t1 = std::chrono::high_resolution_clock::now();
		Optimized2FindAverageValues(eprocess_type::by_rows, matrix, numb_rows, numb_cols, average_vals_in_rows);
		Optimized2FindAverageValues(eprocess_type::by_cols, matrix, numb_rows, numb_cols, average_vals_in_cols);
		t2 = std::chrono::high_resolution_clock::now();
		duration = (t2 - t1);
		printf("\nDuration of function with 2 cilk_for is %f seconds\n", duration.count());

		//PrintAverageVals( eprocess_type::by_rows, average_vals_in_rows, numb_rows );
		//PrintAverageVals( eprocess_type::by_cols, average_vals_in_cols, numb_cols );
	}
	catch ( std::exception& except )
	{
		printf( "Error occured!\n" );
		except.what();
		status = ERROR_STATUS;
	}

	return status;
}