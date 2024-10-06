// An example of a nodeos contract code data structures that are serialized and stored on chain
// These would normally be define within a nodeos Smart Contract, but for simplicity I'm just going to 
// show what the data structures look like.  These will be the same for bot the wasmql and cpplinq based
// contracts.
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
