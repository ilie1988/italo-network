#include <rpc/italod_rpc_client.hpp>

#include <stdexcept>
#include <util/logging/logger.hpp>

#include <router/abstractrouter.hpp>

#include <nlohmann/json.hpp>

#include <util/time.hpp>
#include <util/thread/logic.hpp>

namespace llarp
{
  namespace rpc
  {
    static italomq::LogLevel
    toItaloMQLogLevel(llarp::LogLevel level)
    {
      switch (level)
      {
        case eLogError:
          return italomq::LogLevel::error;
        case eLogWarn:
          return italomq::LogLevel::warn;
        case eLogInfo:
          return italomq::LogLevel::info;
        case eLogDebug:
          return italomq::LogLevel::debug;
        case eLogNone:
        default:
          return italomq::LogLevel::trace;
      }
    }

    ItalodRpcClient::ItalodRpcClient(LMQ_ptr lmq, AbstractRouter* r)
        : m_italoMQ(std::move(lmq)), m_Router(r)
    {
      // m_italoMQ->log_level(toItaloMQLogLevel(LogLevel::Instance().curLevel));

      // TODO: proper auth here
      auto italodCategory = m_italoMQ->add_category("italod", italomq::Access{italomq::AuthLevel::none});
      italodCategory.add_request_command(
          "get_peer_stats", [this](italomq::Message& m) { HandleGetPeerStats(m); });
    }

    void
    ItalodRpcClient::ConnectAsync(italomq::address url)
    {
      if (not m_Router->IsServiceNode())
      {
        throw std::runtime_error("we cannot talk to italod while not a service node");
      }
      LogInfo("connecting to italod via LMQ at ", url);
      m_Connection = m_italoMQ->connect_remote(
          url,
          [self = shared_from_this()](italomq::ConnectionID) { self->Connected(); },
          [self = shared_from_this(), url](italomq::ConnectionID, std::string_view f) {
            llarp::LogWarn("Failed to connect to italod: ", f);
            LogicCall(self->m_Router->logic(), [self, url]() { self->ConnectAsync(url); });
          });
    }

    void
    ItalodRpcClient::Command(std::string_view cmd)
    {
      LogDebug("italod command: ", cmd);
      m_italoMQ->send(*m_Connection, std::move(cmd));
    }

    void
    ItalodRpcClient::UpdateServiceNodeList()
    {
      nlohmann::json request, fields;
      fields["pubkey_ed25519"] = true;
      request["fields"] = fields;
      request["active_only"] = true;
      if (not m_CurrentBlockHash.empty())
        request["poll_block_hash"] = m_CurrentBlockHash;
      Request(
          "rpc.get_service_nodes",
          [self = shared_from_this()](bool success, std::vector<std::string> data) {
            if (not success)
            {
              LogWarn("failed to update service node list");
              return;
            }
            if (data.size() < 2)
            {
              LogWarn("italod gave empty reply for service node list");
              return;
            }
            try
            {
              self->HandleGotServiceNodeList(std::move(data[1]));
            }
            catch (std::exception& ex)
            {
              LogError("failed to process service node list: ", ex.what());
            }
          },
          request.dump());
    }

    void
    ItalodRpcClient::Connected()
    {
      constexpr auto PingInterval = 30s;
      constexpr auto NodeListUpdateInterval = 30s;

      auto makePingRequest = [self = shared_from_this()]() {
        nlohmann::json payload = {{"version", {VERSION[0], VERSION[1], VERSION[2]}}};
        self->Request(
            "admin.italonet_ping",
            [](bool success, std::vector<std::string> data) {
              (void)data;
              LogDebug("Received response for ping. Successful: ", success);
            },
            payload.dump());
      };
      m_italoMQ->add_timer(makePingRequest, PingInterval);
      m_italoMQ->add_timer(
          [self = shared_from_this()]() { self->UpdateServiceNodeList(); }, NodeListUpdateInterval);
      UpdateServiceNodeList();
    }

    void
    ItalodRpcClient::HandleGotServiceNodeList(std::string data)
    {
      auto j = nlohmann::json::parse(std::move(data));
      {
        const auto itr = j.find("block_hash");
        if (itr != j.end())
        {
          m_CurrentBlockHash = itr->get<std::string>();
        }
      }
      {
        const auto itr = j.find("unchanged");
        if (itr != j.end())
        {
          if (itr->get<bool>())
          {
            LogDebug("service node list unchanged");
            return;
          }
        }
      }

      std::vector<RouterID> nodeList;
      {
        const auto itr = j.find("service_node_states");
        if (itr != j.end() and itr->is_array())
        {
          for (auto j_itr = itr->begin(); j_itr != itr->end(); j_itr++)
          {
            const auto ed_itr = j_itr->find("pubkey_ed25519");
            if (ed_itr == j_itr->end() or not ed_itr->is_string())
              continue;
            RouterID rid;
            if (rid.FromHex(ed_itr->get<std::string>()))
              nodeList.emplace_back(std::move(rid));
          }
        }
      }

      if (nodeList.empty())
      {
        LogWarn("got empty service node list from italod");
        return;
      }
      // inform router about the new list
      LogicCall(m_Router->logic(), [r = m_Router, nodeList = std::move(nodeList)]() mutable {
        r->SetRouterWhitelist(std::move(nodeList));
      });
    }

