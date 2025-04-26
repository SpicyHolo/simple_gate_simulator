#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <set>
#include <queue>

#define NULL_GATE_NAME "N/A"
#define DEBUG true

// Gate has pointer to its inputs and outputs
class Gate {
public:
    std::string instance;
    std::string input1_name;
    std::string input2_name;
    std::string output_name;

    Gate* input1 = nullptr;
    Gate* input2 = nullptr;

    bool value;

    Gate(std::string inst, std::string in1, std::string in2, std::string out)
        : instance(std::move(inst)), input1_name(std::move(in1)), input2_name(std::move(in2)), output_name(std::move(out)) {}

    virtual std::string getType() const = 0;
    virtual ~Gate() = default;
    virtual void print() const {
        std::cout << instance << " [" << getType() << "]: "
                  << input1_name << ", " << input2_name << " -> " << output_name << std::endl;
    }
    virtual bool evaluate(std::unordered_map<std::string, bool>& values) = 0; //
};

class ANDGate : public Gate {
public:
    ANDGate(std::string inst, std::string in1, std::string in2, std::string out)
        : Gate(inst, in1, in2, out) {} 
    std::string getType() const override { return "AND"; } 
    bool evaluate(std::unordered_map<std::string, bool>& values) override {
        bool input1_val = values[input1_name];
        bool input2_val = values[input2_name];
        value = input1_val && input2_val;
        return value;
    }
};

class ORGate : public Gate {
public:
    ORGate(std::string inst, std::string in1, std::string in2, std::string out)
        : Gate(inst, in1, in2, out) {} 

    std::string getType() const override { return "OR"; } 
    bool evaluate(std::unordered_map<std::string, bool>& values) override {
        bool input1_val = values[input1_name];
        bool input2_val = values[input2_name];
        value = input1_val || input2_val;
        return value;
    }
};

class NOTGate : public Gate {
public:
   NOTGate(std::string inst, std::string in1, std::string out)
        : Gate(inst, in1, NULL_GATE_NAME, out) {} 

    std::string getType() const override { return "NOT"; } 
    bool evaluate(std::unordered_map<std::string, bool>& values) override {
        bool input_val = values[input1_name];
        value = !input_val;
        return value;
    }
};

class NANDGate : public Gate {
public:
    NANDGate(std::string inst, std::string in1, std::string in2, std::string out)
        : Gate(inst, in1, in2, out) {} 

    std::string getType() const override { return "NAND"; } 
    bool evaluate(std::unordered_map<std::string, bool>& values) override {
        bool input1_val = values[input1_name];
        bool input2_val = values[input2_name];
        value = !(input1_val && input2_val);
        return value;
    }
};

class XORGate : public Gate {
    public:
        XORGate(std::string inst, std::string in1, std::string in2, std::string out)
            : Gate(inst, in1, in2, out) {} 

        std::string getType() const override { return "XOR"; } 
        bool evaluate(std::unordered_map<std::string, bool>& values) override {
            bool input1_val = values[input1_name];
            bool input2_val = values[input2_name];
            value = input1_val ^ input2_val;
            return value;
        }
};

class XNORGate : public Gate {
    public:
        XNORGate(std::string inst, std::string in1, std::string in2, std::string out)
            : Gate(inst, in1, in2, out) {} 

        std::string getType() const override { return "XNOR"; } 
        bool evaluate(std::unordered_map<std::string, bool>& values) override {
            bool input1_val = values[input1_name];
            bool input2_val = values[input2_name];
            value = !(input1_val ^ input2_val);
            return value;
        }
};

class NORGate : public Gate {
    public:
        NORGate(std::string inst, std::string in1, std::string in2, std::string out)
            : Gate(inst, in1, in2, out) {} 

        std::string getType() const override { return "NOR"; } 
        bool evaluate(std::unordered_map<std::string, bool>& values) override {
            bool input1_val = values[input1_name];
            bool input2_val = values[input2_name];
            value = !(input1_val ^ input2_val);
            return value;
        }
};
Gate* createGate(const std::string& type,
                 const std::string& instance,
                 const std::string& in1,
                 const std::string& in2,
                 const std::string& out) {
    if (type == "AND") return new ANDGate(instance, in1, in2, out);
    if (type == "OR") return new ORGate(instance, in1, in2, out);
    if (type == "NOT") return new NOTGate(instance, in1, out);
    if (type == "NAND") return new NANDGate(instance, in1, in2, out);
    if (type == "XOR")return new XORGate(instance, in1, in2, out);
    if (type == "XNOR")return new XNORGate(instance, in1, in2, out);
    if (type == "NOR")return new NORGate(instance, in1, in2, out);



    std::cerr << "Unknown gate type: " << type << "\n";
    return nullptr;
}

// Parses netlist from CSV
// The data format is a directed graph, where each node (gate), has pointers to it's INPUT GATES
std::vector<Gate*> parseGateCSV(const std::string& filename) {
    std::vector<Gate*> gates;
    std::unordered_map<std::string, Gate*> outputs;

    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return gates;
    }

    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string instance, type, in1, in2, out;

        std::getline(ss, instance, ',');
        std::getline(ss, type, ',');
        std::getline(ss, in1, ',');
        std::getline(ss, in2, ',');
        std::getline(ss, out, ',');
        Gate* gate = createGate(type, instance, in1, in2, out);

        if (gate) {
            gates.push_back(gate);
            outputs[gate->output_name] = gate; // Needed to set gate's input pointers, without looping through the netlist again.
        }
    }

    // Set pointers in graph.
    for (auto& gate : gates) {
        std::string in1 = gate->input1_name;
        std::string in2 = gate->input2_name;

        if (in1 != NULL_GATE_NAME && (outputs.count(in1) > 0)) {
            gate->input1 = outputs[in1];
        }

        if (in2 != NULL_GATE_NAME && (outputs.count(in2) > 0)) {
            gate->input2 = outputs[in2];
        }
    }

    return gates;
}

