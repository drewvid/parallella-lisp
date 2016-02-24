(define factorial-iter
  (lambda (n)
     (ldefine fact-iter
            (lambda (n count acc)
              (if (> count n)
                  acc
                  (fact-iter n (+ count 1) (* count acc)))))
   (fact-iter n 1 1)))

(define fibonacci
  (lambda (n)
   (ldefine fibo
          (lambda (n a b)
            (if (= n 0)
                nil
                (cons a (fibo (- n 1) b (+ a b))))))
   (fibo n 0 1)))

(label square
       (lambda (x) (* x x)))

(label cube
       (lambda (x) (* x x x)))

(define length
  (lambda (L)
    (if L
        (+ 1 (length (cdr L)))
      0)))

(fibonacci 90)
(factorial-iter 3)
(square 3)
(cube 3)
(length '(1 2 3 4))