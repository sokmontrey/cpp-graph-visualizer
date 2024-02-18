#include <ctime>
#include <map>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "graph.hpp"
#include "node.hpp"
#include "point.hpp"

using namespace std;

#define WIDTH 800
#define HEIGHT 450
#define RADIUS 30

#define REPEL_FACTOR 0.1
#define MIN_DISTANCE 100

#define DISTANCE_CONSTRAINT_FACTOR 0.1
#define DISTANCE_MAX_DISTANCE 120

enum MODE {
  INSERT,
  EDIT,
  CONNECT,
};

map<MODE, string> mode_to_string = {
    {INSERT, "INSERT"},
    {EDIT, "EDIT"},
    {CONNECT, "CONNECT"},
};

map<MODE, Color> mode_to_color = {
    {INSERT, RED},
    {EDIT, BLUE},
    {CONNECT, GREEN},
};

Node *hoveredNode(vector<Node *> &nodes, Vector2 pos, float radius) {
  for (Node *node : nodes) {
    Vector2 node_pos = node->point.getPos();
    float d = Vector2Distance(pos, node_pos);
    if (d < radius) {
      return node;
    }
  }
  return nullptr;
}

int main() {
  srand((unsigned)time(NULL));

  Graph graph;
  vector<Node *> &nodes = graph.getNodesList();

  bool is_create = false;
  string input = "";
  MODE mode = EDIT;
  Node *selected_node = nullptr;

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

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      // Move nodes with mouse
      Node *hovered_node = hoveredNode(nodes, GetMousePosition(), RADIUS);
      if (hovered_node != nullptr) {
        if (mode == EDIT) {
          hovered_node->point.setPos(GetMousePosition());
          // connect nodes
        } else if (mode == CONNECT) {
          graph.connectTo(selected_node->getName(), hovered_node->getName(), 1);
          selected_node = nullptr;
          mode = EDIT;
        }
      }
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
      Node *hovered_node = hoveredNode(nodes, GetMousePosition(), RADIUS);
      if (hovered_node != nullptr) {
        selected_node = hovered_node;
        mode = CONNECT;
      }
    }

    // create new node
    if (mode == EDIT && IsKeyPressed(KEY_SPACE)) {
      mode = INSERT;
    } else if (mode == INSERT && IsKeyPressed(KEY_ENTER)) {
      mode = EDIT;
      if (input != "") {
        graph.addNode(input);
        graph.getNode(input)->point.setPos(GetMousePosition());
        input = "";
      }
    } else if (mode == INSERT) {
      int key = GetKeyPressed();
      if (key >= 32 && key <= 125) {
        input += static_cast<char>(key);
      } else if (key == KEY_BACKSPACE) {
        input = input.substr(0, input.size() - 1);
      }
    }

    BeginDrawing();
    ClearBackground(WHITE);
    /* Rendering */

    // display user input
    if (mode == INSERT) {
      DrawText(input.c_str(), 10, HEIGHT - 60, 20, BLUE);
    }

    // display mode
    DrawText(mode_to_string[mode].c_str(), 10, HEIGHT - 30, 20,
             mode_to_color[mode]);

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
      string node_name = node->getName();
      DrawText(node_name.c_str(), pos.x + RADIUS + 10, pos.y - 5, 15, GRAY);
    }

    // visualize selected node for connection
    if (selected_node != nullptr) {
      DrawCircleV(selected_node->point.getPos(), RADIUS, RED);
      DrawLineEx(selected_node->point.getPos(), GetMousePosition(), 3, RED);
    }

    EndDrawing();
  }
  CloseWindow();

  return 0;
}
