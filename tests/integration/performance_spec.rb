require 'httparty'
require 'rspec'

RSpec.describe "Performance Tests" do
  let(:base_url) { 'http://127.0.0.1:3000/api/nosql' }

  it 'handles a high volume of requests' do
    threads = []
    100.times do |i|
      threads << Thread.new do
        HTTParty.post("#{base_url}set", body: { key: "key_#{i}", value: "value_#{i}" })
      end
    end
    threads.each(&:join)

    response = HTTParty.get("#{base_url}keys")
    expect(response.code).to eq(200)
    keys = JSON.parse(response.body)
    expect(keys.size).to eq(100)
  end
end
