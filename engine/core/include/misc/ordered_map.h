#pragma once

#include <vector>
#include <unordered_map>
#include <initializer_list>
#include <utility> // for std::pair

namespace engine
{
    template<typename Key, typename Value>
    class ordered_map {
    private:
        std::vector<Key> order;
        std::unordered_map<Key, Value> map;
    public:

        struct KVIterator
        {
            using Iterator = typename std::vector<Key>::const_iterator;
            Iterator it;
            const std::unordered_map<Key, Value>& map;
            mutable std::pair<const Key, Value*> current; // Store current key-value pair

            KVIterator(Iterator it, const std::unordered_map<Key, Value>& map)
                : it(it), map(map) {
                if (it != map.order.end()) {
                    current.first = *it;
                    current.second = &this->map.at(*it);
                }
            }

            bool operator!=(const KVIterator& other) const { return it != other.it; }
            void operator++() {
                ++it;
                if (it != map.order.end()) {
                    current.first = *it;
                    current.second = &map.at(*it);
                }
            }

            // Return a reference to the current pair
            std::pair<const Key&, Value&> operator*() const {
                return { current.first, *current.second };
            }
        };


        ordered_map() = default;

        // Constructor for brace initialization
        ordered_map(std::initializer_list<std::pair<const Key, Value>> init) {
            for (const auto& pair : init) {
                insert(pair.first, pair.second);
            }
        }

        void insert(const Key& key, const Value& value) {
            if (map.find(key) == map.end()) {
                order.push_back(key);
            }
            map[key] = value;
        }

        Value& at(const Key& key) {
            return map.at(key);
        }

        bool contains(const Key& key) const {
            return map.find(key) != map.end();
        }

        template<typename Func>
        void forEach(Func func) const {
            for (const auto& key : order) {
                func(key, map.at(key));
            }
        }

        template<typename Func>
        void forEach(Func func) {
            for (auto& key : order) {
                func(key, map.at(key));
            }
        }

        void erase(const Key& key) {
            map.erase(key);
            order.erase(std::remove(order.begin(), order.end(), key), order.end());
        }

        const std::vector<Key>& keys() const {
            return order;
        }

        size_t size() const {
            return order.size();
        }

        // Begin and end for range-based for loop
        KVIterator begin() const { return KVIterator(order.begin(), map); }
        KVIterator end() const { return KVIterator(order.end(), map); }
    };
}



//#include <iostream>
//
//int main() {
//    ordered_map<std::string, int> myMap;
//
//    // Insert elements
//    myMap.insert("apple", 10);
//    myMap.insert("banana", 20);
//    myMap.insert("orange", 30);
//
//    // Iterate in insertion order
//    myMap.forEach([](const auto& key, const auto& value) {
//        std::cout << key << ": " << value << std::endl;
//    });
//    // Output:
//    // apple: 10
//    // banana: 20
//    // orange: 30
//
//    // Fast lookup
//    std::cout << "banana = " << myMap.at("banana") << std::endl;  // 20
//
//    // Remove an element
//    myMap.erase("banana");
//
//    // Iterate again
//    myMap.forEach([](const auto& key, const auto& value) {
//        std::cout << key << ": " << value << std::endl;
//    });
//    // Output:
//    // apple: 10
//    // orange: 30
//
//    return 0;
//}