include_guard(DIRECTORY)
add_compile_definitions(CCAPI_ENABLE_SERVICE_MARKET_DATA)
add_compile_definitions(CCAPI_ENABLE_SERVICE_EXECUTION_MANAGEMENT)
#
# add_compile_definitions(CCAPI_ENABLE_SERVICE_FIX)
#
add_compile_definitions(CCAPI_ENABLE_EXCHANGE_COINBASE)
#
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_GEMINI)
#
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_KRAKEN)
#
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_KRAKEN_FUTURES)
#
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_BITSTAMP)
#
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_BITFINEX)
#
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_BITMEX)
#
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_BINANCE_US)
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_BINANCE)
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_BINANCE_USDS_FUTURES)
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_BINANCE_COIN_FUTURES)
#
add_compile_definitions(CCAPI_ENABLE_EXCHANGE_HUOBI)
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_HUOBI_USDT_SWAP)
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_HUOBI_COIN_SWAP)
#
add_compile_definitions(CCAPI_ENABLE_EXCHANGE_OKX)
#
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_ERISX)
#
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_KUCOIN)
#
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_FTX)
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_FTX_US)
#
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_DERIBIT)
#
add_compile_definitions(CCAPI_ENABLE_EXCHANGE_GATEIO)

# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_GATEIO_PERPETUAL_FUTURES)
#
add_compile_definitions(CCAPI_ENABLE_EXCHANGE_MEXC)
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_CRYPTOCOM)
#
# add_compile_definitions(CCAPI_ENABLE_EXCHANGE_ASCENDEX)
#
# add_compile_definitions(CCAPI_ENABLE_LOG_TRACE)
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
 add_compile_definitions(CCAPI_ENABLE_LOG_TRACE)
  # add_compile_definitions(CCAPI_ENABLE_LOG_DEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
else()
  add_compile_definitions(CCAPI_ENABLE_LOG_INFO)
  set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG") # disable asserts
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()
set(CCAPI_USE_SINGLE_THREAD "1")
add_compile_definitions(CCAPI_USE_SINGLE_THREAD)
#
# add_compile_definitions(CCAPI_ENABLE_LOG_INFO)
#
# add_compile_definitions(CCAPI_ENABLE_LOG_WARN)
#
# add_compile_definitions(CCAPI_ENABLE_LOG_ERROR)
#
# add_compile_definitions(CCAPI_ENABLE_LOG_FATAL)
#
# set(CCAPI_WEBSOCKET_WRITE_BUFFER_SIZE, "1 << 18")

find_package(ZLIB REQUIRED)
link_libraries(ZLIB::ZLIB)