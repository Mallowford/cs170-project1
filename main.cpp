#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <set>

// Main Definitions
enum QueueFunction {
    UniformCost,
    A_Star_Manhattan,
    A_Star_MispalacedTiles
};
class Problem {
    public:
        std::vector<std::vector<int>> initial_state;
        std::vector<std::vector<int>> goal;
        std::set<std::vector<std::vector<int>>> visited_states;

        bool goal_state(const std::vector<std::vector<int>>& state) const; // Compares given state to the goal state and returns true if they are the same, else false
};
class Node {
    public:
        Node() : parent(nullptr), g_N(0), h_N(0), f_N(0), depth(0), left(nullptr), right(nullptr), down(nullptr), up(nullptr) {};
        ~Node() {
            delete left;
            delete right;
            delete down;
            delete up;
        };
        std::vector<std::vector<int>> data;

        // g(n) and h(n) for A* search
        Node* parent;
        int g_N; // Cost to get to a node
        int h_N; // Estimated distance to the goal
        int f_N; // g(n) + h(n) or estimated cost of cheapest solution through node n
        int depth;

        // Operators / States of moving the blank tile, "0"
        Node* left;
        Node* right;
        Node* down;
        Node* up;
};
void apply_manhattan(std::vector<Node*>& nodes_list);
void apply_mispalacedTiles(std::vector<Node*>& nodes_list);
void calculate_f_N(std::vector<Node*>& nodes_list);
void queueing_function(std::queue<Node*>& nodes, std::vector<Node*>& nodes_list, QueueFunction queue_type) {
    switch (queue_type) {
        case UniformCost:
            // just adding cheapest node so no particular calculations needed
            break;
        case A_Star_Manhattan:
            apply_manhattan(nodes_list);
            break;
        case A_Star_MispalacedTiles:
            apply_mispalacedTiles(nodes_list);
            break;
        default:
            break;
    };
    calculate_f_N(nodes_list);
    std::sort(nodes_list.begin(), nodes_list.end(), [](Node* a, Node* b) {
        return a->f_N < b->f_N;
    });
    for (Node* node : nodes_list) {
        nodes.push(node);
    };
};

std::vector<Node*> make_node(const std::vector<std::vector<int>>& state); // This is just for matching more with the slides
bool EMPTY(const std::queue<Node*>& nodes);
void make_queue(std::queue<Node*>& nodes, std::vector<Node*> nodes_list, QueueFunction queue_type);
Node* remove_front(std::queue<Node*>& nodes); // Mostly for matching with slides
std::vector<Node*>* expand(Node* node, Problem problem);
bool test_insert(std::vector<Node*>& children, Problem problem, Node* node, Node* parent);

Node* general_search(Problem& problem, QueueFunction queuetype) {
    std::queue<Node*> nodes;
    /* nodes = */ make_queue(nodes, make_node(problem.initial_state), queuetype);

    // Keeps searching until a solution is found or there are no more nodes to expand
    do {
        if (EMPTY(nodes)) {
            return nullptr; //failure
        };
        
        Node* node = remove_front(nodes);

        if (problem.goal_state(node->data)) {
            return node;
        };

        /* nodes = */ queueing_function(nodes, *(expand(node, problem)), queuetype);

    } while (true);
};

int main() {
    Problem problem;
    // problem.initial_state = {{1, 2, 3}, {4, 5, 6}, {0, 7, 8}};

    std::vector<std::vector<std::vector<int>>> predefined = {
        {{1, 2, 3}, {4, 5, 6}, {7, 8, 0}}, // Depth 0
        {{1, 2, 3}, {4, 5, 6}, {0, 7, 8}}, // Depth 2
        {{1, 2, 3}, {5, 0, 6}, {4, 7, 8}}, // Depth 4
        {{1, 3, 6}, {5, 0, 2}, {4, 7, 8}}, // Depth 8
        {{1, 3, 6}, {5, 0, 7}, {4, 7, 8}}, // Depth 12
        {{1, 6, 7}, {5, 0, 3}, {4, 8, 2}}, // Depth 16
        {{7, 1, 2}, {4, 8, 5}, {6, 3, 0}}, // Depth 20
        {{0, 7, 2}, {4, 6, 1}, {3, 5, 8}} // Depth 24
    }; 

    problem.initial_state = predefined.at(0);
    problem.goal = {{1, 2, 3}, {4, 5, 6}, {7, 8, 0}};

    Node* solution = general_search(problem, A_Star_Manhattan);

    if (solution) {
        std::cout << "Solution found!" << "\n";
    } else {
        std::cout << "No solution found." << "\n";
    };

    return 0;
};

