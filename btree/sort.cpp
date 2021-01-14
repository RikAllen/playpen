#include <iostream>
#include <functional>
#include <assert.h>
#include <vector>
#include <unordered_set>
#include <algorithm>

void check_sort(std::vector<int>& vec) {
	for (auto next = vec.begin() ; next != vec.end() -1; next++)
	{
		if (*next > *(next + 1)) {
			printf("Error %d > %d\n", *next, *(next + 1));
			exit(-1);
		}
	}
}
void printvec(std::vector<int>& vec) {
	std::srand(0);
	for (auto& x : vec)
	{
		std::cout << x << ", ";
	}
	std::cout << std::endl;
	
}
void randomise(std::vector<int>& vec) {
	std::srand(0);
	for (auto& x : vec)
	{
		x = (std::rand() % 1000) - 500;
	}
	printvec(vec);
}

int bubble(std::vector<int>& vec) {
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

int insert(std::vector<int>& vec) {
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

// An already sorted subset of the array
template <typename T>
struct Range {
	using Iter = typename std::vector<T>::iterator;
	Iter start;
	Iter end;
	T max;

	Range() = delete;
	Range(Iter start) : start(start), end(start + 1), max(*(end-1)) {}
	Range(Iter start, Iter end) : start(start), end(end), max(*(end - 1)) {}

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
		std::vector<T> tmp_a(size());
		std::copy(start, end, std::back_insert_iterator<std::vector<T>>(tmp_a));
		auto pa = tmp_a.begin();

		std::vector<T> tmp_b(x.size());
		std::copy(x.start, x.end, std::back_insert_iterator<std::vector<T>>(tmp_b));
		auto pb = tmp_b.begin();

		auto dest = start;
		while (dest != x.end) {
			if (*pa < *pb) {
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


static const int MIN_RANGE_LEN = 4;

void test_sort()
{
	std::vector<int> vec(16);
	randomise(vec);
	printf("inserts does %d\n", insert(vec));
	printvec(vec); check_sort(vec);
	randomise(vec);
	printf("Bubble does %d\n", bubble(vec));
	printvec(vec); check_sort(vec);
	
	randomise(vec);

	auto tmp = vec.begin();
	auto a = std::make_unique<Range<int>>(tmp);
	tmp++;

	// Build up to n vals
	while (tmp != vec.end() && a->size() < MIN_RANGE_LEN) {
		tmp = a->append_insert(tmp);
	}
	// Slurp any more sorted.
	tmp = a->slurp(vec.end());

	std::cout << *a << std::endl;


	auto b = std::make_unique<Range<int>>(tmp);
	tmp++;
	// Build up to n vals
	while (tmp != vec.end() && b->size() < MIN_RANGE_LEN) {
		tmp = b->append_insert(tmp);
	}
	// Slurp any more sorted.
	tmp = b->slurp(vec.end());

	std::cout << *b << std::endl;

	*a += *b;
	std::cout << *a << std::endl;

}