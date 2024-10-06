use std::collections::HashMap;
use std::error::Error;
use tch::{nn, Device, Tensor};

pub struct ModelManager {
    models: HashMap<String, nn::Module>,
}

impl ModelManager {
    pub fn new() -> Result<Self, Box<dyn Error>> {
        Ok(Self {
            models: HashMap::new(),
        })
    }

    pub fn load_model(&mut self, name: &str, path: &str) -> Result<(), Box<dyn Error>> {
        let model = nn::Module::load(path, Device::Cpu)?;
        self.models.insert(name.to_string(), model);
        Ok(())
    }

    pub fn run_model(&self, name: &str, input: &[f32]) -> Result<Vec<f32>, Box<dyn Error>> {
        let model = self.models.get(name).ok_or("Model not found")?;
        let input_tensor = Tensor::from_slice(input).view([-1, input.len() as i64]);
        let output = model.forward(&input_tensor);
        let result: Vec<f32> = Vec::from(output.flatten(0, -1).into());
        Ok(result)
    }
}
