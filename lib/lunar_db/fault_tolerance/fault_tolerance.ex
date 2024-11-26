defmodule LunarDB.FaultTolerance do
  @moduledoc """
  Provides fault tolerance mechanisms for LunarDB cache operations.
  Implements circuit breakers, retries, and fallback strategies.
  """

  use GenServer
  require Logger

  # Circuit breaker configuration
  @default_threshold 5
  @reset_timeout :timer.seconds(30)

  defstruct [
    state: :closed,
    failure_count: 0,
    threshold: @default_threshold,
    last_failure: nil
  ]

  # Client API
  def start_link(opts \\ []) do
    GenServer.start_link(__MODULE__, opts, name: __MODULE__)
  end

  def execute(operation) do
    GenServer.call(__MODULE__, {:execute, operation})
  end

  # Server callbacks
  def init(opts) do
    threshold = Keyword.get(opts, :threshold, @default_threshold)
    {:ok, %__MODULE__{threshold: threshold}}
  end

  def handle_call({:execute, operation}, _from, state) do
    case state.state do
      :closed ->
        try do
          result = operation.()
          {:reply, {:ok, result}, state}
        catch
          error ->
            new_state = handle_failure(state)
            Logger.warn("Operation failed: #{inspect(error)}")
            {:reply, {:error, :operation_failed}, new_state}
        end

      :open ->
        if should_reset?(state) do
          {:reply, execute_with_retry(operation), reset_circuit(state)}
        else
          {:reply, {:error, :circuit_open}, state}
        end
    end
  end

  # Private functions
  defp handle_failure(state) do
    new_count = state.failure_count + 1

    if new_count >= state.threshold do
      %{state |
        state: :open,
        failure_count: new_count,
        last_failure: System.monotonic_time(:millisecond)
      }
    else
      %{state | failure_count: new_count}
    end
  end

  defp should_reset?(state) do
    elapsed = System.monotonic_time(:millisecond) - state.last_failure
    elapsed >= @reset_timeout
  end

  defp reset_circuit(state) do
    %{state |
      state: :closed,
      failure_count: 0,
      last_failure: nil
    }
  end

  defp execute_with_retry(operation) do
    retry_opts = [
      delay: 100,
      max_retries: 3,
      exponential_backoff: true
    ]

    case RetryHelper.retry_with_backoff(operation, retry_opts) do
      {:ok, result} -> {:ok, result}
      {:error, reason} -> {:error, reason}
    end
  end
end

defmodule RetryHelper do
  @moduledoc """
  Helper module for implementing retry logic with exponential backoff
  """

  def retry_with_backoff(operation, opts) do
    do_retry(operation, 0, opts)
  end

  defp do_retry(_operation, attempts, %{max_retries: max}) when attempts >= max do
    {:error, :max_retries_exceeded}
  end

  defp do_retry(operation, attempts, opts) do
    try do
      {:ok, operation.()}
    catch
      _error ->
        :timer.sleep(calculate_backoff(attempts, opts))
        do_retry(operation, attempts + 1, opts)
    end
  end

  defp calculate_backoff(attempt, %{delay: delay, exponential_backoff: true}) do
    trunc(delay * :math.pow(2, attempt))
  end
end
