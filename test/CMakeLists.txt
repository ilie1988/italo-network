if (WITH_HIVE)
  add_custom_target(hive_build DEPENDS libitalonet pyllarp)
  add_custom_target(hive ${CMAKE_COMMAND} -E
      env PYTHONPATH="$ENV{PYTHONPATH}:${CMAKE_BINARY_DIR}/pybind"
      ${PYTHON_EXECUTABLE} -m pytest
      ${CMAKE_CURRENT_SOURCE_DIR}/hive
      DEPENDS
      hive_build)
endif()

# Old gtest-based tests; new tests should use Catch2, instead.
add_executable(testAll
    # helpers
    main.cpp
    crypto/mock_crypto.cpp
    dht/mock_context.cpp
    test_util.cpp
    # actual test cases
    config/test_llarp_config_ini.cpp
    crypto/test_llarp_crypto_types.cpp
    crypto/test_llarp_crypto.cpp
    crypto/test_llarp_key_manager.cpp
    dht/test_llarp_dht_bucket.cpp
    dht/test_llarp_dht_explorenetworkjob.cpp
    dht/test_llarp_dht_kademlia.cpp
    dht/test_llarp_dht_key.cpp
    dht/test_llarp_dht_node.cpp
    dht/test_llarp_dht_tx.cpp
    dht/test_llarp_dht_txowner.cpp
    llarp_test.cpp
    net/test_llarp_net.cpp
    router/test_llarp_router_version.cpp
    routing/llarp_routing_transfer_traffic.cpp
    routing/test_llarp_routing_obtainexitmessage.cpp
    service/test_llarp_service_address.cpp
    test_llarp_encrypted_frame.cpp
    util/meta/test_llarp_util_memfn.cpp
    util/meta/test_llarp_util_traits.cpp
    util/test_llarp_util_aligned.cpp
    util/test_llarp_util_bencode.cpp
    util/test_llarp_util_log_level.cpp
    util/thread/test_llarp_util_queue_manager.cpp
    util/thread/test_llarp_util_queue.cpp
)

target_link_libraries(testAll PUBLIC gmock gtest libitalonet)
target_include_directories(testAll PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_sources(testAll PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/win32/test.rc")
    target_link_libraries(testAll PUBLIC ws2_32 iphlpapi shlwapi)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    target_link_directories(testAll PRIVATE /usr/local/lib)
endif()

add_subdirectory(Catch2)

add_executable(catchAll
  nodedb/test_nodedb.cpp
  path/test_path.cpp
  dns/test_llarp_dns_dns.cpp
  regress/2020-06-08-key-backup-bug.cpp
  util/test_llarp_util_bits.cpp
  util/test_llarp_util_printer.cpp
  util/test_llarp_util_str.cpp
  util/test_llarp_util_decaying_hashset.cpp
  peerstats/test_peer_db.cpp
  peerstats/test_peer_types.cpp
  config/test_llarp_config_definition.cpp
  config/test_llarp_config_output.cpp
  net/test_ip_address.cpp
  net/test_sock_addr.cpp
  service/test_llarp_service_name.cpp
  exit/test_llarp_exit_context.cpp
  iwp/test_iwp_session.cpp
  service/test_llarp_service_identity.cpp
  test_util.cpp
  test_llarp_router_contact.cpp
  check_main.cpp)

target_link_libraries(catchAll PUBLIC libitalonet Catch2::Catch2)
target_include_directories(catchAll PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Custom targets to invoke the different test suites:
add_custom_target(catch COMMAND catchAll)
add_custom_target(rungtest COMMAND testAll)

# Add a custom "check" target that runs all the test suites:
add_custom_target(check DEPENDS rungtest catch)
