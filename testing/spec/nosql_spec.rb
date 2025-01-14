require 'httparty'
require 'rspec'

RSpec.describe 'NoSQL Tests' do
  let(:base_url) {"http://127.0.0.1:3000/api/nosql/"}

  it 'sets and retrieves a key-value pair' do
    # Set
    set_response = HTTParty.post("#{base_url}set", body: { key: 'foo', value: 'bar' })
    expect(set_response.code).to eq(200)
    expect(set_response.body).to eq('OK')

    # Get
    get_response = HTTParty.get("#{base_url}get", query: { key: 'foo' })
    expect(get_response.code).to eq(200)
    expect(get_response.body).to eq('bar')
  end

  it 'handles non-existent keys' do
    response = HTTParty.get("#{base_url}get", query: { key: 'unknown' })
    expect(response.code).to eq(404)
    expect(response.body).to eq('ERR')
  end

  it 'deletes a key successfully' do
    # Sets the key
    HTTParty.post("#{base_url}set", body: { key: 'temp', value: '123' })

    # Deletes th key
    del_response = HTTParty.post("#{base_url}del", body: { key: 'temp' })
    expect(del_response.code).to eq(200)
    expect(del_response.body).to eq('OK')

    # Verify the deletion
    get_response = HTTParty.get("#{base_url}get", query: { key: 'temp' })
    expect(get_response.code).to eq(404)
    expect(get_response.body).to eq('ERR')
  end
end
