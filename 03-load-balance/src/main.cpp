#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

#include "assigner.hpp"

/*
Students: Jared and Leo

Description of changes for the lab: 
 - Kept the core hashing and chaining logic from the starter files completely intact.
 - Implemented the lab deliverables in main() by automating the testing for both the small and large datasets.
 - Built a text-based histogram generator to visualize the layout distribution.
 - Added comparative experiments (testing weak vs. strong hashes, and scaling the number of rooms) to generate the required data for our final reflection.

 Responsibility: 
- Leo: Write the general test program
- Jared: Revise the code and write the reflection

Statement: Each person contributed about equally to the development of the program by participating in parts of the program writing and conceptual suggestions. 
We all followed the guidelines for Pair Programming in the process of collaboration.
*/

// Usage:
//   ./03_load_balance data/small_artworks.txt 3 2
//   ./03_load_balance data/large_artworks.txt 10 6
//
// args:
//   file  m_rooms  bucket_size_B

static bool load_artwork_ids(const std::string& path, std::vector<std::uint32_t>& ids) {
    std::ifstream in(path);
    if (!in) return false;
    std::uint32_t x;
    while (in >> x) ids.push_back(x);
    return true;
}

int main(int argc, char** argv) {
    std::string file = (argc >= 2) ? argv[1] : "data/small_artworks.txt";
    std::uint32_t m_rooms = (argc >= 3) ? (std::uint32_t)std::stoul(argv[2]) : 3;
    int B = (argc >= 4) ? std::atoi(argv[3]) : 2;

    std::vector<std::uint32_t> ids;
    if (!load_artwork_ids(file, ids)) {
        std::cerr << "Failed to read " << file << "\n";
        return 1;
    }

    // A simple universal-hashing style setup (constants can be changed).
    // P should be a large prime > max artwork id.
    HashedAssigner assigner(/*a=*/2654435761ULL, /*b=*/12345ULL, /*prime=*/4294967311ULL);

    auto result = assigner.assign(ids, m_rooms, B);

    std::cout << "n (artworks) = " << ids.size() << "\n";
    std::cout << "m (rooms)    = " << m_rooms << "\n";
    std::cout << "alpha = n/m  = " << (double)ids.size() / (double)m_rooms << "\n";
    std::cout << "bucket size B = " << B << "\n\n";

    for (std::size_t r = 0; r < result.room_to_artworks.size(); r++) {
        std::cout << "Room " << r << " load=" << result.room_to_artworks[r].size() << " : ";
        // Print at most first 12 ids to keep output readable
        int shown = 0;
        for (auto id : result.room_to_artworks[r]) {
            if (shown++ >= 12) { std::cout << "..."; break; }
            std::cout << id << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\nmax load = " << result.max_load << "\n";
    std::cout << "rooms overflowing (load > B) = " << result.overflows << "\n";

    // Histogram deliverable
    std::cout << "\nDistribution Histogram:\n";
    for (std::size_t r = 0; r < result.room_to_artworks.size(); r++) {
        std::cout << "Room " << r << (r < 10 ? "  | " : " | ");
        for (size_t i = 0; i < result.room_to_artworks[r].size(); ++i) {
            std::cout << "*";
        }
        std::cout << " (" << result.room_to_artworks[r].size() << ")\n";
    }

    // Experiments
    std::cout << "\nAdditional Experiments for Report: \n";

    // Compare with a non-random and poor hash (e.g., room = id % m).
    HashedAssigner weak_hash(1ULL, 0ULL, 4294967311ULL); 
    auto weak_res = weak_hash.assign(ids, m_rooms, B);
    std::cout << "[Poor Hash] a=1, b=0 (id % m) --- Max load: " << weak_res.max_load 
              << ", Overflows: " << weak_res.overflows << "\n";

    // Try multiple (a,b) pairs and compare max load or overflow count.
    HashedAssigner alt_hash(31ULL, 17ULL, 4294967311ULL); 
    auto alt_res = alt_hash.assign(ids, m_rooms, B);
    std::cout << "[Alt Hash]  a=31, b=17 --- Max load: " << alt_res.max_load 
              << ", Overflows: " << alt_res.overflows << "\n";

    // Run experiments for different alpha = n/m and bucket size B.
    std::uint32_t more_rooms = m_rooms * 2;
    auto alpha_res = assigner.assign(ids, more_rooms, B);
    std::cout << "[Low Alpha] Doubled Rooms (m=" << more_rooms << ") --- Max load: " << alpha_res.max_load 
              << ", Overflows: " << alpha_res.overflows << "\n";

    return 0;
}
