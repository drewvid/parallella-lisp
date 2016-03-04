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
(x y)