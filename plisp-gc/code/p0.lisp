
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

(member 'me '(please remove me))
(length '(0 1 2 3 4 5 6 7 8 9))
(filter numberp '(1 2 3 a b c 4 5 6))
(reverse2 '(1 2 3 4 5 6 7 8 9))
(append2 '(1 2 3 4) '(5 6 7 8 9))
(nfibs 10)
