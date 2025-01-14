# LunarDB Server host in your machine

While in BETA, LunarDB has now a self-host server support which you
can launch in your machine.

## Command

**The command for the LunarDB server host is:**

```bash
lunardb-server --host 127.0.0.1 --port 6379 --config '../src/servers/lunardb.json'
```

And it **should** show you a text in your _terminal_ like this:

```bash
Running locally...
LunarDB server has been launched!

IP: <YOURIP>
PORT: <YOURPORT>
```

## What does it do?

What this **command** do is to host lunardb in your machine WHIC THEN
you can connect to it by opening a new terminal then launch `lunar.exe`
then type this:

```bash
> CONNECT <ip:port of your lunardb host>
Successfully connected!
```

**But if it fails:**

```bash
> CONNECT <ip:port>
Error! <error>
```

## Note

This is still in development meaning new updated and patches will be added up sooner on.

Thank you.

-StarloExoliz, Kazooki123
