project(store)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( store store store.cpp )
target_include_directories( store PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_ricardian_directory( store ${CMAKE_SOURCE_DIR}/../ricardian )