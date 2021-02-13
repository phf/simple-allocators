# Simple Allocators

If you're curious about simple memory allocation schemes in C, maybe you'll be
able to learn a thing or two here.

## Limitations

I've tried to keep the code for these allocators as simple as possible, so if
you want to use any of them "in production" you'll have to add a few more bells
and whistles.
For starters, real programs will require more than just one object size, so you
will have to generalize the code for *cached* and *pool* allocators to support
"allocator objects" of some kind.
The *arena* allocator already supports "arbitrary" object sizes, but it has a
fixed maximum capacity that may not be appropriate for all programs; code to
"grow" the arena when necessary might be a good addition.
Nothing here is thread-safe either, so you may need to throw in a lock or three.

## License

I very much doubt that you can use the code "as is" for a real system, but feel
free to take it as a starting point if that seems helpful. Since it's under a
0-clause BSD license, which essentially makes it public domain, you're welcome
to do as you please. If the code was helpful to you I'd *appreciate* an email,
but it's certainly not required.
