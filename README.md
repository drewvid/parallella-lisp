# parallella-lisp
A small implementation of LISP for the Parallella-16 board with basic, mark and sweep, garbage collection

LISP has a long distinguished history and is the second oldest high level programming language in use today. My interest in the language led me to create a version for the Parallella board. The starting point was a blog post on the parallella forum discussing how much LISP could fit on the board. In trying to answer this question for myself, I came across numerous references to John McCarthy’s paper, "A Micro-Manual for Lisp - not the whole Truth" and started there. The result is a small implementation of lisp which runs on the Parallella-16 board.

In the two latest versions of Parallella-lisp, released in early Feb 2016, the printing of input and evaluated input has been moved to the host. Also the freelist containing unallocated nodes is constructed on the host and written to the device. This makes the LISP interpreter smaller and frees up space for more functionality. The version of Parallella-lisp with Garbage Collection now contains more primities. Garbage collection is done at the end of each iteration of the REPL. Variables can be defined with ldefine (local) or define (global). Loop variables can be either local or global thus the following will work and x and y should be 100 when printed.

    (define x 100)
    (define y 100)
    (define func
        (lambda (n1 n2)
            (progn
                (ldefine y n2)
                (loop (> y -1)
                    (ldefine x n1)
                    (loop (> x -1)
                        (print y x)
                        (ldefine x (- x 1))
                    )
                    (ldefine y (- y 1))
                )
            )
        )
    )
    (func 10 10)
    (print x y)


More functionality is slowly being added as space permits.

## Building

The version with gc is in the directory plisp-gc. The lastest version without garbage collection is in the directory plisp. The original version is in the directory plisp-initial. All versions were compiled and tested with esdk.2015.1. To build LISP for the parallella in either of the above mentioned directories type:

      make build

To run LISP type:

      make run

If you would like to build lisp to run on your local machine then type:

     make

Two versions of lisp will be built which can be run by typing:

     ./fl
     ./onefile

Both onefile and fl can now be called with a filename. If no filename is given testfuncs.lisp will be interpreted. For instance, to interpret the code that will run on core 3 type in:

    ./fl code/p2.lisp

## LISP Primitives

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
    print
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

## Generating the code for each core and the examples

The LISP code that runs on each core is generated anew by gencode.py when you build or run LISP. The files generated are named p0.lisp to p15.lisp. If you want to change the filenames then you will have to edit fl-device.c and recompile. Python is also used to generate a single file which can be ported easily to another machine for testing and development. The code isn’t documented because I intend to do so with a series of blog posts towards the end of February of this year (2016).

