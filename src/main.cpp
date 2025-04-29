#include "../include/gate_logic.h"

int main(int argc, char* argv[]) {
    // Load netlist into a graph, sort gates topologically
    std::string filename = "./netlist.csv";
    auto gates = parseGateCSV(filename);
    auto sorted = TopologicalSort(gates);
    // Prepare variables vector
    auto variables = getVariableVector(gates);

    // Parse input
    if (argc % 2 == 0) {
        std::cerr << "Invalid number of arguemnts. Provide key-value pairs. \n";
        return 1;
    }
    
    for (int i = 1; i < argc; i += 2) {
        std::string key = argv[i];
        std::string valueStr = argv[i+1];

        if (valueStr == "0") {
            variables[key] = false;
        } else if (valueStr == "1") {
            variables[key] = true;
        } else {
            std::cerr << "Invalid value for key '" << key << "'. Expected 0 or 1. \n";
            return 1;
        }
    }

    std::cout << "Variables: ";
    for (auto & v : variables) {
        std::cout << v.first << ": " << v.second << ", ";
    }
    std::cout << std::endl;

    // Simulation    
    simulate(gates, sorted, variables);
    std::cout << "Simulating..." << std::endl;

    // Print variables state
    std::cout << "Variables: ";
    for (auto & v : variables) {
        std::cout << v.first << ": " << v.second << ", ";
    }
    std::cout << std::endl;

    // Clean up dynamically allocated memory
    for (auto& gate : gates) {
        delete gate.second;  // Delete the gate objects after use
    }

    return 0;
}

