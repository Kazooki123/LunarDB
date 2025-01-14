use std::collections::HashMap;
use crate::nodes::Node;

pub struct Graph {
  pub adjacency_list: HashMap<Node, Vec<(Node, f64)>>,
}

impl Graph {
  pub fn new() -> Self {
    Graph {
      adjacency_list: HashMap::new(),
    }
  }

  pub fn add_node(&mut self, node: Node) {
    self.adjacency_list.entry(node).or_insert(vec![]);
  }

  pub fn add_edge(&mut self, from: Node, to: Node, weight: f64) {
    self.adjacency_list.entry(from.clone()).or_default().push((to.clone(), weight));

    self.adjacency_list.entry(to).or_default();
  }

  pub fn get_neighbors(&self, node: &Node) -> Option<&Vec<(Node, f64)>> {
    self.adjacency_list.get(node)
  }
}
