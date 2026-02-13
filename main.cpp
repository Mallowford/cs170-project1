#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <set>

/* FOR GRAPH INFO ONLY */
#include <chrono>
int nodesExpanded = 0;
int maxQueueSize = 0;

/* PREDEFINED */
std::vector<std::vector<std::vector<int>>> predefined = { // This list is from the lab1 report example nodes
    {{1, 2, 3}, {4, 5, 6}, {7, 8, 0}}, // Depth 0 * this
    {{1, 2, 3}, {4, 5, 6}, {0, 7, 8}}, // Depth 2
    {{1, 2, 3}, {5, 0, 6}, {4, 7, 8}}, // Depth 4 * this
    {{1, 3, 6}, {5, 0, 2}, {4, 7, 8}}, // Depth 8 * this
    {{1, 3, 6}, {5, 0, 7}, {4, 8, 2}}, // Depth 12 * this
    {{1, 6, 7}, {5, 0, 3}, {4, 8, 2}}, // Depth 16 * this
    {{7, 1, 2}, {4, 8, 5}, {6, 3, 0}}, // Depth 20 * this
    {{0, 7, 2}, {4, 6, 1}, {3, 5, 8}} // Depth 24 * this
};

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
// Interface Functions
class Interface {
    public:
        void print_state(const std::vector<std::vector<int>>& state);
        void print_solutionPath(Node* node);
        QueueFunction ask_user_input(Problem& problem);
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

    maxQueueSize = std::max(static_cast<int>(nodes.size()), maxQueueSize);
};

std::vector<Node*> make_node(const std::vector<std::vector<int>>& state); // This is just for matching more with the slides
bool EMPTY(const std::queue<Node*>& nodes);
void make_queue(std::queue<Node*>& nodes, std::vector<Node*> nodes_list, QueueFunction queue_type);
Node* remove_front(std::queue<Node*>& nodes); // Mostly for matching with slides
std::vector<Node*>* expand(Node* node, Problem& problem);
bool test_insert(std::vector<Node*>& children, Problem& problem, Node* node, Node* parent);

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
    // Problem problem;
    Interface interface;
    // problem.initial_state = {{1, 2, 3}, {4, 5, 6}, {0, 7, 8}};
    int totalIterations = 30;
    double totalSeconds = 0;
    long long totalExpanded = 0;
    long long totalQueueSize = 0;
    int depth = 0;
    for (int heuristics = 0; heuristics < 3; heuristics++) {
        QueueFunction method = static_cast<QueueFunction>(heuristics);
        std::string name = "";
        for (int j = 0; j < 8; j++) {
            for (int i = 0; i < totalIterations; i++) {
                Problem problem;

                problem.initial_state = predefined.at(j);
                problem.goal = {{1, 2, 3}, {4, 5, 6}, {7, 8, 0}};

                // interface.print_state(problem.initial_state);

                auto start = std::chrono::steady_clock::now();
                Node* solution = general_search(problem, static_cast<QueueFunction>(heuristics));
                auto end = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                
                /*
                std::cout << "Seconds: " << static_cast<double>(duration.count())/1000 << "\n";
                std::cout << "Nodes Expanded: " << nodesExpanded << "\n";
                std::cout << "Max Queue Size: " << maxQueueSize << "\n";
                std::cout << "Depth Size: " << solution->depth << "\n";
                */

                totalSeconds += (static_cast<double>(duration.count())/1000);
                totalExpanded += nodesExpanded;
                totalQueueSize += maxQueueSize;
                

                nodesExpanded = 0;
                maxQueueSize = 0;
                depth = solution->depth;
                delete solution;
            };
            std::cout << "======= DEPTH: " << depth << " =========\n";
            
            switch (method) {
                case UniformCost:
                    name = "UniformCost";
                    break;
                case A_Star_Manhattan:
                    name = "Manhattan";
                    break;
                case A_Star_MispalacedTiles:
                    name = "Misplaced";
                    break;
            };

            std::cout << "Method: " << name << "\n"; 
            std::cout << "Average Runtime: " << totalSeconds/totalIterations << "\n";
            std::cout << "Average Nodes Expanded: " << totalExpanded/totalIterations << "\n";
            std::cout << "Average Max Queue Size: " << totalQueueSize/totalIterations << "\n";
            totalSeconds = 0;
            totalExpanded = 0;
            totalQueueSize = 0;
        };

    };
    


    // interface.print_solutionPath(solution);

    
/*
    if (solution) {
        std::cout << "Solution found!" << "\n";
    } else {
        std::cout << "No solution found." << "\n";
    };
*/
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
std::vector<Node*>* expand(Node* node, Problem& problem) {
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

    nodesExpanded += (*children).size();
    return children;
};

// Helper Functions
void apply_manhattan(std::vector<Node*>& nodes_list) { // Applies the Manhattan distance heuristic to the nodes in the list
    for (Node* node : nodes_list) {
        int manhattan = 0;

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
bool test_insert(std::vector<Node*>& children, Problem& problem, Node* node, Node* parent) { // Checks if the expanded node will be a duplicate, if not then it will be expanded
    if (problem.visited_states.find(node->data) != problem.visited_states.end()) {
        delete node;
        return false;
    };

    node->parent = parent;
    node->g_N = parent->g_N + 1;
    node->depth = parent->depth + 1;
    children.push_back(node);
    problem.visited_states.insert(node->data);
    return true;
};
void calculate_f_N(std::vector<Node*>& nodes_list) { // Calculates f(n) for the nodes in the list
    for (Node* node : nodes_list) {
        node->f_N = node->g_N + node->h_N;
    };
};
void Interface::print_state(const std::vector<std::vector<int>>& state) {
    for (int i = 0; i < state.size(); i++) {
        for (int j = 0; j < state.at(i).size(); j++) {
            std::cout << state.at(i).at(j) << " ";
        };
        std::cout << "\n";
    };
};
void Interface::print_solutionPath(Node* node) {
    if (!node) {
        return;
    };
    print_solutionPath(node->parent);
    print_state(node->data);
    std::cout << "=========\n";
};
QueueFunction Interface::ask_user_input(Problem& problem) {
    std::cout << "Would you like to use a predefined state (Y) or use a custom state (N)? \n";
    char input = '0';
    int int_input = 0;
    std::cin >> input;
    std::cin.clear();
    input = tolower(input);
    if (input == 'y') {
        std::cout << "Pick a state (0 - 7) \n";
        std::cout << "Possible States Depths: 0, 2, 4, 8, 12, 16, 20, 24 \n";
        std::cin >> int_input;
        std::cin.clear();
        problem.initial_state = predefined.at(int_input);
    }
    else { // Hard coding 3x3 vector for now
        std::vector<std::vector<int>> result;
        std::cout << "List vector in order by column then moving to row \n";
        std::cout << "Example: 1 2 3 4 5 6 7 8 9 is: Row 1 123\n";
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                std::cin >> int_input;
                result.at(i).at(j) = int_input;
            };
        };
        std::cin.clear();
        problem.initial_state = result;
    };
    std::cout << "Pick a Heuristic Type: Uniform (0/default), Manhattan (1), Misplaced Tiles(2)" << "\n";
    std::cin >> int_input;
    if (int_input > 2) {
        return UniformCost;
    };

    return static_cast<QueueFunction>(int_input);
};