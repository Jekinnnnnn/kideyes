### Wiki
1. Why server always play a regular length video?
    - PlayList depends on hls_playlist_length which is in hls.conf
    - Kideyes just replay latest stream, even using event hls_type;redundant ts files will process by image processing module.

