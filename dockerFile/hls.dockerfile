FROM alpine:latest as Nginx
MAINTAINER Jekinnnnnn

ARG NGINX_VERSION=1.15.3

# install libs
RUN	apk update \
	&& apk add	--no-cache	\
		git	\
		gcc	\
		binutils \
		gmp	\
		isl \
		libgomp	\
		libatomic \
		libgcc \
		openssl	\
		pkgconf	\
		pkgconfig \
		mpfr3 \
		mpc1 \
		libstdc++ \
		ca-certificates \
		libssh2	\
		curl \
		expat \
		pcre \
		musl-dev \
		libc-dev \
		pcre-dev \
		zlib-dev \
		openssl-dev	\
		make

# init nginx
RUN	cd /tmp \
	&& wget http://nginx.org/download/nginx-${NGINX_VERSION}.tar.gz \
	&& git clone https://github.com/arut/nginx-rtmp-module.git \
	&& tar xzf nginx-${NGINX_VERSION}.tar.gz \
	&& cd nginx-${NGINX_VERSION} \
	&& ./configure \
		--prefix=/opt/nginx \
		--with-http_ssl_module \
		--add-module=../nginx-rtmp-module \
		# to disable "fallthrough" in gcc for compliling err
		# cause by alpine latest?
		--with-cc-opt="-Wimplicit-fallthrough=0" \
	&& make \
	&& make install \
	&& rm /opt/nginx/conf/nginx.conf \
	&& rm ../nginx-${NGINX_VERSION}.tar.gz \
	&& rm -rf ../nginx-rtmp-module \
	&& rm -rf ../nginx-${NGINX_VERSION}

# copy config file
COPY hls.conf /opt/nginx/conf/nginx.conf

VOLUME /kideyes

EXPOSE 1935
EXPOSE 8080

#does not invoke cmd shell
CMD ["/opt/nginx/sbin/nginx", "-g", "daemon off;"]