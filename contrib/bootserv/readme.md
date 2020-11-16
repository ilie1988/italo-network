# italonet-bootserv

cgi executable for serving a random RC for bootstrap from a nodedb

## configuring

copy the example config (privileged)

    # cp configs/italonet-bootserv.ini /usr/local/etc/italonet-bootserv.ini
    
edit config to have proper values, 
specifically  make sure the `[nodedb]` section has a `dir` value that points to a static copy of a healthy nodedb

## building

to build:

    $ make

## installing (priviledged)

install cgi binary:

    # cp italonet-bootserv /usr/local/bin/italonet-bootserv 

set up with nginx cgi:

    # cp configs/italonet-bootserv-nginx.conf /etc/nginx/sites-available/italonet-bootserv.conf
    # ln -s /etc/nginx/sites-available/italonet-bootserv.conf /etc/nginx/sites-enabled/ 

## maintainence

add the following to crontab

    0 0 * * * /usr/local/bin/italonet-bootserv --cron
