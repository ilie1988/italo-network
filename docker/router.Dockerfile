ARG bootstrap="https://i2p.rocks/i2procks.signed"
FROM alpine:edge as builder

RUN apk update && \
    apk add build-base cmake git libcap-dev libcap-static libuv-dev libuv-static curl ninja bash binutils-gold curl-dev

WORKDIR /src/
COPY . /src/

RUN make NINJA=ninja STATIC_LINK=ON BUILD_TYPE=Release DOWNLOAD_SODIUM=ON
RUN ./italonet-bootstrap ${bootstrap}

FROM alpine:latest

COPY italonet-docker.ini /root/.italonet/italonet.ini
COPY --from=builder /src/build/daemon/italonet .
COPY --from=builder /root/.italonet/bootstrap.signed /root/.italonet/

CMD ["./italonet"]
EXPOSE 1090/udp 1190/tcp
