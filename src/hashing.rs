use sha2::{Sha256, Digest};
use std::hash::{Hash, Hasher};
use std::collections::hash_map::DefaultHasher;

// SHA256 Algorithm
pub fn hash_sha256(input: &str) -> String {
  let mut hasher = Sha256::new();
  hasher.update(input.as_bytes());
  let result = hasher.finalize();

  format!("{:x}", result)
}

// MURMUR3 Algorithm
pub fn hash_murmur3(input: &str) -> String {
  let mut hasher = DefaultHasher::new();
  input.hash(&mut hasher);
  let result = hasher.finish();

  format!("{:x}", result)
}

// Rotate Algorithm
pub fn rotate_hash(input: &str, shift: u32) -> String {
  let mut hasher = DefaultHasher::new();
  input.hash(&mut hasher);
  let hash = hasher.finish();

  let mut result = String::new();
  for i in 0..shift {
    result.push_str(&format!("{:x}", hash >> (i * 8)));
  }
  result.push_str(&format!("{:x}", hash));

  result
}
