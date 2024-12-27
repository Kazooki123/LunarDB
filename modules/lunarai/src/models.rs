use core::{option::Option::{None, Some}, result::Result::Ok};
use std::{path::PathBuf, sync::Arc};
use safetensors::SafeTensors;
use tokenizers::Tokenizer;
use anyhow::{Result, Context};
use tch::{Device, Tensor};
use serde::Deserialize;

#[derive(Debug, Deserialize)]
pub struct GPT2Config {
    pub vocab_size: usize,
    pub n_positions: usize,
    pub n_embd: usize,
    pub n_layer: usize,
    pub n_head: usize,
}

#[derive(Debug)]
pub struct GPT2Model {
    tokenizer: Tokenizer,
    model_data: Arc<[u8]>,
    model: SafeTensors<'static>,
    config: GPT2Config,
    device: Device,
}

impl GPT2Model {
    pub fn load_from_path(base_path: PathBuf) -> Result<Self> {
        // Load model files
        let model_path = base_path.join("model.safetensors");
        let config_path = base_path.join("config.json");
        let tokenizer_path = base_path.join("tokenizer.json");

        // Load safetensors model and convert directly to Arc<[u8]>
        let model_data: Arc<[u8]> = Arc::from(std::fs::read(&model_path)
            .context("Failed to read model file")?);

        // Create a static reference from the Arc for SafeTensors
        let model_ref: &'static [u8] = Box::leak(Box::from(&*model_data));
        let model = SafeTensors::deserialize(model_ref)
            .context("Failed to deserialize model")?;

        // Load config
        let config_str = std::fs::read_to_string(&config_path)
            .context("Failed to read config file")?;
        let config: GPT2Config = serde_json::from_str(&config_str)
            .context("Failed to parse config")?;

        // Load tokenizer
        let tokenizer = Tokenizer::from_file(tokenizer_path)
            .map_err(|e| anyhow::anyhow!(e))
            .context("Failed to load tokenizer")?;

        // Detect device (CUDA if available)
        let device = if tch::Cuda::is_available() {
            Device::Cuda(0)
        } else {
            Device::Cpu
        };

        Ok(Self {
            tokenizer,
            model_data,
            model,
            config,
            device,
        })
    }

    pub fn encode_text(&self, text: &str) -> Result<Tensor> {
        let encoding = self.tokenizer.encode(text, true)
            .map_err(|e| anyhow::anyhow!(e))
            .context("Failed to encode text")?;

        let input_ids: Vec<i64> = encoding.get_ids()
            .iter()
            .map(|&id| id as i64)
            .collect();

        let tensor = Tensor::from_slice(&input_ids)
            .to_device(self.device);

        Ok(tensor)
    }

    pub fn get_embeddings(&self, input_tensor: &Tensor) -> Result<Tensor> {
        // Get word embeddings matrix from safetensors
        let wte_tensor = self.model
            .tensor("transformer.wte.weight")
            .context("Failed to get word embeddings")?;

        // Convert to tch::Tensor
        let wte = Tensor::from_slice(wte_tensor.data())
            .reshape(&[self.config.vocab_size as i64, self.config.n_embd as i64])
            .to_device(self.device);

        // Get embeddings for input
        let embeddings = wte.index_select(0, input_tensor);

        Ok(embeddings)
    }
}

#[derive(Debug)]
pub struct ModelManager {
    gpt2_model: Option<GPT2Model>,
    model_path: PathBuf,
}

impl ModelManager {
    pub fn new(model_path: PathBuf) -> Self {
        Self {
            gpt2_model: None,
            model_path,
        }
    }

    pub fn load_gpt2(&mut self) -> Result<()> {
        self.gpt2_model = Some(GPT2Model::load_from_path(self.model_path.clone())?);
        Ok(())
    }

    pub fn get_embeddings(&self, text: &str) -> Result<Tensor> {
        let model = self.gpt2_model.as_ref()
            .context("GPT-2 model not loaded")?;

        let input_tensor = model.encode_text(text)?;
        model.get_embeddings(&input_tensor)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_model_loading() {
        let path = PathBuf::from(r"D:\huggingface_cache\models--gpt2\snapshots\607a30d783dfa663caf39e06633721c8d4cfcd7e");
        let mut manager = ModelManager::new(path);
        assert!(manager.load_gpt2().is_ok());
    }

    #[test]
    fn test_embedding_generation() {
        let path = PathBuf::from(r"D:\huggingface_cache\models--gpt2\snapshots\607a30d783dfa663caf39e06633721c8d4cfcd7e");
        let mut manager = ModelManager::new(path);
        manager.load_gpt2().unwrap();

        let result = manager.get_embeddings("Hello, world!");
        assert!(result.is_ok());
    }
}
