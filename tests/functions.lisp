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
 	(if (<= start end)
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

(reverse '(1 2 3 4 5 6 7 8 9))
(append '(a b c d e f g) '(h i j k l m n))
(iota -10 10)
(flatten '((1 2 3) (4 5 6) (7 8 9) (10 11 12) (13 14 15) (16 17 18) (19 20 21)))
