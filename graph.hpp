#pragma once

#include <string>
#include <unordered_map>

#include "node.hpp"

using namespace std;

class Graph {
public:
  Graph() = default;

  Node* addNode() { return this->addNode(to_string(this->nodes.size())); }
  Node* addNode(string node_name) {
    this->nodes[node_name] = Node(node_name);
    nodes_list.push_back(&this->nodes[node_name]);
    return &this->nodes[node_name];
  }

  Node *getNode(string node_name) { return &this->nodes[node_name]; }

  Graph &connect(string node1_name, string node2_name, double weight) {
    this->nodes[node1_name].connectTo(&this->nodes[node2_name], weight);
    this->nodes[node2_name].connectTo(&this->nodes[node1_name], weight);
    return *this;
  }

  Graph &connectTo(string node1_name, string node2_name, double weight) {
    this->nodes[node1_name].connectTo(&this->nodes[node2_name], weight);
    return *this;
  }

  int size() { return this->nodes.size(); }

  vector<Node *> &getNodesList() { return this->nodes_list; }

private:
  unordered_map<string, Node> nodes;
  vector<Node *> nodes_list;
};
