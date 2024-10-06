// Implementing the CppLinq based SQL query contract (same header file, different source file)

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

