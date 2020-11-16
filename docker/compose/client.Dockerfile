FROM compose-base:latest

COPY ./docker/compose/client.ini /root/.italonet/italonet.ini

CMD ["/italonet"]
EXPOSE 1090/udp 1190/tcp
