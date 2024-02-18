#include <ctime>
#include <iostream>
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
#define RADIUS 20
#define STROKE 3

Node *hoveredNode(vector<Node *> &nodes, Vector2 pos, float radius) {
  for (Node *node : nodes) {
    Vector2 node_pos = node->point.getPos();
    float d = Vector2Distance(pos, node_pos);
    if (d < radius)
      return node;
  }
  return nullptr;
}

int main() {
  srand((unsigned)time(NULL));

  Graph graph;
  vector<Node *> &nodes = graph.getNodesList();

  bool is_move = false;
  bool is_connect = false;

  Node *selected_node = nullptr;

  InitWindow(WIDTH, HEIGHT, "Graph visualizer");
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    /* Input */

    /* Add or Move Node */
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      Vector2 mouse_pos = GetMousePosition();
      Node *node = hoveredNode(nodes, mouse_pos, RADIUS);
      if (node == nullptr)
        graph.addNode()->point.setPos(mouse_pos);
      else {
        is_move = true;
        selected_node = node;
      }
    }

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
      Vector2 mouse_pos = GetMousePosition();
      Node *node = hoveredNode(nodes, mouse_pos, RADIUS);
      if (node != nullptr) {
        if (!is_connect) {
          is_connect = true;
          selected_node = node;
        } else {
          if (selected_node != nullptr) {
            double weight = Vector2Distance(node->point.getPos(),
                                            selected_node->point.getPos());
            node->connectTo(selected_node, weight);
            selected_node->connectTo(node, weight);
            is_connect = false;
            selected_node = nullptr;
          }
        }
      }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
      is_move = false;
      selected_node = nullptr;
    }

    if (is_move)
      selected_node->point.setPos(GetMousePosition());

    BeginDrawing();
    ClearBackground(WHITE);
    /* Rendering */

    /* Draw Connections */
    for (Node *node : nodes) {
      Vector2 pos = node->point.getPos();
      for (auto &[other, weight] : node->getConnected()) {
        Vector2 other_pos = other->point.getPos();
        DrawLineEx(pos, other_pos, STROKE, GRAY);
      }
    }

    /* Draw Nodes */
    for (Node *node : nodes) {
      Vector2 pos = node->point.getPos();
      DrawCircleV(pos, RADIUS, BLACK);
      DrawText(node->getName().c_str(), pos.x - 10, pos.y - 10, 20, BLACK);
    }

    /* Draw selected node */
    if (selected_node != nullptr) {
      Vector2 pos = selected_node->point.getPos();
      Color color = is_connect ? GREEN : BLUE;
      DrawCircleV(pos, RADIUS, color);
    }

    EndDrawing();
  }
  CloseWindow();

  return 0;
}
