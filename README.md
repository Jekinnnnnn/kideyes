### What is kideyes
Identify diffierent frame which is in a live stream in a confidence interval base on sample stream.Kideyes is mainly composed of four parts.
1. Using docker builds a streaming server
2. Streaming by HLS
3. Processing ts files by FFmpeg(version 3.4.6) for extracting frame
4. Using OpenCV identifying images

### Usege
- init hls server
    - download hls.dockerfile and hls.conf in a new folder
    - docker build with hls.dockerfile
    - docker run \-p \$\{rtmp_host_post\}:1935 \-p \$\{hls_host_port\}:8080 \$\{image\}
- init image processing

- Streaming(OBS Config)
    - streaming server: rtmp://host:port/${nginx_rtmp_application_name}
    - streaming key: ${key_use_for_pulling_stream}

### Q&A
1.Why not use high version FFmpeg?
Since version 4.0, FFmpeg dropped support for building for Windows XP

### Attention
You may get warning message as shown below, if you build image in windows docker.

- SECURITY WARNING: You are building a Docker image from Windows against a non-Windows Docker host. All files and directories added to build context will have '-rwxr-xr-x' permissions. It is recommended to double check and reset permissions for sensitive files and directories
