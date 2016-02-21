
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
      (progn 
          (print n)
          (define n (- n 1))
          (terpri 1)
      )
)
(ff '(a b) (cdr '(c d)))
((lambda (x y) (cons (car x) y)) '(a b) (cdr '(c d)))
(atom "a")
(atom 1)
(atom nil)
(equal "a" "a")
(equal "a" "b")
(equal "1" 1)
(car '(a b c))
(cdr '(a b c))
(cons "a" (cons "b" nil))
(quote (car '(a b c)))
(cond ((= 1 1) (quote stop))
      (t (quote hi ))
)
(label n 1)
(eval 'n)
(nilp  nil)
(nilp  "a")
(append  '(1 2 3) '(4 5 6))
(concat  '(1 2) '(3 4))
(block (+ 1 2) (+ 2 3) (+ 3 4)) 
(progn (+ 1 2) (+ 2 3) (+ 3 4)) 
(if (= 1 1) t nil) 
(if (= 1 2) t nil) 
(print  '(1 2 3 4))
(terpri  1)
(< 2 1)
(< 1 2)
(> 1 2) 
(> 2 1) 
(+ 1 1) 
(- 4 2) 
(/ 10 5)
(* 3 3)
(= 2 3)
(= 2 2)

(ff '(a b) (cdr '(c d)))
(cdr xx)
(cond ( nil (quote b)) (t (quote c)))
(equal (car (cdr '(a b))) 'a)
'a
(equal nil nil)
(cond ((atom 'a) 'b) (t 'c))
(atom 'a)
(cdr '(a b c))
(cond ( nil 'b) (t 'c))
