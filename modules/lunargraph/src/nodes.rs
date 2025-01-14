#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub struct Node {
  pub id: usize,
  pub label: String,
}

impl Node {
  pub fn new(id: usize, label: String) -> Self {
    Node { id, label }
  }
}
