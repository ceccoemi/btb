# btb - BitTorrent Bits

Minimial BitTorrent command line client written in pure C with minimal dependencies.

## Dependencies

* OpenSSL
* pthread
* curl
* valgrind

## How to compile, test and run

To compile and run the tests (with valgrind checks), `cd` into the root directory of the project and run

    make

To download a torrent file, run

    btb <file .torrent>

## References

* [Building a BitTorrent client from the ground up in Go](https://blog.jse.li/posts/torrent/)
* [How to Write a Bittorrent Client](http://www.kristenwidman.com/blog/33/how-to-write-a-bittorrent-client-part-1/)
