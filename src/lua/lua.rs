use mlua::{Lua, Value, UserData, UserDataMethods};
use std::sync::{Arc, Mutex};
use crate::core::Core;
use crate::core::DataType;
use crate::lua::sandbox::LuaSandbox;
use std::collections::HashMap;
use std::time::Duration;

pub struct LuaEngine {
    core: Arc<Core>,
    sandbox: LuaSandbox,
    script_cache: Mutex<HashMap<String, Vec<u8>>>,
}

struct LunarDBLua {
    core: Arc<Core>,
}

impl UserData for LunarDBLua {
    fn add_methods<M: UserDataMethods<Self>>(methods: &mut M) {
        methods.add_method("get", |lua, this, key: String| {
            match this.core.get(&key) {
                Some(DataType::String(s)) => Ok(Value::String(lua.create_string(&s)?)),
                Some(DataType::List(list)) => {
                    let lua_list = lua.create_table()?;
                    for (i, item) in list.iter().enumerate() {
                        lua_list.set(i + 1, item.clone())?;
                    }
                    Ok(Value::Table(lua_list))
                },
                None => Ok(Value::Nil),
            }
        });

        methods.add_method("set", |lua, this, (key, value): (String, Value)| {
            match value {
                Value::String(s) => {
                    let str_value = s.to_str()?.to_string();
                    this.core.set(key, DataType::String(str_value));
                    Ok(Value::Boolean(true))
                },
                Value::Table(t) => {
                    let mut list = std::collections::VecDeque::new();
                    for i in 1..=t.len()? {
                        if let Ok(item) = t.get::<String>(i) {
                            list.push_back(item);
                        }
                    }
                    this.core.set(key, DataType::List   (list));
                    Ok(Value::Boolean(true))
                },
                _ => Ok(Value::Boolean(false)),
            }
        });

        // Set with expiry
        methods.add_method("setex", |_, this, (key, seconds, value): (String, u64, String)| {
            this.core.set_ex(key, DataType::String(value), seconds);
            Ok(Value::Boolean(true))
        });

        // Delete command
        methods.add_method("del", |_, this, keys: mlua::Table| {
            let mut key_list = Vec::new();
            for pair in keys.pairs::<mlua::Value, String>() {
                if let Ok((_, key)) = pair {
                    key_list.push(key);
                }
            }
            let count = this.core.del(&key_list);
            Ok(Value::Integer(count as i64))
        });

        methods.add_method("lpush", |_, this, (key, values): (String, mlua::Table)| {
            let mut value_list = Vec::new();
            for pair in values.pairs::<mlua::Value, String>() {
                if let Ok((_, value)) = pair {
                    value_list.push(value);
                }
            }
            let count = this.core.lpush(key, value_list);
            Ok(Value::Integer(count as i64))
        });

        methods.add_method("rpush", |_, this, (key, values): (String, mlua::Table)| {
            let mut value_list = Vec::new();
            for pair in values.pairs::<mlua::Value, String>() {
                if let Ok((_, value)) = pair {
                    value_list.push(value);
                }
            }
            let count = this.core.rpush(key, value_list);
            Ok(Value::Integer(count as i64))
        });

        methods.add_method("lpop", |_, this, key: String| {
            match this.core.lpop(&key) {
                Some(value) => Ok(Value::String(methods.lua_ctx().create_string(&value)?)),
                None => Ok(Value::Nil),
            }
        });

        methods.add_method("rpop", |_, this, key: String| {
            match this.core.rpop(&key) {
                Some(value) => Ok(Value::String(methods.lua_ctx().create_string(&value)?)),
                None => Ok(Value::Nil),
            }
        });

        methods.add_method("lrange", |_, this, (key, start, stop): (String, i32, i32)| {
            match this.core.lrange(&key, start, stop) {
                Some(values) => {
                    let lua = methods.lua_ctx();
                    let lua_list = lua.create_table()?;
                    for (i, item) in values.iter().enumerate() {
                        lua_list.set(i + 1, item.clone())?;
                    }
                    Ok(Value::Table(lua_list))
                },
                None => Ok(Value::Nil),
            }
        });

        methods.add_method("llen", |_, this, key: String| {
            let len = this.core.llen(&key);
            Ok(Value::Integer(len as i64))
        });

        methods.add_method("keys", |_, this, pattern: Option<String>| {
            let keys = this.core.keys(pattern.as_deref());
            let lua = methods.lua_ctx();
            let lua_list = lua.create_table()?;
            for (i, key) in keys.iter().enumerate() {
                lua_list.set(i + 1, key.clone())?;
            }
            Ok(Value::Table(lua_list))
        });

        methods.add_method("size", |_, this, _: ()| {
            let size = this.core.size();
            Ok(Value::Integer(size as i64))
        });
    }
}


