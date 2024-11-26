defmodule LunarDB.Cluster.Clustering do
  use GenServer

  @moduledoc """
  This module handles the clustering logic for LunarDB.
  """

  @doc """
  Starts the clustering process.
  """
  def start_link(opts) do
    GenServer.start_link(__MODULE__, opts, name: __MODULE__)
  end

  @impl true
  def init(_opts) do
    {:ok, %{}}
  end

  @doc """
  Adds a node to the cluster.
  """
  def add_node(node) do
    GenServer.call(__MODULE__, {:add_node, node})
  end

  @doc """
  Removes a node from the cluster.
  """
  def remove_node(node) do
    GenServer.call(__MODULE__, {:remove_node, node})
  end

  @doc """
  Gets the current cluster state.
  """
  def get_cluster_state do
    GenServer.call(__MODULE__, :get_cluster_state)
  end

  @impl true
  def handle_call({:add_node, node}, _from, state) do
    new_state = Map.put(state, node, %{})
    {:reply, :ok, new_state}
  end

  @impl true
  def handle_call({:remove_node, node}, _from, state) do
    new_state = Map.delete(state, node)
    {:reply, :ok, new_state}
  end

  @impl true
  def handle_call(:get_cluster_state, _from, state) do
    {:reply, state, state}
  end
end
