#include "catch2/catch.hpp"
#include <crypto/crypto_libsodium.hpp>
#include <service/name.hpp>
#include <italomq/hex.h>

using namespace std::literals;

TEST_CASE("Test LNS name decrypt", "[lns]")
{
  llarp::sodium::CryptoLibSodium crypto;
  constexpr auto recordhex = "0ba76cbfdb6dc8f950da57ae781912f31c8ad0c55dbf86b88cb0391f563261a9656571a817be4092969f8a78ee0fcee260424acb4a1f4bbdd27348b71de006b6152dd04ed11bf3c4"sv;
  const auto recordbin = italomq::from_hex(recordhex);
  CHECK(not recordbin.empty());
  llarp::SymmNonce n{};
  std::vector<byte_t> ciphertext{};
  const auto len = recordbin.size() - n.size();
  std::copy_n(recordbin.cbegin() + len, n.size(), n.data());
  std::copy_n(recordbin.cbegin(), len, std::back_inserter(ciphertext));
  const auto maybe = crypto.maybe_decrypt_name(std::string_view{reinterpret_cast<const char *>(ciphertext.data()), ciphertext.size()}, n, "jason.italo");
  CHECK(maybe.has_value());
  const llarp::service::Address addr{*maybe};
  CHECK(addr.ToString() == "azfoj73snr9f3neh5c6sf7rtbaeabyxhr1m4un5aydsmsrxo964o.italo");
}


TEST_CASE("Test LNS validity", "[lns]")
{
  CHECK(not llarp::service::NameIsValid("italo.italo"));
  CHECK(not llarp::service::NameIsValid("snode.italo"));
  CHECK(not llarp::service::NameIsValid("localhost.italo"));
  CHECK(not llarp::service::NameIsValid("gayballs22.italo.italo"));
  CHECK(not llarp::service::NameIsValid("-italo.italo"));
  CHECK(not llarp::service::NameIsValid("super-mario-gayballs-.italo"));
  CHECK(not llarp::service::NameIsValid("bn--lolexdeeeeee.italo"));
  CHECK(not llarp::service::NameIsValid("2222222222a-.italo"));
  CHECK(not llarp::service::NameIsValid("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.italo"));
  
  CHECK(llarp::service::NameIsValid("xn--animewasindeedamistake.italo"));
  CHECK(llarp::service::NameIsValid("memerionos.italo"));
  CHECK(llarp::service::NameIsValid("whyis.xn--animehorrible.italo"));
  CHECK(llarp::service::NameIsValid("the.goog.italo"));
  CHECK(llarp::service::NameIsValid("420.italo"));
}
