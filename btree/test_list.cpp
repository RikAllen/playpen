#include <iostream>
#include <functional>
#include <assert.h>
#include <vector>
#include <unordered_set>
#include <algorithm>


std::vector<int> rand_vec(int len) {
	std::vector<int> vec(len);

	for (auto& x : vec)
	{
		x = (std::rand() % 1000) - 500;
		std::cout << x << ", ";
	}
	std::cout <<  std::endl;

	return vec;
}

int longest_pos(std::vector<int>& vec){
	int longest = 0;
	int length = 0;
	for (auto& x : vec)
	{
		if (x >= 0)
		{
			length++;
			longest = std::max(longest, length);
		}
		else
		{
			length = 0;
		}
	}
	return longest;
}

int highest_sum(std::vector<int>& vec) {
	int sum = 0;
	int max_sum = 0;
	for (auto& x : vec)
	{
			sum = std::max(0, sum + x); 
			max_sum = std::max(max_sum, sum); 
	}
	return max_sum;
}

int highest_mean(std::vector<int>& vec) {
	auto foo = std::max_element(vec.begin(), vec.end(), [](auto& a, auto& b) {return a < b; });
	return *foo;
}


void test_list()
{
	auto vec = rand_vec(16);
	std::cout << "Longest is " << longest_pos(vec) << std::endl;
	std::cout << "Highest sum is " << highest_sum(vec) << std::endl;
	std::cout << "Highest mean is " << highest_mean(vec) << std::endl;

}