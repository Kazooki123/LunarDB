defmodule LunarDB.MixProject do
  use Mix.Project

  def project do
    [
      app: :lunar_db,
      version: "0.1.0",
      elixir: "~> 1.17",
      start_permanent: Mix.env() == :prod,
      deps: deps()
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger],
      mod: {LunarDB.Application, []}
    ]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      {:plug_cowboy, "~> 2.5"},
      {:httpoison, "~> 1.8"},
      #{:ecto, "~> 3.12.4"},
      #{:phoenix, "~> 1.7"},
      #{:libcluster, "~> 3.4"},
      {:jason, "~> 1.4"}
    ]
  end
end
