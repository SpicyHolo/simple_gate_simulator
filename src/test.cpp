#include <gtest/gtest.h>
#include "../include/gate_logic.h"

#include <ranges>

TEST(MUX, NetListSimulation) {
    std::unordered_map<std::string, Gate*> gates = parseGateCSV("./netlists/mux.csv");
    auto sorted = TopologicalSort(gates);
    
    std::vector<std::unordered_map<std::string, bool>> test_vectors = {
        {{"A", 0}, {"B", 0}, {"S", 0}},
        {{"A", 0}, {"B", 0}, {"S", 1}},
        {{"A", 0}, {"B", 1}, {"S", 1}},
        {{"A", 0}, {"B", 1}, {"S", 0}},
        {{"A", 1}, {"B", 1}, {"S", 0}},
        {{"A", 1}, {"B", 0}, {"S", 0}},
        {{"A", 1}, {"B", 0}, {"S", 1}},
        {{"A", 1}, {"B", 1}, {"S", 1}},
    };
    
    std::vector<bool> expected_value = {0, 0, 1, 0, 1, 1, 0, 1};

    auto it_a = test_vectors.begin();
    auto it_b = expected_value.begin();

    for (; it_a != test_vectors.end() && it_b != expected_value.end(); ++it_a, ++it_b) {
        auto inputs = *it_a;
        auto expected = *it_b;
        // Prepare variables vector
        auto variables = getVariableVector(gates, *it_a);
        
        simulate(gates, sorted, variables);
        
        bool actual_output = variables.at("OUT");
        ASSERT_EQ(actual_output, expected) << "Failed for input combination: " 
                                           << "A=" << inputs.at("A") 
                                           << ", B=" << inputs.at("B") 
                                           << ", S=" << inputs.at("S");
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
