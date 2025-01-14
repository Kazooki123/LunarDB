use crate::core::{Document, Index};
use std::sync::{Arc, Mutex};

pub struct SearchEngine {
  pub index: Arc<Mutex<Index>>,
}

impl SearchEngine {
  pub fn new() -> Self {
    Self {
      index: Arc::new(Mutex::new(Index::new())),
    }
  }

  pub fn add_document(&self, doc: Document) {
    let mut index = self.index.lock().unwrap();
    index.add_document(&doc);
  }

  pub fn search(&self, query: &str) -> Vec<String> {
    let index = self.index.lock().unwrap();
    index.search(query)
  }
}

#[cfg(test)]
mod tests {
  use super::*;

  #[test]
  fn test_search_engine() {
    let engine = SearchEngine::new();

    let doc1 = Document {
      id: "1".to_string(),
      content: "The quick brown fox.".to_string(),
    };

    let doc2 = Document {
      id: "2".to_string(),
      content: "Jump over the lazy dog.".to_string(),
    };

    engine.add_document(doc1);
    engine.add_document(doc2);

    assert_eq!(engine.search("quick"), vec!["1"]);
    assert_eq!(engine.search("dog"), vec!["2"]);
    assert!(engine.search("cat").is_empty());

  }
}
