FROM compose-base:latest

ENV ITALONET_NETID=docker

COPY ./docker/compose/bootstrap.ini /root/.italonet/italonet.ini

CMD ["/italonet"]
EXPOSE 1090/udp 1190/tcp
