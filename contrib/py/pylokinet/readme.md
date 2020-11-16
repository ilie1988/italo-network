# pyitalonet

italonet with python 3

    # python3 setup.py install

## bootserv

bootserv is a bootstrap server for accepting and serving RCs

    $ gunicorn -b 0.0.0.0:8000 pyitalonet.bootserv:app

## pyitalonet instance

obtain `libitalonet-shared.so` from a italonet build

run (root):
    
    # export ITALONET_ROOT=/tmp/italonet-instance/
    # export ITALONET_LIB=/path/to/libitalonet-shared.so
    # export ITALONET_BOOTSTRAP_URL=http://bootserv.ip.address.here:8000/bootstrap.signed
    # export ITALONET_PING_URL=http://bootserv.ip.address.here:8000/ping
    # export ITALONET_SUBMIT_URL=http://bootserv.ip.address.here:8000/
    # export ITALONET_IP=public.ip.goes.here
    # export ITALONET_PORT=1090
    # export ITALONET_IFNAME=eth0
    # python3 -m pyitalonet