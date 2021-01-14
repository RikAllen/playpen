
#include <iostream>
#include <functional>
#include <assert.h>
#include <vector>
#include <unordered_set>
#include <algorithm>

#include <chrono> 
using namespace std::chrono;
template <typename T>
struct Matrix {
	
	Matrix(int n)
		: n(n)
		, data(n, std::vector<T>(n))
	{
		for (int row = 0; row < n; row++) {
			T left = 0;
			for (int col = 0; col < n; col++) {
				T above = row > 0 ? data[row - 1][col] : 0;
				T val = std::max(left, above) + std::rand() % 50000;
				data[row][col] = val;
				left = val;
				max_val = val;
				check_set.insert(val);
			}
		}
//		std::cout << "Set max = " << max_val << " distinct vals = " << check_set.size() << " density = " << ((float)check_set.size())/max_val <<   std::endl;
	}

	void print() {
		for (auto row : data) {
			for (auto cell : row) {
				std::cout << cell << ", ";
			}
			std::cout << std::endl;
		}

	}

	bool On2(const T val) const {
		for (auto& row : data) {
			//if (row[0] > val) break;
			//if (row[n - 1] < val) continue;
			for (auto& cell : row) {
				if (cell == val) return true;
				//if (cell > val) break;
			}
		}
		return false;
	}

	bool On2_slow(const T val) const {
		bool found = false;
		for (auto& row : data) {
			for (auto& cell : row) {
				found |= (cell == val);
			}
		}
		return found;
	}

	bool Onlgn(const T val) const {
		for (auto& row : data) {
			if (binary_search(row.begin(), row.end(), val)) return true;
		}
		return false;
	}

	bool Onlgn2(const T val) const{
		for (auto& row : data) {
			auto head = row.begin();
			auto tail = row.end()-1;
			while (tail != head )
			{
				auto mid = head + ((tail - head) >>1);
				if (*mid == val) {
					return true;
				}
				else if (*mid < val) {
					head = mid+1;
				}
				else
				{
					tail = mid;
				}
			}
		}
		return false;
	}
	bool Onlgn3(const T val) const {
		for (auto& row : data) {
			int count = n;
			auto head = row.data();

			while (count > 0)
			{
				int count2 = count >> 1;
				auto mid = head + count2;
				if (*mid < val) {
					head = mid + 1;
					count -= count2 + 1;
				}
				else  {
					count = count2;
				}
			}
			if (*head == val) return true;
		}
		return false;
	}
	bool On(const T val) const {
		int rown = n-1;
		auto row = data.end() - 1;
		int col = 0;
		do 
		{
			int lookup = (*row)[col];
			if (lookup < val) {
				col++;
				if (col == n) return false;
			} else if (lookup > val) {
				rown--;
				if (rown < 0) return false;
				row--; 
			}
			else {
				return true;
			} 
			 
		} while(true);
		return false;
	}

	T max_val;
	std::vector<std::vector<T>> data;
	std::unordered_set<T> check_set;
	int n;
};

typedef bool ( Matrix<long long int>::*fn)(const long long int val) const;

auto time_it(const Matrix<long long int>& m, fn func) {
	{
		auto start = high_resolution_clock::now();

		int count = 0;
		for (int i = 0; i < 10000; i++)
		{
			count += (m.*func)(std::rand() % (int)(m.max_val));
		}
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		return duration;
	}

}

void test_matrix() {
	const int n = 200;
	Matrix<long long int> m(n);

	int hit_count = 0;

	const int test_loops = 1000;
	long long int max_test_val = 0;
	long long int min_test_val = 0;

	for (int i = 0; i < test_loops; i++)
	{
		long long int val = std::rand() % (m.max_val);
		max_test_val = std::max(max_test_val, val);
		min_test_val = std::max(min_test_val, val);
		bool check = (m.check_set.find(val) != m.check_set.end());
		auto a = m.On2(val);
		auto b = m.Onlgn(val);
		auto c = m.On(val);
		auto d = m.Onlgn2(val);
		auto e = m.Onlgn3(val);
		if ((a ^ check) || (b ^check) || (c ^check) || (d ^check) || (e ^check))
		{
			printf("Bang\n");
			exit(-1);
		}
		hit_count += check;
	}
	std::cout << "Hit rate of testing " << ((double)hit_count) / test_loops << std::endl;
	//time_it(m, &(Matrix::On2));
	//time_it(m, &(Matrix::Onlgn));
	//time_it(m, &(Matrix::Onlgn2));
	//time_it(m, &(Matrix::On));
	
	for (int nn = 10; nn < 50000; nn += 50)
	{
		Matrix<long long int> mm(nn);
		//printf("nn = %d, max = %d ", nn, mm.max_val);
		auto a = time_it(mm, &(Matrix<long long int>::On2));
		//auto c = time_it(mm, &(Matrix<long long int>::Onlgn));
		//auto d = time_it(mm, &(Matrix<long long int>::Onlgn2));
		auto e = time_it(mm, &(Matrix<long long int>::Onlgn3));
		auto f = time_it(mm, &(Matrix<long long int>::On));
		std::cout << nn << ", " << a.count() << ", " << /*c.count() << ", " << d.count() << ", " << */e.count() << ", " << f.count() << std::endl;
	}
}