# AUTHORS KAZOOKI12, STARLOEXOLIZ

defmodule LunarDB.Middleware.ApiGateway do
  use Plug.Router

  plug Plug.Parsers,
    parsers: [:json],
    pass: ["application/json"],
    json_decoder: Jason

  plug :match
  plug :dispatch

  @go_api_url "http://localhost:8080"

  plug :log_request

  # Health check endpoint
  get "/health" do
    send_resp(conn, 200, "OK")
  end

  # Query endpoint
  post "/query" do
    forward_to_go_api(conn)
  end

  # Schema endpoint
  post "/schema" do
    forward_to_go_api(conn)
  end

  # Catch-all route
  match _ do
    send_resp(conn, 404, "Not found")
  end

  defp log_request(conn, _opts) do
    IO.puts("Received request: #{conn.method} #{conn.request_path}")
    conn
  end

  defp forward_to_go_api(conn) do
    go_api_path = "#{@go_api_url}#{conn.request_path}"

    # Forward the request body and headers
    headers = [
      {"content-type", "application/json"},
      {"accept", "application/json"}
    ]

    case HTTPoison.post(go_api_path, conn.body_params |> Jason.encode!(), headers) do
      {:ok, response} ->
        # Forward the response back to the client
        conn
        |> put_resp_content_type("application/json")
        |> send_resp(response.status_code, response.body)

      {:error, %HTTPoison.Error{reason: reason}} ->
        # Handle errors
        conn
        |> put_resp_content_type("application/json")
        |> send_resp(500, Jason.encode!(%{error: "Internal server error: #{reason}"}))
    end
  end
end
