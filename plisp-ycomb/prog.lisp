(define testit (lambda ()
    (print test))
)

(defun foldr (f zero lst)
  (if (null lst) zero
    (funcall f (car lst) (foldr f zero (cdr lst)))
  )
)

(defun foldl (f zero lst)
  (if (null lst) zero
    (foldl f (funcall f (car lst) zero) (cdr lst))))

(defun reverse (lst) (foldl cons nil lst))

(defun append (a b) (foldr cons b a))

(defun iota (start end)
    (if (< start end)
        (cons start (iota (+ 1 start) end))
        nil
    )
)

(defun flatten (lst)
  (if (null lst) nil
      (if (consp (car lst))
          (append (flatten (car lst))
                  (flatten (cdr lst)))
          (cons (car lst)
                (flatten (cdr lst))))))

(define length
  (lambda (L)
    (if L
        (+ 1 (length (cdr L)))
      0)))

(define factorial-iter
  (lambda (n)
     (define fact-iter
            (lambda (n count acc)
              (if (> count n)
                  acc
                  (fact-iter n (+ count 1) (* count acc)))))
   (fact-iter n 1 1)))

(label fibonacci
  (lambda (n)
   (label fibo
          (lambda (n a b)
            (if (= n 0)
                nil
                (cons a (fibo (- n 1) b (+ a b))))))
   (fibo n 0 1)))

(label square
       (lambda (x) (* x x)))

(label cube
       (lambda (x) (* x x x)))

(testit)

(iota 0 10)

(define l '((1 2 3) (4 5 6) (7 8 9) (10 11 12) (13 14 15) (16 17 18) (19 20 21)))
(reverse l)
(flatten l)

(length '(1 2 3 4))
(square 3)
(cube 3)
(fibonacci 11)
(factorial-iter 3)

(print hello world from core (id))
