# Author: Kazooki123, StarloExoliz
# HEALTH CHECKER SCRIPT FOR MONITORING

defmodule LunarDB.Monitor.HealthCheck
do
  use GenServer

  @lunardb_cli "../../../../bin/lunar"

  def start_link(_) do
    GenServer.start_link(__MODULE__, %{}, name: __MODULE__)
  end

  def init(state) do
    schedule_health_check()
    {:ok, state}
  end

  def run_health_check do
    GenServer.cast(__MODULE__, :check_health)
  end

  def handle_cast(:check_health, state) do
    check_lunardb_health()
    schedule_health_check()
    {:noreply, state}
  end

  defp check_lunardb_health do
    case System.cmd(@lunardb_cli, ["--status"]) do
      {"healthy", 0} -> IO.puts("LunarDB is healthy")
      {error_message, _} -> IO.puts("Error: #{error_message}")
    end
  end

  defp schedule_health_check do
    Process.send_after(self(), :check_health, 5_000)
  end

  def handle_info(:check_health, state) do
    check_lunardb_health()
    schedule_health_check()
    {:noreply, state}
  end
end
