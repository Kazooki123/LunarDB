use crate::graph::Graph;
use crate::nodes::Node;
use std::collections::{BinaryHeap, HashMap};
use std::cmp::Ordering;

#[derive(Clone)]
struct State {
  cost: f64,
  node: Node,
}

impl PartialEq for State {
    fn eq(&self, other: &Self) -> bool {
        self.cost.partial_cmp(&other.cost).unwrap() == Ordering::Equal &&
        self.node == other.node
    }
}

impl Eq for State {}

impl Ord for State {
    fn cmp(&self, other: &Self) -> Ordering {
        other.cost.partial_cmp(&self.cost)
            .unwrap_or(Ordering::Equal)
    }
}

impl PartialOrd for State {
  fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
    Some(self.cmp(other))
  }
}

impl Graph {
  pub fn hybrid_dijkstra_a_star(&self, start: Node, goal: Node, heuristic: impl Fn(&Node) -> f64) -> Option<(f64, Vec<Node>)> {
    let mut heap = BinaryHeap::new();
    let mut distances = HashMap::new();
    let mut previous = HashMap::new();

    distances.insert(start.clone(), 0.0);
    heap.push(State { cost: 0.0, node: start.clone() });

    while let Some(State { cost, node }) = heap.pop() {
      if node == goal {
        let mut path =  Vec::new();
        let mut current = Some(goal);

        while let Some(node) = current {
          path.push(node.clone());
          current = previous.get(&node).cloned();
        }
        path.reverse();
        return Some((cost, path));
      }

      if cost > *distances.get(&node).unwrap_or(&f64::INFINITY) {
        continue;
      }

      if let Some(neighbors) = self.get_neighbors(&node) {
        for (neighbor, weight) in neighbors {
          let new_cost = cost + weight;
          let heuristic_cost = new_cost + heuristic(neighbor);

          if new_cost < *distances.get(neighbor).unwrap_or(&f64::INFINITY) {
            distances.insert(neighbor.clone(), new_cost);
            previous.insert(neighbor.clone(), node.clone());

            heap.push(State {
              cost: heuristic_cost,
              node: neighbor.clone(),
            });
          }
        }
      }
    }

    None
  }
}
