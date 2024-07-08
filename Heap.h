#pragma once
#include <iostream>
#include <memory>

namespace algds {

    template<class K, class I>
    class Heap {
    private:
        struct Node {
            K key = K();
            I item = I();
        };
        size_t size = 2;
        size_t length = 0;
        std::unique_ptr<Node[]> arr;
    private:
        void swap(size_t a, size_t b);
        size_t parent(size_t i);
        size_t left(size_t i);
        size_t right(size_t i);
        void fixDown(size_t index);
        void fixUp(size_t index);
        void resize();
    public:
        Heap();
        void push(const K& new_key, const I& new_item);
        I pop();
    };

    template<class K, class I>
    Heap<K, I>::Heap() {
        arr = std::make_unique<Node[]>(size);
    }

    template<class K, class I>
    void Heap<K, I>::push(const K& new_key, const I& new_item)
    {
        resize();
        arr[length] = Node{ new_key,new_item };
        fixUp(length++);
    }

    template<class K, class I>
    I Heap<K, I>::pop()
    {
        I result = arr[0].item;
        arr[0] = arr[length - 1];
        --length;
        fixDown(0);
        resize();
        return result;
    }


    template<class K, class I>
    void Heap<K, I>::swap(size_t a, size_t b)
    {
        Node temp = arr[a];
        arr[a] = arr[b];
        arr[b] = temp;
    }

    template<class K, class I>
    size_t Heap<K, I>::parent(size_t i) {
        return (i - 1) >> 1;
    }

    template<class K, class I>
    size_t Heap<K, I>::left(size_t i) {
        return (i << 1) + 1;
    }

    template<class K, class I>
    size_t Heap<K, I>::right(size_t i) {
        return (i << 1) + 2;
    }

    template<class K, class I>
    void Heap<K, I>::fixDown(size_t index)
    {
        size_t i = index;
        while (i < length)
        {
            size_t l = left(i);
            size_t r = right(i);
            size_t temp = i;
            if (l < length && arr[l].key > arr[i].key)
                temp = l;
            if (r < length && arr[r].key > arr[temp].key)
                temp = r;
            if (temp != i)
                swap(i, temp);
            else
                break;
            i = temp;
        }
    }

    template<class K, class I>
    void Heap<K, I>::fixUp(size_t index)
    {
        long long i = index;
        long long p;
        while (i && arr[i].key > arr[(p = parent(i))].key) {
            swap(i, p);
            i = p;
        }
    }

    template<class K, class I>
    void Heap<K, I>::resize()
    {
        float factor = static_cast<float>(length) / static_cast<float>(size);
        if (length && factor >= 0.75f)
            this->size = this->size << 1;
        else if (length && factor <= 0.25)
            this->size = this->size >> 1;
        else
            return;
        std::unique_ptr<Node[]> new_array = std::make_unique<Node[]>(this->size);
        for (size_t i = 0; i < this->length; ++i)
            new_array[i] = arr[i];
        arr.reset();
        arr = std::move(new_array);
    }
}