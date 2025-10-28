#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <stdexcept>

// Define the type for a single index pair (row, column) using size_t
using IndexPair = std::pair<size_t, size_t>;
// Define the type for the pre-processed input: a vector of vectors of IndexPairs.
using IndexSet = std::vector<std::vector<IndexPair>>;

// =========================================================================
// 2. ProductIterator: Implements the Odometer Algorithm (The core logic)
// =========================================================================

class ProductIterator {
private:
    const IndexSet* index_options_;
    std::vector<size_t> current_indices_;
    bool is_end_ = false;

    std::vector<std::string> getCurrentCombination() const {
        std::vector<std::string> combo;
        std::stringstream ss;
        
        for (size_t i = 0; i < current_indices_.size(); ++i) {
            size_t option_index = current_indices_[i];
            
            const auto& coords = (*index_options_)[i][option_index];
            
            // Format as "ij" string (row and column are size_t)
            ss.str("");
            ss << coords.first << coords.second;
            combo.push_back(ss.str());
        }
        return combo;
    }

public:
    // Standard iterator boilerplate (required types)
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::vector<std::string>;
    using difference_type = std::ptrdiff_t;

    ProductIterator(const IndexSet* options) : index_options_(options) {
        if (!options->empty()) {
            bool has_empty_row = false;
            for(const auto& row : *options) {
                if(row.empty()) {
                    has_empty_row = true;
                    break;
                }
            }
            if (has_empty_row) {
                 is_end_ = true;
            } else {
                 current_indices_.resize(options->size(), 0);
            }
        } else {
            is_end_ = true;
        }
    }

    ProductIterator() : index_options_(nullptr), is_end_(true) {}

    bool operator==(const ProductIterator& other) const {
        return is_end_ == other.is_end_;
    }
    bool operator!=(const ProductIterator& other) const {
        return!(*this == other);
    }

    value_type operator*() const {
        return getCurrentCombination();
    }

    ProductIterator& operator++() {
        if (is_end_) return *this;

        int D = current_indices_.size();
        int i = D - 1; 

        while (i >= 0) {
            current_indices_[i]++;
            
            size_t limit = (*index_options_)[i].size();

            if (current_indices_[i] < limit) {
                break; 
            }
            
            current_indices_[i] = 0; 
            i--;
        }

        if (i < 0) {
            is_end_ = true;
        }
        return *this;
    }

    ProductIterator operator++(int) {
        ProductIterator temp = *this;
        ++(*this);
        return temp;
    }
};

// =========================================================================
// 3. ProductRange: Enables range-based for loops
// =========================================================================
// maybe I should simplify that from the clanker output or tell it to simplify it

class ProductRange {
private:
    const IndexSet* index_options_;
public:
    ProductRange(const IndexSet& options) : index_options_(&options) {}

    ProductIterator begin() const {
        return ProductIterator(index_options_);
    }

    ProductIterator end() const {
        return ProductIterator();
    }
};


/**
 * @brief Reads a matrix from a file, expecting rows separated by newlines
 *        and elements separated by spaces or other whitespace.
 *        maybe teach it to recognize csv of json too
 */
std::vector<std::vector<int>> read_matrix_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file " + filename);
    }

    std::vector<std::vector<int>> matrix;
    std::string line;
    size_t expected_cols = 0; // To ensure all rows have the same width
    size_t row_count = 0;

    while (std::getline(file, line)) {
        std::vector<int> row;
        std::stringstream ss(line);
        int value;

        // Read all integers from the line
        while (ss >> value) {
            row.push_back(value);
        }

        // Skip empty lines
        if (row.empty()) {
            continue;
        }
        
        // Consistency check for matrix dimensions
        if (row_count == 0) {
            expected_cols = row.size();
        } else if (row.size()!= expected_cols) {
            throw std::runtime_error("Error: Matrix is ragged. Row " + std::to_string(row_count) + 
                                     " has " + std::to_string(row.size()) + 
                                     " elements, expected " + std::to_string(expected_cols));
        }

        matrix.push_back(row);
        row_count++;
    }
    
    if (matrix.empty()) {
        throw std::runtime_error("Error: File contains no valid matrix data.");
    }
    
    return matrix;
}

// =========================================================================
// 1. Preprocessing and Main Function
// =========================================================================

/**
 * @brief Pre-processes the matrix to find all valid (row, col) indices of the '1's.
 */
IndexSet preprocess_matrix(const std::vector<std::vector<int>>& matrix) {
    IndexSet valid_indices;
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        std::vector<IndexPair> row_options;
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            if (matrix[i][j] == 1) {
                row_options.push_back({i, j}); 
            }
        }
        valid_indices.push_back(row_options);
    }
    return valid_indices;
}


int main(int argc, char* argv[]) {
    if (argc!= 2) {
        std::cerr << "Usage: " << argv << " <matrix_file_path>\n";
        return 1;
    }
    
    std::vector<std::vector<int>> input_matrix;
    try {
      std::string filename_arg = argv[1];
      input_matrix = read_matrix_from_file(filename_arg);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    // --- Preprocessing ---
    IndexSet valid_options = preprocess_matrix(input_matrix);

    // --- Iteration using the custom ProductRange ---
    std::cout << "--- Cartesian Product of Indices ---\n";
    
    ProductRange product_range(valid_options);
    
    int count = 0;
    
    // The loop lazily generates combinations using the Odometer Algorithm
    for (const auto& combination : product_range) {
        std::cout << "Combination " << ++count << ": [";
        for (size_t i = 0; i < combination.size(); ++i) {
            std::cout << combination[i] << (i < combination.size() - 1? ", " : "");
        }
        std::cout << "]\n";
    }

    std::cout << "\nTotal combinations generated: " << count << std::endl;

    return 0;
}
