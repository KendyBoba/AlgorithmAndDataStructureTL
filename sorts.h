#pragma once
#include <stack>
#include <random>

namespace algds {

	template <class T>
	class merge_sort {
	private:
		void merge(T arr[], size_t l, size_t m, size_t r);
	public:
		merge_sort() {};
		void operator()(T arr[], size_t size);
	};

	template <class T>
	void merge_sort<T>::operator()(T arr[], size_t size) {
		std::stack<std::pair<size_t, size_t>> stack;
		std::stack<std::pair<size_t, size_t>> stack_second;
		stack.push(std::make_pair(0, size - 1));
		while (!stack.empty()) {
			auto cur_points = stack.top();
			stack.pop();
			if (cur_points.first >= cur_points.second)
				continue;
			stack_second.push(cur_points);
			size_t middle = (cur_points.first + cur_points.second) / 2;
			stack.push(std::make_pair(middle + 1, cur_points.second));
			stack.push(std::make_pair(cur_points.first, middle));
		}
		while (!stack_second.empty()) {
			auto cur_points = stack_second.top();
			stack_second.pop();
			size_t middle = (cur_points.first + cur_points.second) / 2;
			merge(arr, cur_points.first, middle, cur_points.second);
		}
	}

	template <class T>
	void merge_sort<T>::merge(T arr[], size_t l, size_t m, size_t r) {
		size_t tmp_m = m - l;
		size_t tmp_r = r - l + 1;
		T* temp_arr = new T[r - l + 1];
		size_t i = 0;
		size_t j = 0;
		for (i = 0; i <= tmp_m; ++i)
			temp_arr[i] = arr[i + l];
		for (i = tmp_r - 1, j = tmp_m + 1; i > tmp_m; --i, ++j)
			temp_arr[j] = arr[i + l];
		i = 0;
		j = tmp_r - 1;
		for (size_t k = l; k <= r; ++k) {
			arr[k] = (temp_arr[i] > temp_arr[j]) ? temp_arr[j--] : temp_arr[i++];
		}
	}

	template <class T>
	void insertionSort(T arr[], size_t size) {
		for (size_t j = 1; j < size; ++j) {
			T key = arr[j];
			size_t i = j;
			while (i > 0 && arr[i - 1] > key) {
				arr[i] = arr[i - 1];
				--i;
			}
			arr[i] = key;
		}
	}


	template<class T>
	class Quiksort {
	private:
		std::mt19937 random;
		size_t Partition(T* arr, size_t l, size_t r);
		void swap(T* arr, size_t a, size_t b);
	public:
		Quiksort();
		void operator()(T* arr, size_t size);
	};

	template<class T>
	Quiksort<T>::Quiksort()
	{
		random.seed(time(nullptr));
	}

	template<class T>
	size_t Quiksort<T>::Partition(T* arr, size_t l, size_t r)
	{
		swap(arr, (this->random() % (r - l)) + l, r);
		T pivot = arr[r];
		size_t wall = l;
		for (size_t i = l; i < r; ++i) {
			if (arr[i] <= pivot) {
				swap(arr, wall, i);
				++wall;
			}
		}
		swap(arr, wall, r);
		return wall;
	}

	template<class T>
	void Quiksort<T>::swap(T* arr, size_t a, size_t b)
	{
		T temp = arr[a];
		arr[a] = arr[b];
		arr[b] = temp;
	}

	template<class T>
	void Quiksort<T>::operator()(T arr[], size_t size)
	{
		std::stack<std::pair<size_t, size_t>> stack;
		stack.push(std::make_pair(0, size - 1));
		while (!stack.empty()) {
			auto el = stack.top();
			stack.pop();
			if (el.first >= el.second)
				continue;
			size_t m = Partition(arr, el.first, el.second);
			stack.push(std::make_pair(m + 1, el.second));
			stack.push(std::make_pair(el.first, (!m) ? 0 : m - 1));
		}
	}

	template<class T>
	class HeapSort {
	private:
		void swap(T* arr, size_t a, size_t b);
		size_t parent(size_t i);
		size_t left(size_t i);
		size_t right(size_t i);
		void fixDown(T* arr, size_t size, size_t index);
		void buildHeap(T* arr, size_t size);
	public:
		HeapSort();
		void operator()(T* arr, size_t size);
	};

	template<class T>
	HeapSort<T>::HeapSort() {}

	template<class T>
	void HeapSort<T>::operator()(T* arr, size_t size)
	{
		if (!size || size == 1)
			return;
		buildHeap(arr, size);
		size_t j = 1;
		for (size_t i = size - 1; i >= 1; --i) {
			swap(arr, 0, i);
			fixDown(arr, size - j, 0);
			++j;
		}
	}

	template<class T>
	void HeapSort<T>::swap(T* arr, size_t a, size_t b)
	{
		T temp = arr[a];
		arr[a] = arr[b];
		arr[b] = temp;
	}

	template<class T>
	size_t HeapSort<T>::parent(size_t i) {
		return (i >> 1);
	}

	template<class T>
	size_t HeapSort<T>::left(size_t i) {
		return (i << 1);
	}

	template<class T>
	size_t HeapSort<T>::right(size_t i) {
		return (i << 1) | 1;
	}

	template<class T>
	void HeapSort<T>::fixDown(T* arr, size_t size, size_t index)
	{
		size_t i = index + 1;
		while (i <= size)
		{
			size_t l = left(i);
			size_t r = right(i);
			size_t temp = i;
			if (l <= size && arr[l - 1] > arr[i - 1])
				temp = l;
			if (r <= size && arr[r - 1] > arr[temp - 1])
				temp = r;
			if (temp != i)
				swap(arr, i - 1, temp - 1);
			else
				break;
			i = temp;
		}
	};

	template<class T>
	void HeapSort<T>::buildHeap(T* arr, size_t size)
	{
		for (long long i = size / 2; i >= 0; --i)
			fixDown(arr, size, i);
	}
}