// Implementations
std::vector<Node*> make_node(const std::vector<std::vector<int>>& state) {
    Node* node = new Node();
    node->data = state;
    return {node};
};
bool Problem::goal_state(const std::vector<std::vector<int>>& state) const {
    bool is_goal = true;

    for (int i = 0; i < state.size(); i++) {
        for (int j = 0; j < state.at(i).size(); j++) {
            if (state.at(i).at(j) != this->goal.at(i).at(j)) {
                is_goal = false;
                break;
            };
        };
    };
    
    return is_goal;
};
bool EMPTY(const std::queue<Node*>& nodes) {
    return nodes.empty();
};
void make_queue(std::queue<Node*>& nodes, std::vector<Node*> nodes_list, QueueFunction queue_type) {
    queueing_function(nodes, nodes_list, queue_type);
};
Node* remove_front(std::queue<Node*>& nodes) {
    Node* node = nodes.front();
    nodes.pop();
    return node;
};
std::vector<Node*>* expand(Node* node, Problem problem) {
    std::vector<Node*>* children = new std::vector<Node*>();

    for (int i = 0; i < node->data.size(); i++) {
        for (int j = 0; j < node->data.at(i).size(); j++) {
            
            // Blank Tile
            if (node->data.at(i).at(j) == 0) {
            
                // Move Left
                if (j > 0) {
                    Node* left_node = new Node();
                    left_node->data = node->data;
                    
                    // Using built-in std::swap from algorithm to swap the blank tile with the tile to the left of it
                    std::swap(left_node->data.at(i).at(j), left_node->data.at(i).at(j - 1));
                    if (test_insert(*(children), problem, left_node, node)) { // Uses helper function to repeat checks
                        node->left = left_node;
                    };
                };
                
                // Move Right
                if (j < node->data.at(i).size() - 1) {
                    Node* right_node = new Node();
                    right_node->data = node->data;

                    // Using swap to swap blank tile with the tile to the right of it
                    std::swap(right_node->data.at(i).at(j), right_node->data.at(i).at(j + 1));
                    if (test_insert(*(children), problem, right_node, node)) {
                        node->right = right_node;
                    };
                };

                // Move Up
                if (i > 0) {
                    Node* up_node = new Node();
                    up_node->data = node->data;

                    // Using swap to swap blank tile with the tile above it
                    std::swap(up_node->data.at(i).at(j), up_node->data.at(i - 1).at(j));
                    if (test_insert(*(children), problem, up_node, node)) {
                        node->up = up_node;
                    };
                };

                // Move Down
                if (i < node->data.size() - 1) {
                    Node* down_node = new Node();
                    down_node->data = node->data;

                    // Using swap to swap blank tile with the tile below it
                    std::swap(down_node->data.at(i).at(j), down_node->data.at(i + 1).at(j));
                    if (test_insert(*(children), problem, down_node, node)) {
                        node->down = down_node;
                    };
                };
            };
        };
    };

    return children;
};

// Helper Functions
void apply_manhattan(std::vector<Node*>& nodes_list) { // Applies the Manhattan distance heuristic to the nodes in the list
    for (Node* node : nodes_list) {
        int manhattan = 0;

        // O(n^2)
        for (int i = 0; i < node->data.size(); i++) {
            for (int j = 0; j < node->data.at(i).size(); j++) {
                // Skip the blank tile
                if (node->data.at(i).at(j) != 0) {
                    int value = node->data.at(i).at(j);
                    // Calculates the row and column needed to move the tile to its goal position and adds the distance to the total manhattan distance
                    int goal_row = (value - 1) / node->data.size();
                    int goal_col = (value - 1) % node->data.size();
                    manhattan += abs(i - goal_row) + abs(j - goal_col);
                };
            };
        };

        node->h_N = manhattan;
    };
};
void apply_mispalacedTiles(std::vector<Node*>& nodes_list) { // Applies the Misplaced Tiles heuristic to the nodes in the list
    for (Node* node : nodes_list) {
        int misplaced_tiles = 0;

        for (int i = 0; i < node->data.size(); i++) {
            for (int j = 0; j < node->data.at(i).size(); j++) {
                // Skip the blank tile
                if (node->data.at(i).at(j) != 0 && node->data.at(i).at(j) != (i * node->data.size() + j + 1)) {
                    misplaced_tiles++;
                };
            };
        };

        node->h_N = misplaced_tiles;
    };
};
bool test_insert(std::vector<Node*>& children, Problem problem, Node* node, Node* parent) { // Checks if the expanded node will be a duplicate, if not then it will be expanded
    if (problem.visited_states.find(node->data) != problem.visited_states.end()) {
        delete node;
        return false;
    };

    node->parent = parent;
    node->g_N = parent->g_N + 1;
    children.push_back(node);
    problem.visited_states.insert(node->data);
    return true;
};
void calculate_f_N(std::vector<Node*>& nodes_list) { // Calculates f(n) for the nodes in the list
    for (Node* node : nodes_list) {
        node->f_N = node->g_N + node->h_N;
    };
};

// Interface Functions
class Interface {
    public:
        void print_state(const std::vector<std::vector<int>>& state);
        void print_solution(Node* node);
        void print_solutionPath(Node* node);
        void print_queue(const std::queue<Node*>& nodes);
        QueueFunction ask_user_input(Problem& problem);
};