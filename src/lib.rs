mod cache;
mod core;
mod save;
mod hashing;
mod module;
mod sharding;
mod sql;
mod connect;
mod lua;
mod handlers;

pub use cache::Cache;
pub use core::{Core, DataType};
pub use lua::{LuaEngine, LuaSandbox};
use crate::save::{save_to_file, load_from_file};

use std::sync::Arc;
use std::time::Duration;

pub struct LunarDB {
    core: Arc<Core>,
    lua_engine: LuaEngine,
}

impl LunarDB {
    pub fn new() -> Self {
        let core = Arc::new(Core::new());
        let lua_engine = LuaEngine::new(Arc::clone(&core));

        LunarDB {
            core,
            lua_engine,
        }
    }

    pub fn save(&self, file_name: &str) -> Result<(), String> {
        let data = self.core().serialize();
        save_to_file(&data, file_name)
    }

    pub fn load(&mut self, file_name: &str) -> Result<(), String> {
        let data = load_from_file(file_name)?;
        self.core_mut().deserialize(&data);
        Ok(())
    }

    pub fn start_tcp_server(&self, port: u16) -> Result<(), String> {
        use crate::handlers::api::tcp::TcpHandler;

        let handler = TcpHandler::new(Arc::clone(&self.core), port);
        handler.start()
    }

    pub fn with_lua_limits(max_execution_time: Duration, max_memory: usize) -> Self {
        let core = Arc::new(Core::new());
        let lua_engine = LuaEngine::with_limits(
            Arc::clone(&core),
            max_execution_time,
            max_memory,
        );

        LunarDB {
            core,
            lua_engine,
        }
    }

    pub fn core(&self) -> &Core {
        &self.core
    }

    pub fn set(&self, key: String, value: String) {
        self.core.set(key, DataType::String(value));
    }

    pub fn get(&self, key: &str) -> Option<String> {
        match self.core.get(key) {
            Some(DataType::String(s)) => Some(s),
            _ => None
        }
    }

    pub fn del(&self, keys: &[String]) -> usize {
        self.core.del(keys)
    }

    pub fn lpush(&self, key: String, values: Vec<String>) -> usize {
        self.core.lpush(key, values)
    }

    pub fn rpush(&self, key: String, values: Vec<String>) -> usize {
        self.core.rpush(key, values)
    }

    pub fn lrange(&self, key: &str, start: i32, stop: i32) -> Option<Vec<String>> {
        self.core.lrange(key, start, stop)
    }

    pub fn llen(&self, key: &str) -> usize {
        self.core.llen(key)
    }

    pub fn keys(&self, pattern: Option<&str>) -> Vec<String> {
        self.core.keys(pattern)
    }

    pub fn size(&self) -> usize {
        self.core.size()
    }

    pub fn cleanup(&self) -> usize {
        self.core.cleanup()
    }

    pub fn core_mut(&mut self) -> &mut Core {
        Arc::get_mut(&mut self.core).unwrap()
    }

    pub fn eval(&self, script: &str, keys: Vec<String>, args: Vec<String>) -> Result<String, String> {
        self.lua_engine.eval(script, keys, args)
    }

    pub fn evalsha(&self, sha1: &str, keys: Vec<String>, args: Vec<String>) -> Result<String, String> {
        self.lua_engine.evalsha(sha1, keys, args)
    }

    pub fn script_load(&self, script: &str) -> String {
        self.lua_engine.script_load(script)
    }

    pub fn script_exists(&self, sha1: &str) -> bool {
        self.lua_engine.script_exists(sha1)
    }

    pub fn script_flush(&self) {
        self.lua_engine.script_flush()
    }
}

impl Default for LunarDB {
    fn default() -> Self {
        Self::new()
    }
}
