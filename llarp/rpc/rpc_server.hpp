#pragma once

#include <string_view>
#include <italomq/italomq.h>
#include <italomq/address.h>

namespace llarp
{
  struct AbstractRouter;
}

namespace llarp::rpc
{
  using LMQ_ptr = std::shared_ptr<italomq::ItaloMQ>;

  struct RpcServer
  {
    explicit RpcServer(LMQ_ptr, AbstractRouter*);
    ~RpcServer() = default;
    void
    AsyncServeRPC(italomq::address addr);

   private:
    LMQ_ptr m_LMQ;
    AbstractRouter* const m_Router;
  };
}  // namespace llarp::rpc
