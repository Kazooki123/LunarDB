use std::collections::HashMap;

pub struct Document {
  pub id: String,
  pub content: String,
}

pub struct Index {
  pub inverted_index: HashMap<String, Vec<String>>,
}

impl Index {
  pub fn new() -> Self {
    Self {
      inverted_index: HashMap::new(),
    }
  }

  pub fn add_document(&mut self, doc: &Document) {
    let terms = self.tokenize(&doc.content);
    for term in terms {
      self.inverted_index.entry(term).or_default().push(doc.id.clone());
    }
  }

  pub fn search(&self, query: &str ) -> Vec<String> {
    let terms = self.tokenize(query);
    let mut result: Vec<String> = Vec::new();

    for term in terms {
      if let Some(doc_ids) = self.inverted_index.get(&term) {
        result.extend(doc_ids.clone());
      }
    }

    result.sort();
    result.dedup();
    result
  }

  fn tokenize(&self, text: &str) -> Vec<String> {
    text.split_whitespace()
      .map(|s| s.to_lowercase())
      .map(|s| s.trim_matches(|c: char| !c.is_alphanumeric()).to_string())
    .filter(|s| !s.is_empty())
      .collect()
  }
}

#[cfg(test)]
mod tests {
  use super::*;

  #[test]
  fn test_indexing_and_searching() {
    let mut index = Index::new();

    let doc1 = Document {
      id: "1".to_string(),
      content: "Students should all be equal.".to_string(),
    };

    let doc2 = Document {
      id: "2".to_string(),
      content: "I finally have a new friend in my town.".to_string(),
    };

    index.add_document(&doc1);
    index.add_document(&doc2);

    assert_eq!(index.search("equal"), vec!["1"]);
    assert_eq!(index.search("friend"), vec!["2"]);
    assert_eq!(index.search("town"), vec!["2"]);
  }
}
