use rust_stemmers::{Algorithm, Stemmer};
use stop_words::{get, LANGUAGE::English};
use unicode_segmentation::UnicodeSegmentation;
use std::collections::HashSet;
use crate::LunarAIError;

pub struct Tokenizer {
    stemmer: Stemmer,
    stop_words: HashSet<String>,
    vocab: HashSet<String>,
    max_tokens: usize,
}

impl Tokenizer {
    pub fn new() -> Self {
        Self {
            stemmer: Stemmer::create(Algorithm::English),
            stop_words: get(English).iter().map(|s| s.to_string()).collect(),
            vocab: HashSet::new(),
            max_tokens: 512,
        }
    }

    pub fn with_max_tokens(max_tokens: usize) -> Self {
        let mut tokenizer = Self::new();
        tokenizer.max_tokens = max_tokens;
        tokenizer
    }

    pub fn tokenize(&self, text: &str) -> Result<Vec<String>, LunarAIError> {
        let normalized = self.normalize(text);
        let words: Vec<String> = normalized
            .unicode_words()
            .filter(|word| !self.stop_words.contains(*word))
            .map(|word| self.stemmer.stem(word).to_string())
            .take(self.max_tokens)
            .collect();

        if words.is_empty() {
            return Err(LunarAIError::EmbeddingError("No valid tokens found".to_string()));
        }

        Ok(words)
    }

    pub fn tokenize_with_positions(&self, text: &str) -> Result<Vec<(String, usize)>, LunarAIError> {
        let normalized = self.normalize(text);
        let tokens_with_positions: Vec<(String, usize)> = normalized
            .unicode_words()
            .enumerate()
            .filter(|(_, word)| !self.stop_words.contains(*word))
            .map(|(pos, word)| (self.stemmer.stem(word).to_string(), pos))
            .take(self.max_tokens)
            .collect();

        if tokens_with_positions.is_empty() {
            return Err(LunarAIError::EmbeddingError("No valid tokens found".to_string()));
        }

        Ok(tokens_with_positions)
    }

    pub fn add_to_vocab(&mut self, terms: &[String]) {
        self.vocab.extend(terms.iter().cloned());
    }

    pub fn is_in_vocab(&self, term: &str) -> bool {
        self.vocab.contains(term)
    }

    pub fn vocab_size(&self) -> usize {
        self.vocab.len()
    }

    fn normalize(&self, text: &str) -> String {
        text.to_lowercase()
            .replace(|c: char| !c.is_alphanumeric() && !c.is_whitespace(), " ")
            .replace("  ", " ")
            .trim()
            .to_string()
    }

    pub fn get_ngrams(&self, text: &str, n: usize) -> Result<Vec<String>, LunarAIError> {
        if n == 0 {
            return Err(LunarAIError::InvalidDimensions("N-gram size must be > 0".to_string()));
        }

        let tokens = self.tokenize(text)?;
        let ngrams: Vec<String> = tokens
            .windows(n)
            .map(|window| window.join(" "))
            .collect();

        Ok(ngrams)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_basic_tokenization() {
        let tokenizer = Tokenizer::new();
        let tokens = tokenizer.tokenize("Hello World! This is a test.").unwrap();
        assert!(tokens.len() > 0);
        assert!(!tokens.contains(&"is".to_string())); // Stop word should be removed
    }

    #[test]
    fn test_ngrams() {
        let tokenizer = Tokenizer::new();
        let ngrams = tokenizer.get_ngrams("hello world test", 2).unwrap();
        assert!(ngrams.len() > 0);
    }
}
