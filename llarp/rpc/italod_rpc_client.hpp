#pragma once

#include <router_id.hpp>

#include <italomq/italomq.h>
#include <italomq/address.h>
#include <crypto/types.hpp>
#include <dht/key.hpp>
#include <service/name.hpp>

namespace llarp
{
  struct AbstractRouter;

  namespace rpc
  {
    using LMQ_ptr = std::shared_ptr<italomq::ItaloMQ>;

    /// The ItalodRpcClient uses italo-mq to talk to make API requests to italod.
    struct ItalodRpcClient : public std::enable_shared_from_this<ItalodRpcClient>
    {
      explicit ItalodRpcClient(LMQ_ptr lmq, AbstractRouter* r);

      /// Connect to italod async
      void
      ConnectAsync(italomq::address url);

      /// blocking request identity key from italod
      /// throws on failure
      SecretKey
      ObtainIdentityKey();

      void
      LookupLNSNameHash(
          dht::Key_t namehash,
          std::function<void(std::optional<service::EncryptedName>)> resultHandler);

     private:
      /// called when we have connected to italod via italomq
      void
      Connected();

      /// do a lmq command on the current connection
      void
      Command(std::string_view cmd);

      void
      UpdateServiceNodeList();

      template <typename HandlerFunc_t, typename Args_t>
      void
      Request(std::string_view cmd, HandlerFunc_t func, const Args_t& args)
      {
        m_italoMQ->request(*m_Connection, std::move(cmd), std::move(func), args);
      }

      template <typename HandlerFunc_t>
      void
      Request(std::string_view cmd, HandlerFunc_t func)
      {
        m_italoMQ->request(*m_Connection, std::move(cmd), std::move(func));
      }

      void
      HandleGotServiceNodeList(std::string json);

      // Handles request from italod for peer stats on a specific peer
      void
      HandleGetPeerStats(italomq::Message& msg);

      std::optional<italomq::ConnectionID> m_Connection;
      LMQ_ptr m_italoMQ;
      std::string m_CurrentBlockHash;

      AbstractRouter* const m_Router;
    };

  }  // namespace rpc
}  // namespace llarp
