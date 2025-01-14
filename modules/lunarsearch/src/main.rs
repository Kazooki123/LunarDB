mod core;
mod search;

use core::Document;
use search::SearchEngine;

fn main() {
  let search_engine = SearchEngine::new();

  let doc1 = Document {
    id: "1".to_string(),
    content: "The house is in fire.".to_string(),
  };

  let doc2 = Document {
    id: "2".to_string(),
    content: "That bunny is so fast.".to_string(),
  };

  search_engine.add_document(doc1);
  search_engine.add_document(doc2);

  println!("Search Results for 'house': {:?}", search_engine.search("house"));
  println!("Search Results for 'fast': {:?}", search_engine.search("fast"));

  println!("Search Results for 'awesome': {:?}", search_engine.search("awesome"));
}
