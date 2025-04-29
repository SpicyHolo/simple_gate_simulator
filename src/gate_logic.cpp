#include "gate_logic.h"

Gate::Gate(std::string name, std::vector<std::string> inputs, std::string output) 
        : name(std::move(name)), inputs(std::move(inputs)), output(std::move(output)) {}


void Gate::addInputGate(Gate* gate) {
        input_gates.push_back(gate);
    }
void Gate::addOutputGate(Gate* gate) {
    output_gates.push_back(gate);
}

void Gate::print() const {
    std::cout << name << " [" << getType() << "]: ";
    
    bool first = true;
    for (auto &in : inputs) {
        if (!first) std::cout << ", ";
        std::cout << in;
        first = false;
    }
    std::cout << " -> " << output << std::endl;

}    

ANDGate::ANDGate(std::string name, std::string in1, std::string in2, std::string out)
    : Gate(std::move(name), 
           std::vector<std::string>{std::move(in1), std::move(in2)}, 
           std::move(out)) {}
std::string ANDGate::getType() const { return "AND"; } 

void ANDGate::evaluate(std::unordered_map<std::string, bool>& values) {
    bool in1 = values[inputs[0]];
    bool in2 = values[inputs[1]];
    bool value = in1 && in2;
    values[output] = value;
}

ORGate::ORGate(std::string name, std::string in1, std::string in2, std::string out)
    : Gate(std::move(name), 
           std::vector<std::string>{std::move(in1), std::move(in2)}, 
           std::move(out)) {}

std::string ORGate::getType() const { return "OR"; } 
void ORGate::evaluate(std::unordered_map<std::string, bool>& values) {
        bool in1 = values[inputs[0]];
        bool in2 = values[inputs[1]];
        bool value = in1 || in2;
        values[output] = value;
    }

NORGate::NORGate(std::string name, std::string in1, std::string in2, std::string out)
    : Gate(std::move(name), 
           std::vector<std::string>{std::move(in1), std::move(in2)}, 
           std::move(out)) {}

std::string NORGate::getType() const { return "NOR"; } 
void NORGate::evaluate(std::unordered_map<std::string, bool>& values) {
    bool in1 = values[inputs[0]];
    bool in2 = values[inputs[1]];
    bool value = !(in1 || in2);
    values[output] = value;
}

NANDGate::NANDGate(std::string name, std::string in1, std::string in2, std::string out)
    : Gate(std::move(name), 
           std::vector<std::string>{std::move(in1), std::move(in2)}, 
           std::move(out)) {}

std::string NANDGate::getType() const { return "NAND"; } 
void NANDGate::evaluate(std::unordered_map<std::string, bool>& values) {
    bool in1 = values[inputs[0]];
    bool in2 = values[inputs[1]];
    bool value = !(in1 && in2);
    values[output] = value;
}

XORGate::XORGate(std::string name, std::string in1, std::string in2, std::string out)
    : Gate(std::move(name), 
           std::vector<std::string>{std::move(in1), std::move(in2)}, 
           std::move(out)) {}

std::string XORGate::getType() const { return "NOR"; }
void XORGate::evaluate(std::unordered_map<std::string, bool>& values) {
    bool in1 = values[inputs[0]];
    bool in2 = values[inputs[1]];
    bool value = in1 ^ in2;
    values[output] = value;
}

XNORGate::XNORGate(std::string name, std::string in1, std::string in2, std::string out)
        : Gate(std::move(name), 
               std::vector<std::string>{std::move(in1), std::move(in2)}, 
               std::move(out)) {}

std::string XNORGate::getType() const { return "XNOR"; } 
void XNORGate::evaluate(std::unordered_map<std::string, bool>& values) {
    bool in1 = values[inputs[0]];
    bool in2 = values[inputs[1]];
    bool value = !(in1 ^ in2);
    values[output] = value;
}

NOTGate::NOTGate(std::string name, std::string in, std::string out)
    : Gate(std::move(name), 
           std::vector<std::string>{std::move(in)}, 
           std::move(out)) {}

std::string NOTGate::getType() const { return "NOT"; } 
void NOTGate::evaluate(std::unordered_map<std::string, bool>& values) {
    bool in = values[inputs[0]];
    bool value = !in;
    values[output] = value;
}

