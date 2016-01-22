import random

setup="""
(label ff (lambda (x y)
              (cons (car x) y)
          )
)
(label xx '(a b))
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
(label ll '())
ll
(label lll '((c)a(b)))
lll
(car lll)
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
(< 1 2) 
(< 2 1) 
(> 1 2) 
(> 2 1) 
(- 0 1 2) 
(* 1 2 3) 
(/ 10 5) 
(+ 1 2 3) 
(= 100 100)
"""

code = """
'a
'(a b c)
(car '(a b c))
(cdr '(a b c))
(cons 'a '(b c))
(equal (car '(a b)) 'a)
(equal (car (cdr '(a b))) 'a)
(atom 'a)
(cond ((atom 'a) 'b) (t 'c))
(cond ( nil (quote b)) (t (quote c)))
(cond ( nil 'b) (t 'c))
((lambda (x y) (cons (car x) y)) '(a b) (cdr '(c d)))
(equal nil nil)
(equal t t)
(equal 1 1)
(ff '(a b) (cdr '(c d)))
(cdr xx)
(car xx)
"""

if __name__ == '__main__':

    lines = code.split("\n");

    for i in range(16):
        sample = random.sample(lines, 10)
        fp = open("p" + str(i) + ".lisp", "w")
        fp.write(setup + "\n")
        for line in sample:
            fp.write(line + "\n")
        fp.close()
