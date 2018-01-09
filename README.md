# amp
A tool that reads data from standard input (stdin) and writes to various output streams.

## Finally!

I herewith proudly announce version 1.0 of `amp`!

Major and minor bugs have been fixed, new features have been implemented and the code has been totally revised.

If you're still using the old version of `amp` (prior to 1.0), please download and compile the latest version.

## How to compile

```bash
gcc -Wall -o amp amp.h main.c amp.c args.c llist.c log.c utils.c -lm
```

## Examples

For some of the following examples, you need an arbitrary file. You can create a simple text file like so:

```bash
echo Hello World! >hello
```

Now let's dive into some examples...

---

Let's assume, you want to copy that file to `hello.copy`:

```bash
amp <hello >hello.copy
```

---

As you know, you could also do that with `cp`, but what if you want to copy that file to multiple files? Take a look at this example:

```bash
amp <hello 1>hello.copy1 3>hello.copy2 4>hello.copy3 5>/tmp/hello.copy4
```

Here we take `hello` as our input file and copy the content to the files `hello.copy1`, `hello.copy2`, `hello.copy3` and `/tmp/hello.copy4` using the streams 1 (stdout), 3, 4 and 5.

If you're wondering why stream 2 is omitted: stream 2 is `stderr` by default and is used to write log messages (infos, warnings, errors, status, etc.), so it cannot be used as an output stream (although it can also be redirected to a file or e.g. `/dev/null`).

---

So much for basic examples. Let's take full advantage of a powerful shell like `bash` (or maybe also other shells):

```bash
amp <hello >hello.copy 3> >(md5sum >hello.md5) 4> >(sha1sum >hello.sha1)
```

In this example, `amp` reads the file `hello`, writes it to `stdout` (which is then redirected to `hello.copy`) and uses process substitution to *pipe* the content into `md5sum` and `sha1sum` to compute the checksums and redirect their output to the corresponding checksum files.

---

Similar to the previous example, the output of `amp` can also be piped into another tool (like `xz` to compress the data):

```bash
amp <hello 3> >(md5sum >hello.md5) 4> >(sha1sum >hello.sha1) | xz -c >hello.xz
```

---

And, of course, you can build a chain of tools by piping the result into the next tool, like so:

```bash
tail -n 100 /var/log/syslog | grep -i error | amp 3> >(md5sum >log.md5) 4> >(sha1sum >log.sha1) | xz -c >log.xz
```

Reads the last 100 lines from `/var/log/syslog` including only messages containing the word "error", pipes that into `amp` (while computing various checksums), and in the end compresses the final result using `xz`.

---

All the previous examples were working on finite input (such as files). But `amp` also works with infinite input and finite output streams:

```bash
amp --status 10s </dev/zero >/dev/sdz
```

Here, `amp` reads from the character device `/dev/zero` and writes that data to `/dev/sdz` until the latter is full, showing a status every 10 seconds on `stderr`.

**Warning!** This example will completely overwrite all data on `/dev/sdz` (if that device exists), so please use with care!

---

Of course, you can use `amp` to wipe your data from multiple devices all at once:

```bash
amp --status 10s --no-stdout </dev/zero 3>/dev/sdx 4>/dev/sdy 5>/dev/sdz
```

The option `--no-stdout` tells `amp` not to write to `stdout`.

---

Same example as above, but instead of zeros, we write random data to wipe our devices:

```bash
amp --status 10s --no-stdout </dev/urandom 3>/dev/sdx 4>/dev/sdy 5>/dev/sdz
```

---

To make the wiping even faster, you can use a larger buffer of 4 megabytes (instead of the default 4096 bytes):

```bash
amp --buffer 4M --status 10s --no-stdout </dev/zero 3>/dev/sdx 4>/dev/sdy 5>/dev/sdz
```

---

You can limit the maximum number of bytes to read by setting the count argument:

```bash
amp --count 4M --status 1M </dev/urandom >random.dat
```

This will read 4 megabytes of random data and write that `random.dat`, showing a status every 1 megabyte.

---

Or instead of a maximum number of bytes, you can limit the number of lines to read:

```bash
amp --lines 10 </var/log/syslog >log_10.dat
```

This will read 10 lines from `/var/log/syslog` and write that to `log_10.dat` (showing no status).