Gate* createGate(const std::string& type,
                 const std::string& name,
                 const std::string& in1,
                 const std::string& in2,
                 const std::string& out) {

    if (type == "AND") return new ANDGate(name, in1, in2, out);
    if (type == "OR") return new ORGate(name, in1, in2, out);
    if (type == "NAND") return new NANDGate(name, in1, in2, out);
    if (type == "NOR") return new NORGate(name, in1, in2, out);
    if (type == "XOR") return new XORGate(name, in1, in2, out);
    if (type == "XNOR") return new XNORGate(name, in1, in2, out);
    
    // Handle NOTGate separately, since it only takes 1 input
    if (type == "NOT") return new NOTGate(name, in1, out);

    std::cerr << "Unknown gate type: " << type << "\n";
    return nullptr;
}

// Parses netlist from CSV
// The data format is a directed graph, where each node (gate), has pointers to it's INPUT GATES
std::unordered_map<std::string, Gate*> parseGateCSV(const std::string& filename) {
    std::unordered_map<std::string, Gate*> gates; // Maps gate name to gate

    // Maps to add pointers, to previous and next gates. (gates that current gates depends on directly and gates that depend on current gate directly)
    // Needed to avoid nested loops, where we need to search all the gates.
    std::unordered_map<std::string, Gate*> outputs; // Maps output name to gate
    std::unordered_map<std::string, std::set<Gate*>> dependentGates; // Maps output name to list of gates dependent on it

    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return gates;
    }

    std::getline(file, line); // Skip header
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string name, type, in1, in2, out;

        std::getline(ss, name, ',');
        std::getline(ss, type, ',');
        std::getline(ss, in1, ',');
        std::getline(ss, in2, ',');
        std::getline(ss, out, ',');
        Gate* gate = createGate(type, name, in1, in2, out);

        if (gate) {
            gates[name] = gate;
            outputs[out] = gate;

            if (!in1.empty()) dependentGates[in1].insert(gate);
            if (!in2.empty()) dependentGates[in2].insert(gate);
        }
    }

    // Set pointers in graph.
    for (auto& pair : gates) {
        auto name = pair.first;
        auto gate = pair.second;

        // Add input gates pointers
        for (auto& input_name : gate->inputs) {
            if (input_name != NULL_GATE_NAME && (outputs.count(input_name) > 0)) {
                gate->addInputGate(outputs[input_name]);
            }
        }

        // Add output gate pointers
        if (dependentGates.count(gate->output) > 0) {
            for (auto* dependent_gate : dependentGates[gate->output]) {
                gate->addOutputGate(dependent_gate);
            }
        
        }
    }

    return gates;
}

std::vector<std::string> TopologicalSort(const std::unordered_map<std::string, Gate*> &gates) {
    std::queue<std::string> q;
    std::vector<std::string> sorted;
    std::unordered_map<std::string, int> ranks;

    for (auto& pair : gates) {
        std::string gate_name = pair.first;
        for (const auto &dependent_gate : gates.at(gate_name)->output_gates) {
            ranks[dependent_gate->name]++;
        }
    }

    for (auto &pair : gates) {
        std::string gate_name = pair.first;
        if (ranks[gate_name] == 0) {
            q.push(pair.first);
        }
    }

    while (!q.empty()) {
        std::string gateName = q.front();    
        q.pop();
        sorted.push_back(gateName);
        
        for (const auto &dependent_gate : gates.at(gateName)->output_gates) {
            ranks[dependent_gate->name]--;
            if (ranks[dependent_gate->name] == 0) {
                q.push(dependent_gate->name);
            }
        }
    }

    if (sorted.size() != gates.size()) {
        std::cerr << "Cycle detected! Topological sort not possible.\n";
        return {};
    }

    return sorted;
}

void simulate(const std::unordered_map<std::string, Gate*>& gates, 
         const std::vector<std::string>& order, 
         std::unordered_map<std::string, bool>& variables) {
    for (auto& gate_name : order) {
        auto gate = gates.at(gate_name);
        gate->evaluate(variables);
    }
}

std::unordered_map<std::string, bool> getVariableVector(const std::unordered_map<std::string, Gate*> &gates, const std::unordered_map<std::string, bool> &inputs) {
    std::unordered_map<std::string, bool> variables;
    for (auto& pair : gates) {
        auto gate = pair.second;
        for (auto &in : gate->inputs) {
            if (in != NULL_GATE_NAME) variables[in] = 0;
        }

        if (gate->output != NULL_GATE_NAME) variables[gate->output] = 0;
    }
    
    for (auto& pair : inputs) {
       variables[pair.first] = pair.second;
    }

    return variables;
}
