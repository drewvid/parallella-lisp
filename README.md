# parallella-lisp
A small implementation of LISP for the Parallella-16 board

LISP has a long distinguished history and is the second oldest high level programming language in use today. My interest in this language led me to create a version for the Parallella board. The starting point was a blog post on the parallella forum discussing how much LISP could fit on the board. In trying to answer this question for myself, I came across numerous references to John McCarthy’s paper, "A Micro-Manual for Lisp - not the whole Truth" and therefore stsrted there. The result is a small implementation of lisp which is void of any garbage collection but does run on the Parallella-16 board. 

To build LISP for the parallella type:

      make build

in the pcode directory. To run LISP type:

      make run

in the same directory. If you would like to build lisp to run on your local machine then just type:

     make

Two versions of lisp will be built which can be run by typing:

     ./fl
     ./onefile

This version of LISP has no garbage collector and just enough primitives to get something going.

The 10 primitive suggested by John McCarthy's paper are:

	atom
	eq
	car
	cdr
	cons
	quote
	cond
	lambda
	label
	eval

The following primitives have been included on top of the original 10.

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

The LISP code that runs on each core is generated anew by gencode.py when you build or run LISP. Python is also used to generate a single file which can be ported easily to another machine for testing and development.

The code isn’t documented because I intend to do that with a series of blog posts over the next few months or so.
