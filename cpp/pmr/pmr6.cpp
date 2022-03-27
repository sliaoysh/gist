#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <memory_resource>
#include <deque>
using namespace std;

class noisy_allocator : public pmr::memory_resource {
    void* do_allocate(size_t bytes, size_t alignment) override {
        cout << "+ Allocating " << bytes << " bytes @ ";
        void* p = pmr::new_delete_resource()->allocate(bytes, alignment);
        cout << p << '\n';
        return p;
    }
 
    void do_deallocate(void* p, size_t bytes, size_t alignment) override {
        cout << "- Deallocating " << bytes << " bytes @ " << p << '\n';
        return pmr::new_delete_resource()->deallocate(p, bytes, alignment);
    }
 
    bool do_is_equal(const pmr::memory_resource& other) const noexcept override {
        return pmr::new_delete_resource()->is_equal(other);
    }
};
 
int main() {
    noisy_allocator mem;
    pmr::set_default_resource(&mem);

    {
        pmr::deque<long long> v1{{1}};
        cout << "==========" << endl;

        for (int i{0}; i != 1000; ++i)
        {
            v1.push_back(i);
            //cout << "v1.size(): " << v1.size() << '\n';
        }
        cout << "==========" << endl;
    }
    cout << "---------------------" << endl;

    {
        pmr::unsynchronized_pool_resource pool{{64,0}};
        pmr::deque<long long> v2{{1}, &pool};
        pmr::deque<long long> v3{{1}, &pool};
        pmr::deque<long long> v4{{1}, &pool};
        cout << "==========" << endl;

        for (int i{0}; i != 1000; ++i)
        {
            v2.push_back(i);
            v3.push_back(i);
            v4.push_back(i);
            //cout << "v2.size(): " << v2.size() << '\n';
            //cout << "v3.size(): " << v3.size() << '\n';
            //cout << "v4.size(): " << v4.size() << '\n';
        }
        cout << "==========" << endl;
    }
}
