require 'httparty'
require 'rspec'

RSpec.describe 'SQL Query Tests' do
  let(:base_url) {"http://127.0.0.1:3000/api/sql/"}

  it 'executes a valid SQL Query' do
    response = HTTParty.post("#{base_url}query", body: { query: 'SELECT 1' })
    expect(response.code).to eq(200)
    expect(response.body).to include('1')
  end

  it 'handles invalid SQL' do
    response = HTTParty.post("#{base_url}query", body: { query: 'INVALID SQL' })
    expect(response.code).to eq(400)
    expect(response.body).to include('ERR')
  end
end
