#ifndef BOOTSERV_ITALONET_CRON_HPP
#define BOOTSERV_ITALONET_CRON_HPP

#include "handler.hpp"

namespace italonet
{
  namespace bootserv
  {
    struct CronHandler final : public Handler
    {
      CronHandler(std::ostream& o);
      ~CronHandler();

      int
      Exec(const Config& conf) override;

      int
      ReportError(const char* err) override;
    };

  }  // namespace bootserv
}  // namespace italonet

#endif
