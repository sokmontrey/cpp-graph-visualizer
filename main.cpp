#include <cstdlib>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

#define WIDTH 800
#define HEIGHT 450
#define RADIUS 30

#define REPEL_FACTOR 0.1
#define MIN_DISTANCE 100

#define DISTANCE_CONSTRAINT_FACTOR 0.1
#define DISTANCE_MAX_DISTANCE 120

class Point {
public:
  Point() = default;

  Vector2 getPos() { return this->pos; }

  void setPos(Vector2 new_pos) { this->pos = new_pos; }

private:
  Vector2 pos = {static_cast<float>(300 + rand() % (100)),
                 static_cast<float>(100 + rand() % (250))};
};

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

class Graph {
public:
  Graph() = default;

  Graph &addNode(string node_name) {
    this->nodes[node_name] = Node(node_name);
    nodes_list.push_back(&this->nodes[node_name]);
    return *this;
  }

	Node* getNode(string node_name) {
		return &this->nodes[node_name];
	}

  Graph &connect(string node1_name, string node2_name, double weight) {
    this->nodes[node1_name].connectTo(&this->nodes[node2_name], weight);
    this->nodes[node2_name].connectTo(&this->nodes[node1_name], weight);
    return *this;
  }

  Graph &connectTo(string node1_name, string node2_name, double weight) {
    this->nodes[node1_name].connectTo(&this->nodes[node2_name], weight);
    return *this;
  }

  vector<Node *> &getNodesList() { return this->nodes_list; }

private:
  unordered_map<string, Node> nodes;
  vector<Node *> nodes_list;
};

int main() {
  srand((unsigned)time(NULL));

  Graph g;
  vector<Node *> &nodes = g.getNodesList();

  bool is_create = false;
  string input = "";

  InitWindow(WIDTH, HEIGHT, "Graph visualizer");

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    /* Logic */

    // min distance constraint for all nodes
    for (int i = 0; i < nodes.size(); i++) {
      for (int j = i + 1; j < nodes.size(); j++) {
        Node *node1 = nodes[i];
        Node *node2 = nodes[j];
        Vector2 pos1 = node1->point.getPos();
        Vector2 pos2 = node2->point.getPos();

        float d = Vector2Distance(pos1, pos2);
        float diff = MIN_DISTANCE - d;
        if (diff > 0) {
          Vector2 dir = Vector2Normalize(Vector2Subtract(pos2, pos1));
          pos1 = Vector2Add(pos1, Vector2Scale(dir, -diff * REPEL_FACTOR / 2));
          pos2 = Vector2Add(pos2, Vector2Scale(dir, diff * REPEL_FACTOR / 2));

          node1->point.setPos(pos1);
          node2->point.setPos(pos2);
        }
      }
    }

    // distance constraint for connected nodes
    for (Node *node : nodes) {
      vector<pair<Node *, double>> &connected = node->getConnected();
      Vector2 pos1 = node->point.getPos();
      for (auto &[other, _] : connected) {
        Vector2 pos2 = other->point.getPos();
        float d = Vector2Distance(pos1, pos2);
        float diff = d - DISTANCE_MAX_DISTANCE;
        if (d > 0) {
          Vector2 dir = Vector2Normalize(Vector2Subtract(pos2, pos1));
          pos1 = Vector2Add(
              pos1, Vector2Scale(dir, diff * DISTANCE_CONSTRAINT_FACTOR / 2));
          pos2 = Vector2Add(
              pos2, Vector2Scale(dir, -diff * DISTANCE_CONSTRAINT_FACTOR / 2));

          node->point.setPos(pos1);
          other->point.setPos(pos2);
        }
      }
    }

    // Move nodes with mouse
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      for (Node *node : nodes) {
        Vector2 pos = node->point.getPos();
        Vector2 mouse_pos = GetMousePosition();
        float d = Vector2Distance(mouse_pos, pos);
        if (d < RADIUS) {
          node->point.setPos(mouse_pos);
          break;
        }
      }
    }

    // create new node
    if (IsKeyPressed(KEY_A)) {
      is_create = true;
    } else if (IsKeyPressed(KEY_ENTER)) {
      is_create = false;
			g.addNode(input);
			g.getNode(input)->point.setPos(GetMousePosition());
      input = "";
    } else {
			int key = GetKeyPressed();
      if (key >= 32 && key <= 125) {
        input += static_cast<char>(key);
      }
    }

    // connect nodes

    BeginDrawing();
    ClearBackground(WHITE);
    /* Rendering */

    // draw all connections
    for (Node *node : nodes) {
      Vector2 pos = node->point.getPos();
      for (auto &[other, weight] : node->getConnected()) {
        Vector2 other_pos = other->point.getPos();
        DrawLineEx(pos, other_pos, 3, LIGHTGRAY);
      }
    }

    // draw all nodes
    for (Node *node : nodes) {
      Vector2 pos = node->point.getPos();
      DrawCircleV(pos, RADIUS, BLUE);
      DrawText(node->getName().c_str(), pos.x - 7, pos.y - 10, RADIUS, WHITE);
    }

    EndDrawing();
  }
  CloseWindow();

  return 0;
}
