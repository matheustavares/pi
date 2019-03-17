
# Parallel approximation of PI in C

Here is a parallel C code that approximates PI using riemann integration.
We do that calculating an aproximation for the integral of
`f(x) = sqrt(1 - x^2)`, with x in `[0, 1]`, which corresponds to one quarter of
a unitary circle's area. As we know this value can also be calculated as
`(pi * radius^2) / 4`, which, in this case, is `pi / 4`. So to get a PI
approximation we just have to multiply the value of the integration by 4.

The goal is not to have the best PI approximation strategy, but to show a simple
and annotated pthread code for those who are learning parallel programming in
C with pthreads.

### To compile

You must have `gcc` and `make` installed. Than, run:
```
    $ make
```
(tested on linux only)

### To run

```
    $ time ./pi <num_threads> <integration_divisions>
```
Where `num_threads` is the desired number of threads to be used and
`integration_divisions` is the number of partitions to be made for the riemann
integration.

#### Tip:

Run with a high value of `integration_divisions` (say, 1.e9), and compare the
time when you change `num_threads` from 1 to 4 (or 8, depending on your CPU).

You can then calculate the speedup with `n` threads, which is the result of
`sequential_time / time_with_n_threads`. This value will show the effectiveness
of the parallel version against the sequential and the higher it is, the better.
