# ncrypt
Small utility to turn encrypt/decrypt NAOMI binary files

Wrote this quite some time back when I was playing with the Naomi system.  I had a couple of versions of this floating around, and it may be of interest to others - so putting it up on GitHub for safekeeping.

# Compilation
Pretty straight forward, you will probably need openssl-devel and zlib-devel packages installed on your system.
On a recent centos7 install, the command is:
````gcc ncrypt.c -lm /lib64/libcrypto.so  /lib64/libz.so -o ncrypt````

