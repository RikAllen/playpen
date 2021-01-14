#include <iostream>
#include <functional>
#include <assert.h>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <stack>
#include <random>

#include <chrono> 
using namespace std::chrono;

template <typename T>
void check_sort(std::vector<T>& vec) {
	for (auto next = vec.begin() ; next != vec.end() -1; next++)
	{
		if (*next > *(next + 1)) {
			printf("Error %d > %d\n", *next, *(next + 1));
			exit(-1);
		}
	}
}

template <typename T>
void printvec(std::vector<T>& vec) {
	std::srand(0);
	for (auto& x : vec)
	{
		std::cout << x << ", ";
	}
	std::cout << std::endl;
	
}

template <typename T>
void randomise(std::vector<T>& vec) {
	std::srand(0);

	for (auto& x : vec)
	{
		x = std::rand() + (std::rand() << 16);
	}
}

template <typename T>
int bubble(std::vector<T>& vec) {
	int ops = 0;
	for (auto next = vec.end() -1 ; next != vec.begin(); next--)
	{
		for (auto comp = vec.begin(); comp != next; comp++)
		{
			ops++;

			if (*comp > *(comp + 1)) {
				std::swap(*comp, *(comp + 1));
			}
		}
	}
	std::cout << std::endl;

	return ops;
}

template <typename T>
int insert(std::vector<T>& vec) {
	int ops = 0;
	for (auto next = vec.begin() + 1; next < vec.end(); next++)
	{
		for (auto comp = next; comp != vec.begin(); comp--)
		{
			ops++;
			if (*comp < *(comp - 1)) {
				std::swap(*comp, *(comp - 1));
			}
			else {
				break;
			}
		}
	}
	return ops;
}
static const int MIN_RANGE_LEN = 4;

// An already sorted subset of the array
template <typename T>
struct Range {

	
	using Iter = typename std::vector<T>::iterator;
	Iter start;
	Iter end;
	T max;

	Range() = delete;
	Range(Iter start) : start(start), end(start + 1), max(*(end-1)) {}
	Range(Iter start, Iter end) : start(start), end(end), max(*(end - 1)) { assert(end - start > 1); }
	Range(Iter& input_start, Iter input_end, int min_len ) 
		: Range(input_start)
	{
		assert(end - start >= 1 && "Can't create a zero sized range");
		input_start++;

		// Build up to n vals
		while (input_start != input_end && size() < MIN_RANGE_LEN) {
			input_start = append_insert(input_start);
		}
		// Slurp any more sorted.
		input_start = slurp(input_end);

	}

	Iter append_insert(Iter& val) {
		assert(val == end && "Can only insert if new item is next to existing range");
		end++;
		while (val != start && *val < *(val - 1)) {
			std::swap(*val, *(val - 1));
			val--;
		}
		max = *(end - 1);
		return end;
	}

	// Such from source while it is sorted.
	Iter slurp(Iter src_end) {
		
		while (end != src_end && *end > max) {
			max = *end;
			end++;
		}
		return end;
	}


	Range<T> operator +=( Range<T>&  x) {
		assert(x.start == end && "Assume merged ranges are contiguous");
		std::vector<T> tmp_a(start, end);

		auto pa = tmp_a.begin();

		std::vector<T> tmp_b(x.start, x.end);
		auto pb = tmp_b.begin();

		auto dest = start;
		while (dest != x.end) {
			if (pb == tmp_b.end() || (pa != tmp_a.end() &&  *pa < *pb)) {
				*dest++ = *pa++;
			}
			else {
				*dest++ = *pb++;
			}
		}
		end = x.end;
		max = *(end - 1);
		return *this;
	}

	int size() { return end - start; }
	friend std::ostream & operator<<(std::ostream &os, const Range& p);
};

std::ostream & operator<<(std::ostream &os, const Range<int>& r)
{
	os << "( ";
	for (auto val = r.start; val != r.end; val++) {
		os << *val << ", ";
	}
	os << " )";
	return os;
}

