defmodule LunarDB.Application do
  @moduledoc false

  use Application

  def start(_type, _args) do
    children = [
      LunarDB.Monitor.HealthCheck,
      LunarDB.Middleware.ApiGateway,
    ]

    opts = [strategy: :one_for_one, name: LunarDB.Supervisor]
    Supervisor.start_link(children, opts)
  end
end
