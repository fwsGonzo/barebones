## Machine with shared library loader

This tiny machine allows you to call into a function in a shared library, and just enough to make it usable. No constructors or destructors.

## Questions

- Why is the init function using the C calling convention?
  - It's to make it easier to look up the functions name, as it won't be mangled.
- Can you look up functions from the shared library?
  - No, and that is ideally something done at loading time where functions are resolved automatically, which is something called dynamic linking.
