FROM ubuntu:latest
WORKDIR /tmp
COPY . .
RUN rm -rf build

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update
RUN apt-get install -yq git cmake pkg-config build-essential linux-headers-$(uname -r)
RUN apt-get install -yq libssl-dev libcurl4-openssl-dev libcurlpp-dev libmagick++-dev

# Lower SSL security level (see https://askubuntu.com/questions/1233186/ubuntu-20-04-how-to-set-lower-ssl-security-level)
COPY docker/openssl.cnf.1 .
COPY docker/openssl.cnf.2 .
RUN cat openssl.cnf.1 > openssl.cnf.tmp
RUN cat /etc/ssl/openssl.cnf >> openssl.cnf.tmp
RUN cat openssl.cnf.2 >> openssl.cnf.tmp
RUN mv openssl.cnf.tmp /etc/ssl/openssl.cnf

WORKDIR /data
RUN cmake /tmp
RUN cmake --build .

CMD ["/data/WideBot"]
