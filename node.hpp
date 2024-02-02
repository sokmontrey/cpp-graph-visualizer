#pragma once

#include <string>
#include "point.hpp"
#include <vector>
#include <utility>

using namespace std;

class Node {
public:
  Point point;

  Node() = default;
  Node(string name) { this->name = name; }

  void connectTo(Node *other, double weight) {
    this->connected.push_back({other, weight});
  }

  void visit() { this->is_visited = true; }

  vector<pair<Node *, double>> &getConnected() { return this->connected; }

  string getName() { return this->name; }

private:
  vector<pair<Node *, double>> connected;
  bool is_visited = false;
  string name = "";
};
