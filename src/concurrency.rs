use std::thread;
use std::thread::JoinHandle;

pub fn spawn_councurent<F>(f: F) -> JoinHandle<()>
where
    F: FnOnce() + Send + 'static,
{
    thread::spawn(f)
}