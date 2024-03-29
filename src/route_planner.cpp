#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // TODO 2: Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
  	
	  RoutePlanner::start_node = & (m_Model.FindClosestNode(start_x, start_y));
  	RoutePlanner::end_node   = & (m_Model.FindClosestNode(end_x, end_y));
}


// TODO 3: Implement the CalculateHValue method.
// Tips:
// - You can use the distance to the end_node for the h value.
// - Node objects have a distance method to determine the distance to another node.

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
	return node->distance(*RoutePlanner::end_node);
}


// TODO 4: Complete the AddNeighbors method to expand the current node by adding all unvisited neighbors to the open list.
// Tips:
// - Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// - For each node in current_node.neighbors, set the parent, the h_value, the g_value. 
// - Use CalculateHValue below to implement the h-Value calculation.
// - For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
	current_node->FindNeighbors();
  for ( auto * neighbor_node : current_node->neighbors){
    neighbor_node->parent = current_node;
    neighbor_node->h_value = CalculateHValue(neighbor_node);
    neighbor_node->g_value = current_node->g_value + neighbor_node->distance(*current_node);
    
    RoutePlanner::open_list.push_back(neighbor_node);
    neighbor_node->visited = true;
  }

}


// TODO 5: Complete the NextNode method to sort the open list and return the next node.
// Tips:
// - Sort the open_list according to the sum of the h value and g value.
// - Create a pointer to the node in the list with the lowest sum.
// - Remove that node from the open_list.
// - Return the pointer.
bool CompareFValues(RouteModel::Node const *A, RouteModel::Node const *B ){
  return (A->g_value + A->h_value) < (B->g_value + B->h_value);
}

RouteModel::Node *RoutePlanner::NextNode() {
	std::sort(RoutePlanner::open_list.begin(), RoutePlanner::open_list.end(), CompareFValues);
  auto * node = RoutePlanner::open_list[0];
  	
  // remove node from open list
  RoutePlanner::open_list.erase(RoutePlanner::open_list.begin());
    
  return node;
}


// TODO 6: Complete the ConstructFinalPath method to return the final path found from your A* search.
// Tips:
// - This method should take the current (final) node as an argument and iteratively follow the 
//   chain of parents of nodes until the starting node is found.
// - For each node in the chain, add the distance from the node to its parent to the distance variable.
// - The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;

    // TODO: Implement your solution here.
    while(true){
      
      // add node into the path
      path_found.push_back(*current_node);
      
      auto * parent = current_node->parent;
      distance += current_node->g_value - parent->g_value;
      
      if (parent == RoutePlanner::start_node){
        // add the start node into the path and breack
        path_found.push_back(*parent);
        
        // reorder
        std::reverse(path_found.begin(), path_found.end());
        break;
      }
      
      // update current_node
      current_node = parent;
    }
  	
    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.

    // return path
    return path_found;

}


// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node =  nullptr;

    // TODO: Implement your solution here.
    RoutePlanner::start_node->parent = nullptr;
    RoutePlanner::start_node->visited = true;

    RoutePlanner::open_list.push_back(RoutePlanner::start_node);
    
  	AddNeighbors(RoutePlanner::start_node);
    
  	while ( ! RoutePlanner::open_list.empty()){
      
      current_node = NextNode();
      // find current_node neighbors and add them into the open_list
      AddNeighbors(current_node);
      
      if (current_node->g_value == RoutePlanner::end_node->g_value){
        m_Model.path = ConstructFinalPath(current_node);
        break;
      }
    }	
    
}