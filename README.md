# parallella-lisp
A small implementation of LISP for the Parallella-16 board with basic, mark and sweep, garbage collection

LISP has a long distinguished history and is the second oldest high level programming language in use today. My interest in the language led me to create a version for the Parallella board. The starting point was a blog post on the parallella forum discussing how much LISP could fit on the board. In trying to answer this question for myself, I came across numerous references to John McCarthy’s paper, "A Micro-Manual for Lisp - not the whole Truth" and started there. The result is a small implementation of lisp which runs on the Parallella-16 board.

In the two latest versions of Parallella-lisp, released in early Feb 2016, the printing of input and evaluated input has been moved to the host. Also the freelist containing unallocated nodes is constructed on the host and written to the device. This makes the LISP interpreter smaller and frees up space for more functionality. The version of Parallella-lisp with Garbage Collection now contains more primities. Garbage collection is done at the end of each iteration of the REPL. Variables can be defined with ldefine (local) or define (global). Loop variables can be either local or global thus the following will work and x and y should be 100 when printed.

    (defun foldr (f zero lst)
      (if (null lst) zero
        (funcall f (car lst) (foldr f zero (cdr lst)))
      )
    )
    
    (defun append (a b) (foldr cons b a))
    
    (define x 2)
    (define y 2)
    (define result nil)
    (define func
        (lambda (n1 n2)
            (progn
                (ldefine y n2)
                (loop (> y -1)
                    (ldefine x n1)
                    (loop (> x -1)
                        (define result (append result ((x y))))
                        (ldefine x (- x 1))
                    )
                    (ldefine y (- y 1))
                )
                nil
            )
        )
    )
    (func x y)
    result
    (block x y)

More functionality is slowly being added as space permits.

## Building and Running

The version with gc is in the directory plisp-gc. The latest version without garbage collection is in the directory plisp. The original version is in the directory plisp-initial. All versions were compiled and tested with esdk.2015.1. To build LISP for the parallella in either of the above-mentioned directories type:

      make build

To run LISP type:

      make run

If you would like to build lisp to run on your local machine, then type:

     make

Two versions of lisp will be built which can be run by typing:

     ./fl
     ./onefile

Both onefile and fl can now be called with a filename. If no filename is given, testfuncs.lisp will be interpreted. For instance, to interpret the code that will run on core 3, type in:

    ./fl code/p2.lisp

Adding more tests to testfuncs.lisp in the folder plisp-gc is not a good idea because there is a chance you will run out of memory. The shell script plisp is also provided to execute the same lisp code on every core, try:

    ./plisp prog.lisp

## LISP Primitives

Parallella-lisp includes the 10 primitives suggested by John McCarthy, which are:

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

In addition the following primitives are included on top of the original 10.

    nilp
    append
    concat
    loop
    block
    progn
    if
    define
    ldefine
    terpri
    <
    >
    +
    -
    /
    *
    =
    <=
    >=
    defun
    funcall
    null
    consp
    times
    zerop
    greaterp
    lessp
    sub1
    add1
    numberp
    eq
    and
    or
    not
    setflag

## The Y-Combinator

The tests directory now contains the code for the derivation of the y-combinator. For things to work, I added one c function:

    node *bind_variables(node *expr, node *env)

This function is called by a modified evlambda. The Y-Combinator implemented in plisp is very similar to the emacs LISP version and you can read about how this function is derived in this blog post [Y-Combinator in emacs LISP](http://cestdiego.github.io/blog/2015/10/12/y-combinator-in-emacs-lisp/) (this post is no longer available).

The plisp version along with an example looks like this:

    (setyc)

    (defun YCombinator (f)
    	(funcall 	(lambda (x)
                		(funcall f 	(lambda (y)
                                		(funcall (funcall x x) y)
                                	)
                		)
                	)
               		(lambda (x)
                   		(funcall f 	(lambda (y)
                        				(funcall (funcall x x) y)
                        			)
                   		)
                   	)
    	)
    )
    
    (funcall
    	(YCombinator
    		(lambda (f)
    			(lambda (n)
    				(if (eq n 0)
                  		1
                		(* n (funcall f (1- n)))
                	)
                )
            )
        )
     	6
    )
    
    (unsetyc)

The file, ycomb.lisp, cotains the the above code along with a modified version of the code listed in the blog post on how to derive the function to work with emacs LISP. To see everything working, do this in the plisp or plisp-gc directory:

    make
    ./fl ../tests/ycomb.lisp
    or
    make build
    ./plisp ../tests/ycomb.lisp

./fl runs the local version and ./plisp runs the code on the parallella 16.

## Generating the code for each core and the examples

The LISP code that runs on each core is generated anew by gencode.py when you build or run LISP. The files generated are named p0.lisp to p15.lisp. If you want to change the filenames then you will have to edit fl-device.c and recompile. Python is also used to generate a single file which can be ported easily to another machine for testing and development. The code isn’t documented because I intend to do so with a series of blog posts towards the end of February of this year (2016).

