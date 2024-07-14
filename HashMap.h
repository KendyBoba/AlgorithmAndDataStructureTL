#ifndef HMAP
#define HMAP
#include <memory>
#include <string>
#include <iostream>
#include <functional>

namespace algds {

	using uint64 = unsigned long long;
	template <class V>
	class HashMap {
	private:
		enum class State {
			DELETED,
			EMPTY,
			FILL
		};
	public:
		struct node {
			std::string key = std::string();
			V value = V();
			State state = State::EMPTY;
		};
	private:
		const float max_factor = 0.75f;		//-_- hmmmm...
		const float min_factor = 0.25f;
		uint64 cur_size = 2;
		uint64 fill = 0;
		std::unique_ptr<node[]> pmap;
	private:
		uint64 toNum(const std::string& data);
		uint64 hash(uint64 key);
		bool insert(std::unique_ptr<node[]>& map, const std::string& key, const V& value);
		uint64 find_elem(const std::string& key);
		void increase();
		void reduce();
	public:
		HashMap();
		HashMap(HashMap& map);
		HashMap& operator=(HashMap& map);
		HashMap(HashMap&& map) = delete;
		void insert(std::string key, V value);
		V find(const std::string& key);
		void remove(const std::string& key);
		void run(std::function<void(node)> func);
	};

	template <class V>
	HashMap<V>::HashMap() {
		pmap = std::make_unique<node[]>(cur_size);
	}

	template <class V>
	HashMap<V>::HashMap(HashMap& map) {
		pmap = std::make_unique<node[]>(cur_size);
		map.run([this](node elem)->void {
			this->insert(elem.key, elem.value);
			});
	}
	template <class V>
	HashMap<V>& HashMap<V>::operator=(HashMap& map) {
		if(!pmap)
			pmap = std::make_unique<node[]>(cur_size);
		map.run([this](node elem)->void {
			this->insert(elem.key, elem.value);
			});
		return *this;
	}

	template <class V>
	void HashMap<V>::run(std::function<void(node)> func) {
		for (uint64 i = 0; i < cur_size; ++i) {
			if (pmap[i].state == State::DELETED || pmap[i].state == State::EMPTY)
				continue;
			func(pmap[i]);
		}
	}

	template <class V>
	V HashMap<V>::find(const std::string& key) {
		return pmap[find_elem(key)].value;
	}

	template <class V>
	uint64 HashMap<V>::find_elem(const std::string& key) {
		uint64 key_num = toNum(key);
		uint64 h1 = hash(key_num) % cur_size;
		uint64 h2 = hash(key_num) * 2 + 1;
		while (pmap[h1].key != key) {
			if (pmap[h1].state == State::EMPTY)
				throw std::domain_error("key not found");
			h1 = (h1 + h2) % cur_size;
		}
		return h1;
	}

	template <class V>
	void HashMap<V>::remove(const std::string& key) {
		uint64 pos = find_elem(key);
		pmap[pos].key.clear();
		pmap[pos].value = V();
		pmap[pos].state = State::DELETED;
	}

	template <class V>
	uint64 HashMap<V>::toNum(const std::string& data) {
		uint64 res = 0;
		static const short shift = 10;
		uint64 b = static_cast<uint64>(std::pow(2.0, static_cast<double>(sizeof(uint64))) - 1);
		for (size_t i = 0; i < data.size(); ++i) {
			res += (data[i] * (i + shift)) % b;
		}
		return res % b;
	}

	template <class V>
	uint64 HashMap<V>::hash(uint64 key) {
		double a = (0.618 * key);
		double drob = (a - (int)a);
		return cur_size * drob;
	}

	template <class V>
	void HashMap<V>::increase() {
		uint64 old_size = cur_size;
		cur_size = cur_size << 1;
		std::unique_ptr<node[]> new_pmap = std::make_unique<node[]>(cur_size);
		for (uint64 i = 0; i < old_size; ++i) {
			if (pmap[i].state == State::DELETED || pmap[i].state == State::EMPTY)
				continue;
			insert(new_pmap, pmap[i].key, pmap[i].value);
		}
		pmap.swap(new_pmap);
	}

	template <class V>
	void HashMap<V>::reduce() {
		if (fill <= 2)
			return;
		uint64 old_size = cur_size;
		cur_size = cur_size >> 1;
		std::unique_ptr<node[]> new_pmap = std::make_unique<node[]>(cur_size);
		for (uint64 i = 0; i < old_size; ++i) {
			if (pmap[i].state == State::DELETED || pmap[i].state == State::EMPTY)
				continue;
			insert(new_pmap, pmap[i].key, pmap[i].value);
		}
		pmap.swap(new_pmap);
	}

	template <class V>
	bool HashMap<V>::insert(std::unique_ptr<node[]>& map, const std::string& key, const V& value) {
		uint64 key_num = toNum(key);
		uint64 h1 = hash(key_num) % cur_size;
		uint64 h2 = hash(key_num) * 2 + 1;
		while (map[h1].state == State::DELETED || map[h1].state == State::FILL) {
			if (map[h1].key == key)
				return false;
			h1 = (h1 + h2) % cur_size;
		}
		map[h1] = node{ key,value,State::FILL };
		return true;
	}

	template <class V>
	void HashMap<V>::insert(std::string key, V value) {
		double factor = static_cast<double>(fill) / static_cast<double>(cur_size);
		if (factor >= max_factor)
			increase();
		if (factor < min_factor)
			reduce();
		if (insert(pmap, key, value))
			fill += 1;
	}
}
#endif