template <typename T>
void timsort(std::vector<T>& vec, int min_range) {
	using RangePtr = std::unique_ptr<Range<T>>;

	std::list<RangePtr>  ranges;

	auto tmp = vec.begin();

	ranges.push_back(std::make_unique<Range<T>>(tmp, vec.end(), MIN_RANGE_LEN));
	while (tmp != vec.end()) {
		ranges.push_front(std::make_unique<Range<T>>(tmp, vec.end(), MIN_RANGE_LEN));
		//std::cout << *ranges.front() << std::endl;

		bool try_merge = true;
		while (try_merge && ranges.size() >= 3) {
			// Consider consolidation
			auto tmp = ranges.begin();
			auto X = tmp++;
			auto Y = tmp++;

			auto Z = tmp++;
			//auto Y = X + 1;
			//auto Z = Y + 1;
			auto sx = (*X)->size();
			auto sy = (*Y)->size();
			auto sz = (*Z)->size();
			//std::cout << "X : " << sx << " Y : " << sy << " Z : " << sz << std::endl;

			if ((sz <= sy + sx) || sx > sy) {
				if (sx < sz) {
					//std::cout << "Merge XY\n";
					// Merge x and y
					**Y += **X;
					ranges.erase(X);

				}
				else {
					// Merge z and y
					//std::cout << "Merge ZY\n";
					**Z += **Y;
					ranges.erase(Y);
				}
			}
			else {
				try_merge = false;
			}

		}
		if (ranges.size() >= 2)
		{
			// Consider consolidation
			auto tmp = ranges.begin();
			auto X = tmp++;
			auto Y = tmp++;
			auto sx = (*X)->size();
			auto sy = (*Y)->size();
			if (sx > sy)
			{
				//std::cout << "Merge Pair XY\n";
				// Merge x and y
				**Y += **X;
				ranges.erase(X);
			}
		}

		//*a += *b;
		//std::cout << *a << std::endl;
	}
	while (ranges.size() > 1)
	{
		auto tmp = ranges.begin();
		auto X = tmp++;
		auto Y = tmp++;
		**Y += **X;
		ranges.erase(X);
	}
}

template <typename T>
void radix_sort(std::vector<T>& vec) {

	std::vector<T> tmp(vec.size());
	const int RADIX = 8;

	for (int shift = 0; shift < sizeof(int) * 8; shift+= RADIX) {
		int counts[1<<RADIX] = { 0 };

		for (auto& val : vec) {
			int byte = (val >> shift) & ((1 << RADIX) - 1);
			counts[byte] ++;
		}
		int next_idx = 0;
		for (int count = 0; count < 1 << RADIX; count++) {
			int tmp = counts[count];
			counts[count] = next_idx;
			next_idx += tmp;
		}
		for (auto& val : vec) {
			int byte = (val >> shift) & ((1 << RADIX) - 1);
			tmp[counts[byte]++] = val;
		}
		std::swap(tmp, vec);
	}

}

void test_sort()
{

	
	std::vector<unsigned int> vec(10000000);
	//{
	//	randomise(vec);
	//	auto start = high_resolution_clock::now();
	//	radix_sort(vec);

	//	auto stop = high_resolution_clock::now();
	//	auto duration = duration_cast<microseconds>(stop - start);
	//	check_sort(vec);

	//	std::cout << "Radix takes " << duration.count() << std::endl;
	//}
	//{
	//	randomise(vec);
	//	auto start = high_resolution_clock::now();
	//	insert(vec);

	//	auto stop = high_resolution_clock::now();
	//	auto duration = duration_cast<microseconds>(stop - start);
	//	check_sort(vec);

	//	std::cout << "Insert takes " << duration.count() << std::endl;
	//}
	//{
	//	randomise(vec); 
	//	auto start = high_resolution_clock::now();
	//	bubble(vec);

	//	auto stop = high_resolution_clock::now();
	//	auto duration = duration_cast<microseconds>(stop - start);
	//	check_sort(vec);

	//	std::cout << "Bubble takes " << duration.count() << std::endl;
	//}
	for (int bs = 4; bs < 16; bs++)
	{
		randomise(vec);
		auto start = high_resolution_clock::now();
		timsort(vec,bs);

		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		check_sort(vec);

		std::cout << "Tim " << bs << " takes " << duration.count() << std::endl;
	}
}