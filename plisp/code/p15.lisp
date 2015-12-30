
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

(ff '(a b) (cdr '(c d)))
(car '(a b c))
(cons 'a '(b c))

(car xx)
(cdr '(a b c))
(equal 1 1)
(cond ((atom 'a) 'b) (t 'c))
(cond ( nil 'b) (t 'c))
(equal (car (cdr '(a b))) 'a)