## The test functions defined on each core

    (defun nfibs (n)
        (if (< n 2) n
            (+ 0 (nfibs (- n 1)) (nfibs (- n 2)))
        )
    )
    
    (defun member (a lat)
      (cond ((null lat) nil)
        ((eq (car lat) a) t)
        (t (member a (cdr lat)))
      )
    )
    
    (defun rember (a lat)
      (cond ((null lat) ())
          ((eq (car lat) a) (cdr lat))
          (t (cons (car lat) (rember a (cdr lat))))
      )
    )
    
    (defun eqn (n1 n2)
            (cond ((zerop n2) (zerop n1))
                ((zerop n1) nil)
                (t (eqn (sub1 n1) (sub1 n2)))
            )
    )
    
    (defun multiins (old new lat)
            (cond   ((null lat) ())
                    ((eq (car lat) old) (cons (car lat)
                        (cons new (multiins old new (cdr lat)))))
                    (t (cons (car lat) (multiins old new (cdr lat))))
            )
    )
    
    (defun mapcar (fn x)
            (cond ((null x) nil)
                  (t (cons (funcall fn (car x)) (mapcar fn (cdr x))))
            )
    )
    
    (defun maplist (fn x)
            (cond ((null x) nil)
                  (t (cons (funcall fn x) (maplist fn (cdr x))))
            )
    )
    
    (defun map (f lst)
      (if (atom lst) lst
        (cons (f (car lst)) (map f (cdr lst)))))
    
    (defun mycar (x) (car x))
    (defun mycdr (x) (cdr x))
    
    (defun subst (old new lat)
            (cond ((null lat) ())
                ((eq (car lat) old) (cons new (cdr lat)))
                (t (cons (car lat) (subst old new (cdr lat))))
            )
    )
    
    (defun length (lat)
            (cond ((null lat) 0)
                (t (add1 (length (cdr lat))))
            )
    )
    
    (defun intersect (set1 set2)
            (cond ((null set1) ())
                ((member (car set1) set2) (cons (car set1) (intersect (cdr set1) set2)))
                (t (intersect (cdr set1) set2))
            )
    )
    
    (defun rem (x d)
        (- x (* (/ x d) d)))
    
    (defun is-even (x)
        (if (= 0 (rem x 2))
            t
          nil))
    
    (defun is-odd (x)
        (if (is-even x)
            nil
          t))
    
    (defun nth (lis n)
            (if (= n 0)
                (car lis)
                (nth (cdr lis) (- n 1))
            )
    )
    
    (defun nthcdr (lst n)
      (if (<= n 0) lst
        (nthcdr (cdr lst) (- n 1))))
    
    (defun list-ref (lst n)
      (car (nthcdr lst n)))
    
    (defun filter (pred lst)
      (cond ((null lst) ())
            ((not (pred (car lst))) (filter pred (cdr lst)))
            (t (cons (car lst) (filter pred (cdr lst))))))
    
    (defun caar (x) (car (car x)))
    
    (defun assoc (item lst)
      (cond ((atom lst) ())
            ((eq (caar lst) item) (car lst))
            (t (assoc item (cdr lst)))))
    
    (defun abs (x) (if (< x 0) (- 0 x) x))
    
    (defun reverse- (zero lst)
      (if (null lst) zero
          (reverse- (cons (car lst) zero) (cdr lst))))
    
    (defun last (l)
      (cond ((atom l)        l)
            ((atom (cdr l))  l)
            (t               (last (cdr l)))))
    
    (defun reverse (lst) (reverse- () lst))
    
    (defun foldr (f zero lst)
      (if (null lst) zero
        (f (car lst) (foldr f zero (cdr lst)))))
    
    (defun foldl (f zero lst)
      (if (null lst) zero
        (foldl f (f (car lst) zero) (cdr lst))))
    
    (defun reverse2 (lst) (foldl cons nil lst))
    
    (defun append2 (a b) (foldr cons b a))
    
    (defun identity (x) x)
    (defun copy-list (l) (map identity l))
    (defun copy-tree (l)
      (if (atom l) l
        (cons (copy-tree (car l))
              (copy-tree (cdr l)))))
    
    (defun every (pred lst)
      (or (atom lst)
          (and (funcall pred (car lst))
               (every pred (cdr lst)))))
    
    (defun any (pred lst)
      (and (consp lst)
           (or (funcall pred (car lst))
               (any pred (cdr lst)))))

## The random code executed by each core - five random lines are selected

    (nfibs 10)
    (rember 'me '(please remove me))
    (multiins 'one 'two '(one three one three one three one three))
    (member 'me '(please remove me))
    (intersect '(a b c d e f) '(d e f))
    (length '(0 1 2 3 4 5 6 7 8 9))
    (subst 'me 'you '(a list with me))
    (mapcar 'atom (list 1 '(2) foo t nil))
    (mapcar 'is-prime (3 5 7 11 13 17 19 23 29 31 37 41 43 47))
    (mapcar  'mycar '( (1 2) (3 4) (5 6)))
    (reverse '(1 2 3 4 5 6 7))
    (nth '(1 2 3 4 5) 1)
    (nthcdr '(1 2 3 4 5) 2)
    (list-ref '(1 2 3 4) 1)
    (last '(1 2 3 4))
    (filter numberp '(1 2 3 a b c 4 5 6))
    (assoc 'five '((one two) (three four) (five six) (seven eight)))
    (reverse2 '(1 2 3 4 5 6 7 8 9))
    (append2 '(1 2 3 4) '(5 6 7 8 9))
    (copy-list '(a b c d e f g))
    (copy-tree '((a b c d) (e f g) h i j))