    SecretKey
    ItalodRpcClient::ObtainIdentityKey()
    {
      std::promise<SecretKey> promise;
      Request(
          "admin.get_service_privkeys",
          [self = shared_from_this(), &promise](bool success, std::vector<std::string> data) {
            try
            {
              if (not success)
              {
                throw std::runtime_error(
                    "failed to get private key request "
                    "failed");
              }
              if (data.empty() or data.size() < 2)
              {
                throw std::runtime_error(
                    "failed to get private key request "
                    "data empty");
              }
              const auto j = nlohmann::json::parse(data[1]);
              SecretKey k;
              if (not k.FromHex(j.at("service_node_ed25519_privkey").get<std::string>()))
              {
                throw std::runtime_error("failed to parse private key");
              }
              promise.set_value(k);
            }
            catch (const std::exception& e)
            {
              LogWarn("Caught exception while trying to request admin keys: ", e.what());
              promise.set_exception(std::current_exception());
            }
            catch (...)
            {
              LogWarn("Caught non-standard exception while trying to request admin keys");
              promise.set_exception(std::current_exception());
            }
          });
      auto ftr = promise.get_future();
      return ftr.get();
    }

    void
    ItalodRpcClient::LookupLNSNameHash(
        dht::Key_t namehash,
        std::function<void(std::optional<service::EncryptedName>)> resultHandler)
    {
      LogDebug("Looking Up LNS NameHash ", namehash);
      const nlohmann::json req{{"type", 2}, {"name_hash", namehash.ToHex()}};
      Request(
          "rpc.lns_resolve",
          [r = m_Router, resultHandler](bool success, std::vector<std::string> data) {
            std::optional<service::EncryptedName> maybe = std::nullopt;
            if (success)
            {
              try
              {
                service::EncryptedName result;
                const auto j = nlohmann::json::parse(data[1]);
                result.ciphertext = italomq::from_hex(j["encrypted_value"].get<std::string>());
                const auto nonce = italomq::from_hex(j["nonce"].get<std::string>());
                if (nonce.size() != result.nonce.size())
                {
                  throw std::invalid_argument(stringify(
                      "nonce size mismatch: ", nonce.size(), " != ", result.nonce.size()));
                }

                std::copy_n(nonce.data(), nonce.size(), result.nonce.data());
                maybe = result;
              }
              catch (std::exception& ex)
              {
                LogError("failed to parse response from lns lookup: ", ex.what());
              }
            }
            LogicCall(r->logic(), [resultHandler, maybe]() { resultHandler(maybe); });
          },
          req.dump());
    }

    void
    ItalodRpcClient::HandleGetPeerStats(italomq::Message& msg)
    {
      LogInfo("Got request for peer stats (size: ", msg.data.size(), ")");
      for (auto str : msg.data)
      {
        LogInfo("    :", str);
      }

      assert(m_Router != nullptr);

      if (not m_Router->peerDb())
      {
        LogWarn("HandleGetPeerStats called when router has no peerDb set up.");

        // TODO: this can sometimes occur if italod hits our API before we're done configuring
        //       (mostly an issue in a loopback testnet)
        msg.send_reply("EAGAIN");
        return;
      }

      try
      {
        // msg.data[0] is expected to contain a bt list of router ids (in our preferred string
        // format)
        if (msg.data.empty())
        {
          LogWarn("italod requested peer stats with no request body");
          msg.send_reply("peer stats request requires list of router IDs");
          return;
        }

        std::vector<std::string> routerIdStrings;
        italomq::bt_deserialize(msg.data[0], routerIdStrings);

        std::vector<RouterID> routerIds;
        routerIds.reserve(routerIdStrings.size());

        for (const auto& routerIdString : routerIdStrings)
        {
          RouterID id;
          if (not id.FromString(routerIdString))
          {
            LogWarn("italod sent us an invalid router id: ", routerIdString);
            msg.send_reply("Invalid router id");
            return;
          }

          routerIds.push_back(std::move(id));
        }

        auto statsList = m_Router->peerDb()->listPeerStats(routerIds);

        int32_t bufSize =
            256 + (statsList.size() * 1024);  // TODO: tune this or allow to grow dynamically
        auto buf = std::unique_ptr<uint8_t[]>(new uint8_t[bufSize]);
        llarp_buffer_t llarpBuf(buf.get(), bufSize);

        PeerStats::BEncodeList(statsList, &llarpBuf);

        msg.send_reply(std::string_view((const char*)llarpBuf.base, llarpBuf.cur - llarpBuf.base));
      }
      catch (const std::exception& e)
      {
        LogError("Failed to handle get_peer_stats request: ", e.what());
        msg.send_reply("server error");
      }
    }

  }  // namespace rpc
}  // namespace llarp
