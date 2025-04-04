use mlua::{Lua, Result as LuaResult, Table, Function, Value, StdLib, LuaOptions};
use std::time::Duration;
use std::sync::Arc;

pub struct LuaSandbox {
    max_execution_time: Duration,
    max_memory: usize,
}

impl LuaSandbox {
    pub fn new() -> Self {
        Self {
            max_execution_time: Duration::from_secs(5),
            max_memory: 1024 * 1024 * 10, // 10MB
        }
    }

    pub fn with_limits(max_execution_time: Duration, max_memory: usize) -> Self {
        Self {
            max_execution_time,
            max_memory,
        }
    }

    pub fn create_env() -> LuaResult<Lua> {
        let lua = Lua::unsafe_new_with(
            StdLib::TABLE | StdLib::STRING | StdLib::MATH,
            LuaOptions::default(),
        );

        let globals = lua.globals();
        globals.set("dofile", lua.create_function(|_, _: ()| Ok(()))?)?;
        globals.set("loadfile", lua.create_function(|_, _: ()| Ok(()))?)?;
        globals.set("load", lua.create_function(|_, _: ()| Ok(()))?)?;
        globals.set("os", lua.create_table()?)?;
        globals.set("io", lua.create_table()?)?;
        globals.set("package", lua.create_table()?)?;
        globals.set("require", lua.create_function(|_, _: ()| Ok(()))?)?;

        Ok(lua)
    }

    fn restrict_environment(&self, lua: &Lua) -> LuaResult<()> {
        let globals = lua.globals();

        globals.set("dofile", lua.null())?;
        globals.set("loadfile", lua.null())?;
        globals.set("load", lua.null())?;

        globals.set("os", lua.null())?;
        globals.set("io", lua.null())?;

        globals.set("package", lua.null())?;
        globals.set("require", lua.null())?;

        Ok(())
    }

    fn setup_memory_limit(&self, lua: &Lua) -> LuaResult<()> {
        let max_memory = self.max_memory;
        lua.set_memory_limit(max_memory)?;

        Ok(())
    }

    fn setup_time_limit(&self, lua: &Lua) -> LuaResult<()> {
        let max_time = self.max_execution_time;

        lua.set_hook(move |_, _| {
            if std::thread::current().name().unwrap_or("") == "lua_timer" {
                return Ok(());
            }

            Err(mlua::Error::RuntimeError("Script execution time exceeded".to_string()))
        }, mlua::HookTriggers::every(1000))?;

        Ok(())
    }

    pub fn execute(&self, script: &str) -> LuaResult<mlua::Value> {
        let lua = LuaSandbox::create_env()?;
        let result = lua.load(script).eval();
        result
    }
}

impl Default for LuaSandbox {
    fn default() -> Self {
        Self::new()
    }
}
