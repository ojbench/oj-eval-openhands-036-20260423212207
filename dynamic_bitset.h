#ifndef DYNAMIC_BITSET_H
#define DYNAMIC_BITSET_H

#include <vector>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <algorithm>

struct dynamic_bitset {
private:
    std::vector<uint64_t> data;
    std::size_t bit_size;

public:
    dynamic_bitset() : bit_size(0) {}
    
    ~dynamic_bitset() = default;
    
    dynamic_bitset(const dynamic_bitset &) = default;
    
    dynamic_bitset &operator = (const dynamic_bitset &) = default;
    
    dynamic_bitset(std::size_t n) : bit_size(n) {
        std::size_t blocks = (n + 63) / 64;
        data.resize(blocks, 0);
    }
    
    dynamic_bitset(const std::string &str) : bit_size(str.size()) {
        std::size_t blocks = (str.size() + 63) / 64;
        data.resize(blocks, 0);
        for (std::size_t i = 0; i < str.size(); ++i) {
            if (str[i] == '1') {
                std::size_t block = i / 64;
                std::size_t pos = i % 64;
                data[block] |= (1ULL << pos);
            }
        }
    }
    
    bool operator [] (std::size_t n) const {
        if (n >= bit_size) return false;
        std::size_t block = n / 64;
        std::size_t pos = n % 64;
        return (data[block] >> pos) & 1ULL;
    }
    
    dynamic_bitset &set(std::size_t n, bool val = true) {
        if (n >= bit_size) return *this;
        std::size_t block = n / 64;
        std::size_t pos = n % 64;
        if (val) {
            data[block] |= (1ULL << pos);
        } else {
            data[block] &= ~(1ULL << pos);
        }
        return *this;
    }
    
    dynamic_bitset &push_back(bool val) {
        std::size_t new_bit = bit_size;
        bit_size++;
        std::size_t block = new_bit / 64;
        std::size_t pos = new_bit % 64;
        
        if (block >= data.size()) {
            data.push_back(0);
        }
        
        if (val) {
            data[block] |= (1ULL << pos);
        } else {
            data[block] &= ~(1ULL << pos);
        }
        return *this;
    }
    
    bool none() const {
        for (std::size_t i = 0; i < data.size(); ++i) {
            if (i < data.size() - 1) {
                if (data[i] != 0) return false;
            } else {
                std::size_t valid_bits = bit_size % 64;
                if (valid_bits == 0) valid_bits = 64;
                uint64_t mask = (valid_bits == 64) ? ~0ULL : ((1ULL << valid_bits) - 1);
                if ((data[i] & mask) != 0) return false;
            }
        }
        return true;
    }
    
    bool all() const {
        if (bit_size == 0) return true;
        for (std::size_t i = 0; i < data.size(); ++i) {
            if (i < data.size() - 1) {
                if (data[i] != ~0ULL) return false;
            } else {
                std::size_t valid_bits = bit_size % 64;
                if (valid_bits == 0) valid_bits = 64;
                uint64_t mask = (valid_bits == 64) ? ~0ULL : ((1ULL << valid_bits) - 1);
                if ((data[i] & mask) != mask) return false;
            }
        }
        return true;
    }
    
    std::size_t size() const {
        return bit_size;
    }
    
    dynamic_bitset &operator |= (const dynamic_bitset &other) {
        std::size_t min_size = std::min(bit_size, other.bit_size);
        std::size_t min_blocks = (min_size + 63) / 64;
        for (std::size_t i = 0; i < min_blocks; ++i) {
            data[i] |= other.data[i];
        }
        return *this;
    }
    
    dynamic_bitset &operator &= (const dynamic_bitset &other) {
        std::size_t min_size = std::min(bit_size, other.bit_size);
        std::size_t min_blocks = (min_size + 63) / 64;
        for (std::size_t i = 0; i < min_blocks; ++i) {
            data[i] &= other.data[i];
        }
        return *this;
    }
    
    dynamic_bitset &operator ^= (const dynamic_bitset &other) {
        std::size_t min_size = std::min(bit_size, other.bit_size);
        std::size_t min_blocks = (min_size + 63) / 64;
        for (std::size_t i = 0; i < min_blocks; ++i) {
            data[i] ^= other.data[i];
        }
        return *this;
    }
    
    dynamic_bitset &operator <<= (std::size_t n) {
        if (n == 0) return *this;
        
        std::size_t new_size = bit_size + n;
        std::size_t new_blocks = (new_size + 63) / 64;
        
        std::vector<uint64_t> new_data(new_blocks, 0);
        
        std::size_t block_shift = n / 64;
        std::size_t bit_shift = n % 64;
        
        if (bit_shift == 0) {
            for (std::size_t i = 0; i < data.size(); ++i) {
                new_data[i + block_shift] = data[i];
            }
        } else {
            for (std::size_t i = 0; i < data.size(); ++i) {
                new_data[i + block_shift] |= (data[i] << bit_shift);
                if (i + block_shift + 1 < new_blocks) {
                    new_data[i + block_shift + 1] |= (data[i] >> (64 - bit_shift));
                }
            }
        }
        
        data = std::move(new_data);
        bit_size = new_size;
        return *this;
    }
    
    dynamic_bitset &operator >>= (std::size_t n) {
        if (n >= bit_size) {
            data.clear();
            bit_size = 0;
            return *this;
        }
        
        std::size_t new_size = bit_size - n;
        std::size_t new_blocks = (new_size + 63) / 64;
        
        std::vector<uint64_t> new_data(new_blocks, 0);
        
        std::size_t block_shift = n / 64;
        std::size_t bit_shift = n % 64;
        
        if (bit_shift == 0) {
            for (std::size_t i = 0; i < new_blocks; ++i) {
                new_data[i] = data[i + block_shift];
            }
        } else {
            for (std::size_t i = 0; i < new_blocks; ++i) {
                new_data[i] = data[i + block_shift] >> bit_shift;
                if (i + block_shift + 1 < data.size()) {
                    new_data[i] |= (data[i + block_shift + 1] << (64 - bit_shift));
                }
            }
        }
        
        data = std::move(new_data);
        bit_size = new_size;
        return *this;
    }
    
    dynamic_bitset &set() {
        for (auto &block : data) {
            block = ~0ULL;
        }
        return *this;
    }
    
    dynamic_bitset &flip() {
        for (auto &block : data) {
            block = ~block;
        }
        return *this;
    }
    
    dynamic_bitset &reset() {
        for (auto &block : data) {
            block = 0;
        }
        return *this;
    }
};

#endif
