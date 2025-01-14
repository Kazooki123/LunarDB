#[derive(Debug)]
pub enum LunarStreamError {
    StreamNotFound,
    ConsumerGroupNotFound,
    ConsumerNotFound,
}

impl std::fmt::Display for LunarStreamError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::StreamNotFound => write!(f, "Stream Not Found"),
            Self::ConsumerGroupNotFound => write!(f, "Consumer Group Not Found"),
            Self::ConsumerNotFound => write!(f, "Consumer Not Found"),
        }
    }
}

impl std::error::Error for LunarStreamError {}