#ifndef GATE_LOGIC_H
#define GATE_LOGIC_H

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


class Gate {
public:
    std::string name;
    
    // Inputs
    std::vector<std::string> inputs;
    std::vector<Gate*> input_gates; 


    // Outptus
    std::string output;
    std::vector<Gate*> output_gates; 

    Gate(std::string name, std::vector<std::string> inputs, std::string output);

    virtual ~Gate() = default;

    void addInputGate(Gate* gate);
    void addOutputGate(Gate* gate);

    virtual std::string getType() const = 0; 
    virtual void evaluate(std::unordered_map<std::string, bool>& values) = 0;
    virtual void print() const;
};

class ANDGate : public Gate {
public:
    ANDGate(std::string name, std::string in1, std::string in2, std::string out);
    std::string getType() const override;
    void evaluate(std::unordered_map<std::string, bool>& values) override;
};

class ORGate : public Gate {
public:
    ORGate(std::string name, std::string in1, std::string in2, std::string out);
    std::string getType() const override;
    void evaluate(std::unordered_map<std::string, bool>& values) override;
};

class NORGate : public Gate {
public:
    NORGate(std::string name, std::string in1, std::string in2, std::string out);
    std::string getType() const override;
    void evaluate(std::unordered_map<std::string, bool>& values) override;
};

class NANDGate : public Gate {
public:
    NANDGate(std::string name, std::string in1, std::string in2, std::string out);
    std::string getType() const override;
    void evaluate(std::unordered_map<std::string, bool>& values) override;
};

class XORGate : public Gate {
public:
    XORGate(std::string name, std::string in1, std::string in2, std::string out);
    std::string getType() const override;
    void evaluate(std::unordered_map<std::string, bool>& values) override;
};

class XNORGate : public Gate {
public:
    XNORGate(std::string name, std::string in1, std::string in2, std::string out);
    std::string getType() const override;
    void evaluate(std::unordered_map<std::string, bool>& values) override;
};

class NOTGate : public Gate {
public:
    NOTGate(std::string name, std::string in, std::string out);

    std::string getType() const override;
    void evaluate(std::unordered_map<std::string, bool>& values) override;
};

Gate* createGate(const std::string& type,
                 const std::string& name,
                 const std::string& in1,
                 const std::string& in2,
                 const std::string& out);

std::unordered_map<std::string, Gate*> parseGateCSV(const std::string& filename);

std::vector<std::string> TopologicalSort(const std::unordered_map<std::string, Gate*> &gates);

void simulate(const std::unordered_map<std::string, Gate*>& gates, 
         const std::vector<std::string>& order, 
         std::unordered_map<std::string, bool>& variables);

std::unordered_map<std::string, bool> getVariableVector(const std::unordered_map<std::string, Gate*> &gates, const std::unordered_map<std::string, bool> &inputs = {});
#endif