// WASMQL Query Contract 

struct Instrument {
    Instrument(instrument inst) : _data{std::move(inst)} {}
    b1::query::String ticker() const { 
        return get<b1::query::String>([](auto& record){ return record.ticker; });
    }
    b1::query::String name() const { 
        return get<b1::query::String>([](auto& record){ return record.name; });
    }
    uint8_t nativePrecision() const { 
        return get<uint8_t>([](auto& record){ return record.native_precision; });
    }
    uint8_t custodyPrecision() const { 
        return get<uint8_t>([](auto& record){ return record.custody_precision; });
    }
    uint8_t tradingPrecision() const { 
        return get<uint8_t>([](auto& record){ return record.trading_precision; });
    }
    b1::query::String type() const { 
        return get<b1::query::String>([](auto& record){ return to_string(record.type); });
    }
    uint8_t typeCode() const { 
        return get<uint8_t>([](auto& record){ return to_int(record.type); });
    }
    b1::query::String status() const { 
        return get<b1::query::String>([](auto& record){ return to_string(record.status); });
    }
    uint8_t statusCode() const { 
        return get<uint8_t>([](auto& record){ return to_int(record.status); });
    }
private:
    std::variant<instrument> _data;
};
EOSIO_QUERY_OBJECT(Instrument,
                   ticker,
                   name,
                   nativePrecision,
                   custodyPrecision,
                   tradingPrecision,
                   type,
                   typeCode,
                   status,
                   statusCode)

using InstrumentEdge = b1::query::Edge<Instrument>;
using InstrumentConnection = b1::query::Connection<InstrumentEdge>;

struct Network {
    Network(network net) : _data{std::move(net)} {};
    b1::query::String id() const { 
        return get<b1::query::String>([](auto& record){ return record.id; });
    }
    b1::query::String name() const { 
        return get<b1::query::String>([](auto& record){ return record.name; });
    }
    uint32_t minBlockHeight() const { 
        return get<uint32_t>([](auto& record){ return record.min_block_height; });
    }
    uint32_t requiredConfirmations() const { 
        return get<uint32_t>([](auto& record){ return record.required_confirmations; });
    }
    uint32_t withdrawalBatchSize() const { 
        return get<uint32_t>([](auto& record){ return record.withdrawal_batch_size; });
    }
    b1::query::String addressing() const { 
        return get<b1::query::String>([](auto& record){ return to_string(record.addressing); });
    }
    uint8_t addressingCode() const { 
        return get<uint8_t>([](auto& record){ return to_int(record.addressing); });
    }
    b1::query::String fee() const { 
        return get<b1::query::String>([](auto& record){ return to_string(record.fee); });
    }
    uint8_t feeCode() const { 
        return get<uint8_t>([](auto& record){ return to_int(record.fee); });
    }
    b1::query::String authorization() const { 
        return get<b1::query::String>([](auto& record){ return record.authorization.to_string(); });
    }
    b1::query::String type() const { 
        return get<b1::query::String>([](auto& record){ return to_string(record.type); });
    }
    uint8_t typeCode() const { 
        return get<uint8_t>([](auto& record){ return to_int(record.type); });
    }
    b1::query::String status() const { 
        return get<b1::query::String>([](auto& record){ return to_string(record.status); });
    }
    uint8_t statusCode() const { 
        return get<uint8_t>([](auto& record){ return to_int(record.status); });
    }
private:
    std::variant<network> _data;
}
EOSIO_QUERY_OBJECT(Network,
                   id,
                   name,
                   minBlockHeight,
                   requiredConfirmations,
                   withdrawalBatchSize,
                   addressing,
                   addressingCode,
                   fee,
                   feeCode,
                   authorization,
                   type,
                   typeCode,
                   status,
                   statusCode)

using NetworkEdge = b1::query::Edge<Network>;
using NetworkConnection = b1::query::Connection<NetworkEdge>;

struct Coin {
    Coin(coin c) : _data{std::move(c)} {};
    b1::query::String id() const { 
        return get<b1::query::String>([](auto& record){ return record.id; });
    }
    b1::query::String instrumentID() const { 
        return get<b1::query::String>([](auto& record){ return record.instrument_id; });
    }
    b1::query::String networkID() const { 
        return get<b1::query::String>([](auto& record){ return record.network_id; });
    }
    b1::query::String withdrawalFee() const { 
        return get<b1::query::String>([](auto& record){ return record.withdrawal_fee.to_string(); });
    }
    b1::query::String status() const { 
        return get<b1::query::String>([](auto& record){ return to_string(record.status); });
    }
    uint8_t statusCode() const { 
        return get<uint8_t>([](auto& record){ return to_int(record.status); });
    }
private:
    std::variant<coin> _data;
}
EOSIO_QUERY_OBJECT(Coin,
                   id,
                   instrumentID,
                   networkID,
                   withdrawalFee,
                   status,
                   statusCode)

using CoinEdge = b1::query::Edge<Coin>;
using CoinConnection = b1::query::Connection<CoinEdge>;

struct Query {
    std::optional<Instrument> instrument(const b1::query::String& ticker) const {
        auto record = instrument_table::instance().get(ticker);
        if (!record) {
            return {};
        }
        return {*record};
    }
    InstrumentConnection instruments(const b1::query::Range& r) const {
        auto& table = instrument_table::instance();
        return b1::query::getConnection<InstrumentConnection>(table.by_ticker, r);
    }
    std::optional<Network> network(const b1::query::String& id) const {
        auto record = network_table::instance().get(id);
        if (!record) {
            return {};
        }
        return {*record};
    }
    NetworkConnection networks(const b1::query::Range& r) const {
        auto& table = network_table::instance();
        return b1::query::getConnection<NetworkConnection>(table.by_ticker, r);
    }
    std::optional<Coin> coin(const b1::query::String& id) const {
        auto record = coin_table::instance().get(id);
        if (!record) {
            return {};
        }
        return {*record};
    }
    CoinConnection coins(const b1::query::Range& r) const {
        auto& table = coin_table::instance();
        return b1::query::getConnection<CoinConnection>(table.by_ticker, r);
    }
};
EOSIO_QUERY_OBJECT(Query,
                   instrument,
                   instruments,
                   network,
                   networks,
                   coin,
                   coins)

struct [[eosio::contract("coins.query")]] query_contract : public eosio::contract
{
    using contract::contract;
    [[eosio::action]] eosio::anyvar queryit(const eosio::query& query) const;
};

namespace query {
EOSIO_ACTIONS(query_contract, b1x::coins_query_account, queryit)
}

struct QueryRoot {
    Query query() const { return {}; }

};
EOSIO_QUERY_OBJECT(QueryRoot, query)

eosio::anyvar query_contract::queryit(const eosio::query& query) const {
    auto context = eosio::SchemaQuery<QueryRoot>{};
    return eosio::dispatch(context, query);
}

// To execute a query
cleos push action coins.q queryit '
{
    "method": "query",
    "arg": null,
    "filter: [
        {
            "method": "coin",
            "arg": "BTC@BTC",
            "filter": [
                {
                    "method": "id",
                    "arg", null,
                    "filter": []
                },
                {
                    "method": "instrumentID",
                    "arg", null,
                    "filter": []
                },
                {
                    "method": "networkID",
                    "arg", null,
                    "filter": []
                },
                {
                    "method": "withdrawalFee",
                    "arg", null,
                    "filter": []
                },
                {
                    "method": "status",
                    "arg", null,
                    "filter": []
                },
                {
                    "method": "statusCode",
                    "arg", null,
                    "filter": []
                },
            ]
        }
    ]
}'
