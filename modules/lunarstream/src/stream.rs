use std::collections::VecDeque;
use std::sync::{Arc, Mutex};

#[derive(Debug, Clone)]
pub struct Message {
    pub id: String,
    pub payload: String,
}

#[derive(Debug, Clone)]
pub struct Stream {
    name: String,
    messages: Arc<Mutex<VecDeque<Message>>>,
}

impl Stream {
    pub fn new(name: &str) -> Self {
        Self {
            name: name.to_string(),
            messages: Arc::new(Mutex::new(VecDeque::new())),
        }
    }

    pub fn add_message(&self, payload: &str) -> String {
        let id = uuid::Uuid::new_v4().to_string();
        let message = Message {
            id: id.clone(),
            payload: payload.to_string(),
        };

        let mut messages = self.messages
            .lock()
            .unwrap();
        messages.push_back(message);

        id
    }

    pub fn get_messages(&self, count: usize) -> Vec<Message> {
        let mut messages = self.messages
            .lock()
            .unwrap();
        messages.drain(..count).collect()
    }
}
