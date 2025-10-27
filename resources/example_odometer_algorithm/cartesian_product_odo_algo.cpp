#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <algorithm>
#include <sstream>

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

    /**
     * @brief Looks up the coordinates and formats them into the requested double-digit string list.
     */
    std::vector<std::string> getCurrentCombination() const {
        std::vector<std::string> combo;
        
        // Use stringstream to build the "ij" string quickly
        std::stringstream ss;
        
        for (size_t i = 0; i < current_indices_.size(); ++i) {
            size_t option_index = current_indices_[i];
            
            // Access the pair of (row, col) coordinates
            const auto& coords = (*index_options_)[i][option_index];
            
            // Format as "ij" string
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

    // Constructor for the 'begin()' iterator
    ProductIterator(const IndexSet* options) : index_options_(options) {
        // Initialize the odometer, but only if all rows have at least one option.
        if (!options->empty()) {
            bool has_empty_row = false;
            for(const auto& row : *options) {
                if(row.empty()) {
                    has_empty_row = true;
                    break;
                }
            }
            if (has_empty_row) {
                 is_end_ = true; // Product is zero if any component is zero
            } else {
                 current_indices_.resize(options->size(), 0); // Start at [0, 0, 0,...]
            }
        } else {
            is_end_ = true;
        }
    }

    // Constructor for the 'end()' sentinel iterator
    ProductIterator() : index_options_(nullptr), is_end_(true) {}

    // Equality check
    bool operator==(const ProductIterator& other) const {
        return is_end_ == other.is_end_;
    }
    bool operator!=(const ProductIterator& other) const {
        return!(*this == other);
    }

    // Dereference operator
    value_type operator*() const {
        return getCurrentCombination();
    }

    /**
     * @brief Increment operator (++): Implements the Odometer (carry) logic.
     */
    ProductIterator& operator++() {
        if (is_end_) return *this;

        // Start at the least significant digit (rightmost row index)
        int D = current_indices_.size();
        int i = D - 1; 

        while (i >= 0) {
            current_indices_[i]++;
            
            size_t limit = (*index_options_)[i].size();

            if (current_indices_[i] < limit) {
                break; // Carry absorbed: stop propagating.
            }
            
            // Carry required: reset and move left.
            current_indices_[i] = 0; 
            i--;
        }

        if (i < 0) {
            // Carry propagated past the first dimension (Row 0), signaling termination.
            is_end_ = true;
        }
        return *this;
    }

    // Postfix increment
    ProductIterator operator++(int) {
        ProductIterator temp = *this;
        ++(*this);
        return temp;
    }
};

// =========================================================================
// 3. ProductRange: Enables range-based for loops
// =========================================================================

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


int main() {
    std::vector<std::vector<int>> input_matrix = {
        {1, 0, 0, 1, 1}, // (0,0), (0,3), (0,4)
        {0, 0, 0, 1, 0}, // (1,3)
        {1, 1, 0, 0, 1}, // (2,0), (2,1), (2,4)
        {0, 0, 1, 0, 0}, // (3,2)
        {0, 1, 0, 1, 0}  // (4,1), (4,3)
    };

    IndexSet valid_options = preprocess_matrix(input_matrix);

    std::cout << "--- Cartesian Product of Indices ---\n";
    std::cout << "Total expected combinations: 18\n\n";
    
    ProductRange product_range(valid_options);
    
    int count = 0;
    
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