std::vector<std::string> TopologicalSort(std::vector<Gate*> gates) {
    std::unordered_map<std::string, int> ranks;    
    std::unordered_map<std::string, std::vector<std::string>> dependencies; 
    std::unordered_map<std::string, std::string> out_name_to_gate;
    std::vector<std::string> sorted_gates;

    for (auto& gate : gates) {
        ranks[gate->instance] = 0;  // Initialize in-degree to 0 for all gates
        out_name_to_gate[gate->output_name] = gate->instance;
    }

    for (auto& gate : gates) {
        if (gate->input1_name != NULL_GATE_NAME && gate->input1_name != "A" && gate->input1_name != "B" && gate->input1_name != "C" && gate->input1_name != "D") {
            ranks[gate->instance]++;  // Increment in-degree of the gate that gate->input1 depends on
            dependencies[out_name_to_gate[gate->input1_name]].push_back(gate->instance);  // Track dependency
        }
        if (gate->input2_name != NULL_GATE_NAME && gate->input2_name != "A" && gate->input2_name != "B" && gate->input2_name != "C" && gate->input2_name != "D") {
            ranks[gate->instance]++;  // Increment in-degree of the gate that gate->input2 depends on
            dependencies[out_name_to_gate[gate->input2_name]].push_back(gate->instance);  // Track dependency
        }
    }
    
    // Step 3: Initialize a queue of gates that have 0 in-degree (i.e., no dependencies)
    std::queue<std::string> zero_in_degree;
    for (auto& rank : ranks) {
        if (rank.second == 0) {
            zero_in_degree.push(rank.first);  // Add gate to queue if its in-degree is 0
        }
    }

    // Step 4: Process gates with 0 in-degree and update their dependent gates' in-degrees
    while (!zero_in_degree.empty()) {
        std::string gate_name = zero_in_degree.front();
        zero_in_degree.pop();
        sorted_gates.push_back(gate_name);  // Add the gate to the sorted list

        // For each gate dependent on the current gate, decrease its in-degree
        for (const std::string& dependent : dependencies[gate_name]) {
            ranks[dependent]--;  // Decrease in-degreeo
            if (ranks[dependent] == 0) {
                zero_in_degree.push(dependent);  // If in-degree is 0, add to queue
            
            }
        }
    }

    std::cout << "Sorted gates: "  << std::endl;
    for (auto& gate : sorted_gates) {
        std::cout << gate << std::endl;
    }
    // Step 5: If all gates are processed, return sorted order, otherwise it's a cycle (DAG check)
    if (sorted_gates.size() == gates.size()) {
        return sorted_gates;
    } else {
        std::cerr << "Error: The graph contains a cycle!" << std::endl;
        return sorted_gates;  // Return an empty vector if there's a cycle
    }
}

void simulate(const std::vector<Gate*>& gates, 
         const std::vector<std::string>& sorted_gates, 
         std::unordered_map<std::string, bool>& variables,
         const std::unordered_map<std::string, Gate*>& inst_to_gate) {

    for (auto& inst : sorted_gates) {
        auto gate = inst_to_gate.at(inst);
        auto input1_name = gate->input1_name;
        auto input2_name = gate->input2_name; 
        auto output_name = gate->output_name;

        std::unordered_map<std::string, bool> inputs;
        if (input1_name != NULL_GATE_NAME) inputs[input1_name] = variables[input1_name];
        if (input2_name != NULL_GATE_NAME) inputs[input2_name] = variables[input2_name];

        bool out = gate->evaluate(inputs);
        variables[output_name] = out;
    }
}

int main(int argc, char* argv[]) {

    // Load netlist into a graph, sort gates topologically
    std::string filename = "netlist.csv";
    auto gates = parseGateCSV(filename);
    auto sorted = TopologicalSort(gates);

    // Prepare variables vector
    std::unordered_map<std::string, bool> variables;
    std::unordered_map<std::string, Gate*> inst_to_gate;
    for (auto& gate : gates) {
        if (gate->input1_name != NULL_GATE_NAME) variables[gate->input1_name] = 0;
        if (gate->input2_name != NULL_GATE_NAME) variables[gate->input2_name] = 0;
        if (gate->output_name != NULL_GATE_NAME) variables[gate->output_name] = 0;
        inst_to_gate[gate->instance] = gate;
    }

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
    simulate(gates, sorted, variables, inst_to_gate);
    std::cout << "Simulating..." << std::endl;

    // Print variables state
    std::cout << "Variables: ";
    for (auto & v : variables) {
        std::cout << v.first << ": " << v.second << ", ";
    }
    std::cout << std::endl;

    // Clean up dynamically allocated memory
    for (auto& gate : gates) {
        delete gate;  // Delete the gate objects after use
    }

    return 0;
}