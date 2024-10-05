
// An example of a nodeos contract code data structures that are serialized and stored on chain

struct instrument {
    std::string     ticker;
    std::string     name;
    uint8_t         native_precision{0};
    uint8_t         custody_precision{0};
    uint8_t         trading_precision{0};
    instrument_type type{instrument_type::crypto};
    status_type     status{status_type::unavailable};

    auto by_ticker() const { return ticker; }
    auto by_name() const { return name; }
};
EOSIO_REFLECT(instrument, 
              ticker, 
              name, 
              native_precision, 
              custody_precision, 
              trading_precision, 
              type, 
              status)

struct network {
    std::string     id;
    std::string     name;
    uint32_t        min_block_height{0};
    uint16_t        required_confirmations{0};
    uint16_t        withdrawal_batch_size{0};
    addressing_type addressing{addressing_type::by_address};
    fee_type        fee{fee_type::no_fee};
    eosio::name     authorization;
    network_type    type{network_type::fiat};
    status_type     status{status_type::unavailable};

    auto by_id() const { return id; }
    auto by_name() const {return name; }
};
EOSIO_REFLECT(network, 
              id,
              name,
              min_block_height,
              required_confirmations,
              withdrawal_batch_size,
              addressing,
              fee,
              authorization,
              type, 
              status)

struct coin {
    std::string id;
    std::string instrument_id;
    std::string network_id;
    bullish::coin withdrawal_fee;
    status_type status{status_type::unavailable};
    
    auto by_id() const { return id; }
    auto by_instrument_id() const { return instrument_id; }
    auto by_network_id() const { return network_id; }
};
EOSIO_REFLECT(coin,
              id,
              instrument_id,
              network_id,
              withdrawal_fee,
              status)

struct [[eosio::table, eosio::contract("coins")]] instrument_table : eosio::kv::table<instrument, "instv1"_n>
{
    KV_NAMED_PRIMARY_INDEX("by.ticker"_n, by_ticker);
    KV_NAMED_INDEX("by.name"_n, by_name);

    instrument_table();

    static instrument_table& instance();
};

struct [[eosio::table, eosio::contract("coins")]] network_table : eosio::kv::table<network, "netwv1"_n>
{
    KV_NAMED_PRIMARY_INDEX("by.id"_n, by_id);
    KV_NAMED_INDEX("by.name"_n, by_name);

    network_table();

    static network_table& instance();
};

struct [[eosio::table, eosio::contract("coins")]] coin_table : eosio::kv::table<coin, "coinv1"_n>
{
    KV_NAMED_PRIMARY_INDEX("by.id"_n, by_id);
    KV_NAMED_INDEX("by.instr"_n, by_instrument_id);
    KV_NAMED_INDEX("by.netw"_n, by_network_id);

    coin_table();

    static coin_table& instance();
};

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

// Implementing the SQL query contract (same header file, different source file)

// The information declare in these macros could quite possibly be code generated while iterating
// over the AST by annotating the types like this
//
// struct [[cpplinq::table]] instrument_table { ... }
DECLARE_TABLE(
    "instrument_table",         // The table name that is used in the sql queries
    instrument_table,           // The declared type of the table
    ((ticker, std::string))     // The columns declared on the table (doesn't need to be all, this can be considerd a "view" of the table)
    ((name, std::string))
    ((native_precision, uint8_t))
    ((custody_precision, uint8_t))
    ((trading_precision, uint8_t))
    ((type, instrument_type))
    ((status, status_type)),
    ((by_ticker, ((ticker, std::string)))) // The indices declared on the table (doesn't need to be all)
    ((by_name, ((name, std::string))((ticker, std::string))))
)

DECLARE_TABLE(
    "network_table",        
    network_table,           
    ((id, std::string))     
    ((name, std::string))
    ((min_block_height, uint32_t))
    ((required_confirmations, uint16_t))
    ((withdrawal_batch_size, uint16_t))
    ((addressing, addressing_type))
    ((fee, fee_type))
    ((type, network_type))
    ((status, status_type)),
    ((by_id, ((id, std::string)))) 
    ((by_name, ((name, std::string))((id, std::string))))
)

DECLARE_TABLE(
    "coin_table",        
    coin_table,           
    ((id, std::string))     
    ((instrument_id, std::string))
    ((network_id, std::string))
    ((status, status_type)),
    ((by_instrument_id, ((instrument_id, std::string))((id, std::string)))) 
    ((by_network_id, ((network_id, std::string))((id, std::string))))
)

struct [[eosio::contract("coins.query")]] query_contract : public eosio::contract
{
    using contract::contract;
    [[eosio::action]] std::vector<std::vector<eosio::anyvar>> sql(const std::string& query) const;
};

namespace query {
EOSIO_ACTIONS(query_contract, b1x::coins_query_account, queryit)
}

struct QueryRoot {
    Query query() const { return {}; }

};
EOSIO_QUERY_OBJECT(QueryRoot, query)

std::vector<std::vector<eosio::anyvar>> query_contract::sql(const std::string& query) const {
    return cpplinq::sql_parser::sql::evaluate(query);
}

