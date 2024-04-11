#ifndef MAXHEAP_HPP
#define MAXHEAP_HPP

#include "scratch.hpp"

template <typename ELEM, Nat N>
class MaxHeap {
    bool (*const _greater_than)(const ELEM &lhs, const ELEM &rhs);
    ELEM _default_value;
    Array<ELEM> _elems;
    int _size;
public:
    MaxHeap()
        : _greater_than{ [](const ELEM &lhs, const ELEM &rhs){ return lhs > rhs; } }, _default_value{ 0 }, _elems{ Array<ELEM>(N, _default_value) }, _size{ 0 }
    {
    }
    MaxHeap(bool (*const greater_than)(const ELEM &lhs, const ELEM &rhs), const ELEM &default_value)
        : _greater_than{ greater_than }, _default_value{ default_value }, _elems{ Array<ELEM>(N, _default_value) }, _size{ 0 }
    {
    }
    ~MaxHeap()
    {
    }
    MaxHeap(const MaxHeap &other) = delete;
    MaxHeap(MaxHeap &&other) = delete;
    Nat size(void) const
    {
        return int2size_t(_size);
    }
    bool isEmpty(void) const
    {
        return size() == 0;
    }
    bool isFull(void) const
    {
        return size() == N;
    }
    void heapify(Nat i)
    {
        for (Nat p = i; ; i = p) {
            const Nat l = 2 * i + 1;
            const Nat r = 2 * i + 2;
            if (l < N && _greater_than(_elems[l], _elems[p]))
                p = l;
            if (r < N && _greater_than(_elems[r], _elems[p]))
                p = r;
            if (p == i) 
                break;
            std::swap(_elems[i], _elems[p]);
        }
    }
    bool push(const ELEM &new_elem)
    {
        if (isFull())
            return false;
        else {
            _size++;
            _elems[_size] = new_elem;
            for (int i = (int)(N / 2) - 1; i >= 0; i--)
                heapify(i);
            return true;
        }
    }
    const ELEM &top(void) const
    {
        if (isEmpty())
            throw std::out_of_range{ "the heap is empty\n" };
        else
            return _elems[0];
    }
    ELEM pop(void)
    {
        if (isEmpty())
            throw std::out_of_range{ "the heap is empty\n" };
        else {
            const ELEM res = _elems[0];
            _elems[0] = _elems[_size];
            _elems[_size] = _default_value;
            _size--;
            for (int i = (int)(N / 2) - 1; i >= 0; i--)
                heapify(i);
            return res;
        }
    }
};

#endif
