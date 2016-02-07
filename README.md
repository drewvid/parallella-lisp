# parallella-lisp
A small implementation of LISP for the Parallella-16 board with basic, mark and sweep, garbage collection

LISP has a long distinguished history and is the second oldest high level programming language in use today. My interest in the language led me to create a version for the Parallella board. The starting point was a blog post on the parallella forum discussing how much LISP could fit on the board. In trying to answer this question for myself, I came across numerous references to John McCarthy’s paper, "A Micro-Manual for Lisp - not the whole Truth" and started there. The result is a small implementation of lisp which runs on the Parallella-16 board.

In the two latest versions of Parallella-lisp, released in early Feb 2016, the printing of input and evaluated input has been moved to the host. Also the freelist containing unallocated nodes is constructed on the host and written to the device. This makes the LISP interpreter smaller and frees up space for more functionality. Garbage collection is done at the end of each iteration of the REPL and variables are either local or global. This means that loop variables must be global for now. I'm slowly adding more functionality when space permits.

The version with gc is in the directory plisp-gc. The lastest version without garbage collection is in the directory plisp. The original version is in the directory plisp-initial.

All versions were compiled and tested with esdk.2015.1. To build LISP for the parallella in either of the above mentioned directories type:

      make build

To run LISP type:

      make run

If you would like to build lisp to run on your local machine then type:

     make

Two versions of lisp will be built which can be run by typing:

     ./fl
     ./onefile

Parallella-lisp includes the 10 primitives suggested by John McCarthy which are:

    atom
    equal
    car
    cdr
    cons
    quote
    cond
    lambda
    label
    eval

In addition the following primitives have been included on top of the original 10.

    nilp
    append
    concat
    loop
    block
    progn
    if
    define
    ldefine
    print
    terpri
    <
    >
    +
    -
    /
    *
    =

The LISP code that runs on each core is generated anew by gencode.py when you build or run LISP. The files generated are named p0.lisp to p15.lisp. If you want to change the filenames then you will have to edit fl-device.c and recompile.

Python is also used to generate a single file which can be ported easily to another machine for testing and development.

Here are some examples of the LISP code that this interpreter will execute.

    (label ff (lambda (x y)
                  (cons (car x) y)
              )
    )
    (label nfibs (lambda (n)
                     (if (< n 2) n
                         (+ 0 (nfibs (- n 1)) (nfibs (- n 2)))
                     )
                 )
    )
    (label recurse (lambda (x)
                       (cond ((nilp x) (quote stop))
                             (t (recurse (cdr x)))
                       )
                   )
    )
    (recurse (a b c d e f))
    (nfibs 10)
    (define n 5)
    (loop (> n 0)
          (block
              (print n)
              (define n (- n 1))
              (terpri 1)
          )
    )

The code isn’t documented because I intend to do so with a series of blog posts towards the end of February of this year (2016).

