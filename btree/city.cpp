
#include <iostream>
#include <functional>
#include <assert.h>
#include <vector>
#include <algorithm>

struct Building {
	 int height;
	 int width;
};
using namespace std;

int city_n2_terse(std::vector<Building>& buildings) {
	int volume = 0;
	for (auto target = buildings.begin(); target != buildings.end(); target++) {
		volume += target->width * std::max(0,
			min(max_element(buildings.begin(), target, [](auto& a, auto& b) {return a.height < b.height; })->height,
				max_element(target, buildings.end(), [](auto& a, auto& b) {return a.height < b.height; })->height)
			- target->height);
	}
	return volume;
}

int city_n2(std::vector<Building>& buildings) {
	int volume = 0;
	for (auto target = buildings.begin(); target != buildings.end(); target++) {
		int left_height = 0;
		int right_height = 0;
		std::for_each(buildings.begin(), target, [&left_height](const Building& b) {left_height = std::max(left_height, b.height); });
		std::for_each(target + 1, buildings.end(), [&right_height](const Building& b) {right_height = std::max(right_height, b.height); });

		int water_level = std::min(left_height, right_height);
		volume += std::max(0, water_level - target->height) * target->width;
	}
	return volume;
}

int city_n2_verbose(std::vector<Building>& buildings) {
	int volume = 0;

	for (auto target = buildings.begin(); target != buildings.end(); target++) {
		int left_height = 0;
		int right_height = 0;
		for (auto left = buildings.begin(); left != target; left++) {
			left_height = max(left_height, left->height);
		}
		for (auto right = target + 1; right != buildings.end(); right++) {
			right_height = max(right_height, right->height);
		}
		
		int water_level = std::min(left_height, right_height);
		volume += std::max(0, water_level - target->height) * target->width;
	}
	return volume;
}

int city_n(std::vector<Building>& buildings) {
	int left_height = 0;
	int right_height = 0;
	int depth = std::min(left_height, right_height);

	auto left = buildings.begin();
	auto right = buildings.end() - 1;
	int volume = 0;

	do {
		left_height = std::max(left_height, left->height);
		right_height = std::max(right_height, right->height);
		depth = std::min(left_height, right_height);

		if (left_height < right_height) {
			left++;
			volume += left->width * std::max(0, depth - left->height);
		}
		else {
			right--;
			volume += right->width * std::max(0, depth - right->height);

		}
	} while (left != right);
	return volume;
}

void test_city() {
	std::vector<Building> buildings{
		{20,10},
		{4,10},
		{24,10}, 
		{20,10},
		{40,5},
		{18,12},
		{23,8},
		{19,18},
		{25,6},
		{36,9},
		{18,10}
	};

	
	printf("Version 1 volume = %d\n", city_n2(buildings));
	printf("Version 2 volume = %d\n", city_n(buildings));
	printf("Version 2 volume = %d\n", city_n2_terse(buildings));

}

