#ifndef LLARP_CONFIG_HPP
#define LLARP_CONFIG_HPP

#include <crypto/types.hpp>
#include <router_contact.hpp>
#include <util/fs.hpp>
#include <util/str.hpp>
#include <config/ini.hpp>
#include <config/definition.hpp>
#include <constants/files.hpp>

#include <cstdlib>
#include <functional>
#include <string>
#include <utility>
#include <vector>
#include <unordered_set>

struct llarp_config;

namespace llarp
{
  using SectionValues_t = llarp::ConfigParser::SectionValues_t;
  using Config_impl_t = llarp::ConfigParser::Config_impl_t;

  // TODO: don't use these maps. they're sloppy and difficult to follow
  using FreehandOptions = std::unordered_multimap<std::string, std::string>;

  /// Small struct to gather all parameters needed for config generation to reduce the number of
  /// parameters that need to be passed around.
  struct ConfigGenParameters
  {
    bool isRelay = false;
    fs::path defaultDataDir;
  };

  struct RouterConfig
  {
    size_t m_minConnectedRouters;
    size_t m_maxConnectedRouters;

    std::string m_netId;
    std::string m_nickname;

    fs::path m_dataDir;

    bool m_blockBogons;

    bool m_publicOverride = false;
    struct sockaddr_in m_ip4addr;
    AddressInfo m_addrInfo;

    int m_workerThreads;
    int m_numNetThreads;

    size_t m_JobQueueSize;

    std::string m_routerContactFile;
    std::string m_encryptionKeyFile;
    std::string m_identityKeyFile;
    std::string m_transportKeyFile;

    void
    defineConfigOptions(ConfigDefinition& conf, const ConfigGenParameters& params);
  };

  struct NetworkConfig
  {
    std::optional<bool> m_enableProfiling;
    std::string m_routerProfilesFile;
    std::string m_strictConnect;
    FreehandOptions m_options;

    void
    defineConfigOptions(ConfigDefinition& conf, const ConfigGenParameters& params);
  };

  struct DnsConfig
  {
    FreehandOptions m_options;

    void
    defineConfigOptions(ConfigDefinition& conf, const ConfigGenParameters& params);
  };

  struct LinksConfig
  {
    struct LinkInfo
    {
      std::string interface;
      int addressFamily;
      uint16_t port = -1;
    };
    /// Create a LinkInfo from the given string.
    /// @throws if str does not represent a LinkInfo.
    LinkInfo
    LinkInfoFromINIValues(std::string_view name, std::string_view value);

    LinkInfo m_OutboundLink;
    std::vector<LinkInfo> m_InboundLinks;

    void
    defineConfigOptions(ConfigDefinition& conf, const ConfigGenParameters& params);
  };

  struct ConnectConfig
  {
    std::vector<fs::path> routers;

    void
    defineConfigOptions(ConfigDefinition& conf, const ConfigGenParameters& params);
  };

  struct ServicesConfig
  {
    std::vector<std::pair<std::string, std::string>> services;
    void
    defineConfigOptions(ConfigDefinition& conf, const ConfigGenParameters& params);
  };

  struct ApiConfig
  {
    bool m_enableRPCServer;
    std::string m_rpcBindAddr;

    void
    defineConfigOptions(ConfigDefinition& conf, const ConfigGenParameters& params);
  };

  struct LokidConfig
  {
    bool usingSNSeed;
    bool whitelistRouters;
    fs::path ident_keyfile;
    std::string lokidRPCAddr;
    std::string lokidRPCUser;
    std::string lokidRPCPassword;

    void
    defineConfigOptions(ConfigDefinition& conf, const ConfigGenParameters& params);
  };

  struct BootstrapConfig
  {
    std::vector<std::string> routers;
    void
    defineConfigOptions(ConfigDefinition& conf, const ConfigGenParameters& params);
  };

  struct LoggingConfig
  {
    LogType m_logType;
    LogLevel m_logLevel;
    std::string m_logFile;

    void
    defineConfigOptions(ConfigDefinition& conf, const ConfigGenParameters& params);
  };

  struct SnappConfig
  {
    std::string m_keyfile;
    bool m_reachable;
    int m_hops;
    int m_paths;
    std::vector<std::string> m_snodeBlacklist;
    std::string m_exitNode;
    std::string m_localDNS;
    std::string m_upstreamDNS;
    std::string m_mapAddr;

    void
    defineConfigOptions(ConfigDefinition& conf, const ConfigGenParameters& params);
  };

  struct Config
  {
    RouterConfig router;
    NetworkConfig network;
    ConnectConfig connect;
    DnsConfig dns;
    LinksConfig links;
    ServicesConfig services;
    ApiConfig api;
    LokidConfig lokid;
    BootstrapConfig bootstrap;
    LoggingConfig logging;

    // Initialize config definition
    void
    initializeConfig(ConfigDefinition& conf, const ConfigGenParameters& params);

    /// Insert config entries for backwards-compatibility (e.g. so that the config system will
    /// tolerate old values that are no longer accepted)
    ///
    /// @param conf is the config to modify
    void
    addBackwardsCompatibleConfigOptions(ConfigDefinition& conf);

    // Load a config from the given file
    bool
    Load(const char* fname, bool isRelay, fs::path defaultDataDir);

    /// Load (initialize) a default config.
    ///
    /// This delegates to the ConfigDefinition to generate a default config,
    /// as though an empty config were specified.
    ///
    /// If using Config without the intention of loading from file (or string), this is necessary
    /// in order to obtain sane defaults.
    ///
    /// @param isRelay determines whether the config will reflect that of a relay or client
    /// @param dataDir is a path representing a directory to be used as the data dir
    /// @return true on success, false otherwise
    bool
    LoadDefault(bool isRelay, fs::path dataDir);

    std::string
    generateBaseClientConfig(fs::path defaultDataDir);

    std::string
    generateBaseRouterConfig(fs::path defaultDataDir);

    llarp_config*
    Copy() const;
  };

  void
  ensureConfig(
      const fs::path& defaultDataDir, const fs::path& confFile, bool overwrite, bool asRouter);

}  // namespace llarp

#endif
