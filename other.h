#pragma once
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>

namespace algds {

	template <typename T>
	std::shared_ptr<std::vector<T>> subsequence(const std::vector<T>& obj1, const std::vector<T>& obj2) {
		std::unique_ptr<std::pair<size_t, uint8_t>[]> map = std::make_unique<std::pair<size_t, uint8_t>[]>((obj1.size() + 1) * (obj2.size() + 1));
		for (size_t i = 0; i < obj1.size(); ++i) {
			map[i].first = 0;
			map[i].second = 0;
		}
		for (size_t i = 0; i < obj2.size(); ++i) {
			map[0 + i * obj1.size()].first = 0;
			map[0 + i * obj1.size()].second = 0;
		}
		auto result = std::make_shared<std::vector<T>>();
		for (size_t j = 0; j < obj2.size(); ++j) {
			for (size_t i = 0; i < obj1.size(); ++i) {
				if (obj1[i] == obj2[j]) {
					map[i + 1 + (j + 1) * obj1.size()].first = map[i + j * obj1.size()].first + 1;
					map[i + 1 + (j + 1) * obj1.size()].second = 1;
				}
				else {
					map[i + 1 + (j + 1) * obj1.size()].first = std::max(map[i + (j + 1) * obj1.size()].first, map[(i + 1) + j * obj1.size()].first);
					map[i + 1 + (j + 1) * obj1.size()].second = (map[i + (j + 1) * obj1.size()].first > map[(i + 1) + j * obj1.size()].first) ? 2 : 3;
				}
			}
		}
		for (size_t i = obj1.size(), j = obj2.size(); i > 0 && j > 0;) {
			if (map[i + j * obj1.size()].second == 1) {
				--i; --j;
				result->push_back(obj1[i]);
			}
			else if (map[i + j * obj1.size()].second == 2) {
				--i;
			}
			else if (map[i + j * obj1.size()].second == 3) {
				--j;
			}
		}
		std::reverse(result->begin(), result->end());
		std::cout << map[obj1.size() + (obj2.size()) * obj1.size()].first;
		return result;
	}

	template<class T>
	struct ItemBP
	{
		std::shared_ptr<T> item = nullptr;
		unsigned int weight;
		double price;
	};

	template<class T>
	struct ItemsBP {
		double sum_price = 0;
		std::vector<ItemBP<T>> isbp;
	};

	template<class T>
	std::vector<ItemBP<T>> backPack(const std::vector<ItemBP<T>>& vec, unsigned int max_weight) {
		auto mem = std::make_unique<ItemsBP<T>[]>(max_weight * vec.size());
		for (unsigned int i = 0; i < max_weight; ++i) {
			if (vec[0].weight > i) {
				mem[0 * max_weight + i].sum_price = 0.0;
			}
			else {
				mem[0 * max_weight + i].sum_price = vec[0].price;
				mem[0 * max_weight + i].isbp.push_back(vec[0]);
			}
		}
		for (unsigned int i = 1; i < vec.size(); ++i) {
			for (unsigned int j = 0; j < max_weight; ++j) {
				if (vec[i].weight > j) {
					mem[i * max_weight + j].sum_price = 0.0;
					continue;
				}
				double a = mem[(i - 1) * max_weight + j].sum_price;
				double b = vec[i].price + mem[(i - 1) * max_weight + j - vec[i].weight].sum_price;
				if (a > b) {
					mem[i * max_weight + j].sum_price = a;
					std::for_each(mem[(i - 1) * max_weight + j].isbp.begin(), mem[(i - 1) * max_weight + j].isbp.end(), [&](ItemBP<T>& el)->void {
						mem[i * max_weight + j].isbp.push_back(el);
						});
				}
				else {
					mem[i * max_weight + j].sum_price = b;
					std::for_each(mem[(i - 1) * max_weight + j - vec[i].weight].isbp.begin(), mem[(i - 1) * max_weight + j - vec[i].weight].isbp.end(), [&](ItemBP<T>& el)->void {
						mem[i * max_weight + j].isbp.push_back(el);
						});
					mem[i * max_weight + j].isbp.push_back(vec[i]);
				}
			}
		}
		return mem[max_weight * vec.size() - 1].isbp;
	}

}