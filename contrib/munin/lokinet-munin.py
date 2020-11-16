#!/usr/bin/env python3
#
# requires python3-requests
#
import requests
import json
import os
import sys

from collections import defaultdict as Dict

from requests.exceptions import RequestException


def jsonrpc(method, **args):
    return requests.post('http://127.0.0.1:1190/', data=json.dumps(
        {'method': method, 'params': args, 'id': 'munin'}), headers={'content-type': 'application/json'}).json()


def exit_sessions_main():
    if len(sys.argv) == 2 and sys.argv[1] == 'config':
        print("graph_title italonet exit sessions")
        print("graph_vlabel sessions")
        print("graph_category network")
        print("graph_info This graph shows the number of exit sessions on a italonet exit")
        print("_exit_sessions.info Number of exit sessions")
        print("_exit_sessions.label sessions")
    else:
        count = 0
        try:
            j = jsonrpc("llarp.admin.exit.list")
            count = len(j['result'])
        except RequestException:
            pass
        print("_exit_sessions.value {}".format(count))


def peers_main():
    if len(sys.argv) == 2 and sys.argv[1] == 'config':
        print("graph_title italonet peers")
        print("graph_vlabel peers")
        print("graph_category network")
        print("graph_info This graph shows the number of node to node sessions of this italonet router")
        print("_peers_outbound.info Number of outbound italonet peers")
        print("_peers_inbound.info Number of inbound italonet peers")
        print("_peers_outbound.label outbound peers")
        print("_peers_inbound.label inbound peers")
        print("_peers_clients.info Number of italonet client peers")
        print("_peers_clients.label italonet client peers")
    else:
        inbound = Dict(int)
        outbound = Dict(int)
        clients = Dict(int)
        try:
            j = jsonrpc("llarp.admin.link.neighboors")
            for peer in j['result']:
                if peer["svcnode"]:
                    if peer["outbound"]:
                        outbound[peer['ident']] += 1
                    else:
                        inbound[peer['ident']] += 1
                else:
                    clients[peer['ident']] += 1
        except RequestException:
            pass

        print("_peers_outbound.value {}".format(len(outbound)))
        print("_peers_inbound.value {}".format(len(inbound)))
        print("_peers_clients.value {}".format(len(clients)))

if __name__ == '__main__':
    exe = os.path.basename(sys.argv[0]).lower()
    if exe == 'italonet_peers':
        peers_main()
    elif exe == 'italonet_exit':
        exit_sessions_main()
    else:
        print(
            'please symlink this as `italonet_peers` or `italonet_exit` in munin plugins dir')
