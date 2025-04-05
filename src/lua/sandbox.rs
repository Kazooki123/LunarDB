use mlua::{Lua, Result as LuaResult, StdLib, LuaOptions};
use std::time::Duration;

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
        let lua = unsafe {
            Lua::unsafe_new_with(
                StdLib::TABLE | StdLib::STRING | StdLib::MATH,
                LuaOptions::default(),
            )
        };

        let globals = lua.globals();
        globals.set("dofile", lua.create_table())?;
        globals.set("loadfile", lua.create_table())?;
        globals.set("load", lua.create_table())?;
        globals.set("os", lua.create_table())?;
        globals.set("io", lua.create_table())?;
        globals.set("package", lua.create_table())?;
        globals.set("require", lua.create_table())?;

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

        lua.set_hook(
            mlua::HookTriggers::new().every_line(),
            move |_, _| {
                if std::thread::current().name().unwrap_or("") == "lua_timer" {
                    Ok(())
                } else {
                    Err(mlua::Error::RuntimeError(
                        "Script execution time exceeded".to_string(),
                    ))
                }
            },
        )?;

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