impl LuaEngine {
    pub fn new(core: Arc<Core>) -> Self {
        Self {
            core,
            sandbox: LuaSandbox::new(),
            script_cache: Mutex::new(HashMap::new()),
        }
    }

    pub fn with_limits(core: Arc<Core>, max_execution_time: Duration, max_memory: usize) -> Self {
        Self {
            core,
            sandbox: LuaSandbox::with_limits(max_execution_time, max_memory),
            script_cache: Mutex::new(HashMap::new()),
        }
    }

    pub fn eval(&self, script: &str, keys: Vec<String>, args: Vec<String>) -> Result<String, String> {
        let lua = Lua::new();

        let globals = lua.globals();
        let db = LunarDBLua { core: Arc::clone(&self.core) };
        if let Err(e) = globals.set("db", db) {
            return Err(format!("Failed to set db global: {}", e));
        }

        let keys_table = match lua.create_table() {
            Ok(t) => t,
            Err(e) => return Err(format!("Failed to create KEYS table: {}", e)),
        };

        for (i, key) in keys.iter().enumerate() {
            if let Err(e) = keys_table.set(i + 1, key.clone()) {
                return Err(format!("Failed to set key in KEYS table: {}", e));
            }
        }

        let args_table = match lua.create_table() {
            Ok(t) => t,
            Err(e) => return Err(format!("Failed to create ARGV table: {}", e)),
        };

        for (i, arg) in args.iter().enumerate() {
            if let Err(e) = args_table.set(i + 1, arg.clone()) {
                return Err(format!("Failed to set arg in ARGV table: {}", e));
            }
        }

        if let Err(e) = globals.set("KEYS", keys_table) {
            return Err(format!("Failed to set KEYS global: {}", e));
        }

        if let Err(e) = globals.set("ARGV", args_table) {
            return Err(format!("Failed to set ARGV global: {}", e));
        }

        match lua.load(script).eval::<mlua::Value>() {
            Ok(result) => match self.lua_value_to_string(result) {
                Ok(s) => Ok(s),
                Err(e) => Err(format!("Failed to convert result: {}", e)),
            },
            Err(e) => Err(format!("Script execution error: {}", e)),
        }
    }

    pub fn evalsha(&self, sha1: &str, keys: Vec<String>, args: Vec<String>) -> Result<String, String> {
        let script_bytes = {
            let cache = self.script_cache.lock().unwrap();
            match cache.get(sha1) {
                Some(bytes) => bytes.clone(),
                None => return Err(format!("Script not found in cache: {}", sha1)),
            }
        };

        let script = match String::from_utf8(script_bytes) {
            Ok(s) => s,
            Err(e) => return Err(format!("Failed to decode script: {}", e)),
        };

        self.eval(&script, keys, args)
    }

    pub fn script_load(&self, script: &str) -> String {
        use sha2::{Sha256, Digest};
        let mut hasher = Sha256::new();
        hasher.update(script.as_bytes());
        let hash = format!("{:x}", hasher.finalize());
        let mut cache = self.script_cache.lock().unwrap();
        cache.insert(hash.clone(), script.as_bytes().to_vec());
        hash
    }

    pub fn script_exists(&self, sha1: &str) -> bool {
        let cache = self.script_cache.lock().unwrap();
        cache.contains_key(sha1)
    }

    pub fn script_flush(&self) {
        let mut cache = self.script_cache.lock().unwrap();
        cache.clear();
    }

    fn lua_value_to_string(&self, value: mlua::Value) -> Result<String, String> {
        match value {
            Value::Nil => Ok("nil".to_string()),
            Value::Boolean(b) => Ok(b.to_string()),
            Value::Integer(i) => Ok(i.to_string()),
            Value::Number(n) => Ok(n.to_string()),
            Value::String(s) => match s.to_str() {
                Ok(s) => Ok(s.to_string()),
                Err(e) => Err(format!("Failed to convert string: {}", e)),
            },
            Value::Table(t) => {
                let mut result = String::from("[");
                let mut first = true;

                let len = match t.len() {
                    Ok(len) => len,
                    Err(e) => return Err(format!("Failed to get table length: {}", e)),
                };

                for i in 1..=len {
                    if let Ok(value) = t.get::<_, mlua::Value>(mlua::Value::Integer(i)) {
                        if !first {
                            result.push_str(", ");
                        }
                        match self.lua_value_to_string(value) {
                            Ok(s) => result.push_str(&s),
                            Err(e) => return Err(e),
                        }
                        first = false;
                    }
                }

                result.push(']');
                Ok(result)
            }
            _ => Err("Unsupported value type".to_string()),
        }
    }
}
