(defun caar (lst) (car (car lst)))
(defun cddr (lst) (cdr (cdr lst)))
(defun cadr (lst) (car (cdr lst)))
(defun cdar (lst) (cdr (car lst)))
(defun cadar (lst) (car (cdr (car lst))))
(defun caddr (lst) (car (cdr (cdr lst))))
(defun caddar (lst) (car (cdr (cdr (car lst)))))

(defun assoc (var lst)
  (cond ((eq (caar lst) var) (cadar lst))
        (t (assoc var (cdr lst)))))

(defun pair (x y)
  (cons x (cons y nil)))

(defun zip (x y)
  (cond ((and (null x) (null y)) nil)
        ((and (not (atom x)) (not (atom y)))
         (cons (pair (car x) (car y))
               (zip (cdr x) (cdr y))))))

(zip '(1 2 3) '(4 5 6))

(defun append (x y)
    (cond ((null x) y)
          (t (cons (car x) (append (cdr x) y)))))

(append '(1 2 3) '(4 5 6))

(defun and (x y)
    (cond (x (cond (y t) (t nil)))
          (t nil)))

(defun or (x y)
    (cond (x t) 
          (t (cond (y t) (t nil)))))

(defun not (x)
    (cond (x nil)
          (t t)))

(defun eval (exp env)
  (cond
    ((atom exp) (assoc exp env))
    ((atom (car exp))
      (cond
         ((eq (car exp) 'quote) (cadr exp))
         ((eq (car exp) 'atom)  (atom (eval (cadr exp) env)))
         ((eq (car exp) 'eq)    (eq   (eval (cadr exp) env)
                                      (eval (caddr exp) env)))
         ((eq (car exp) 'car)   (car  (eval (cadr exp) env)))
         ((eq (car exp) 'cdr)   (cdr  (eval (cadr exp) env)))
         ((eq (car exp) 'cons)  (cons (eval (cadr exp) env)
                                      (eval (caddr exp) env)))
         ((eq (car exp) 'cond)  (evcon (cdr exp) env))
         ((eq (car exp) 'print)  (print (eval (cadr exp) env)))
         (t (eval (cons (assoc (car exp) env) (cdr exp)) env))
      )
    )
    ((eq (caar exp) 'label) (bind exp env))
    ((eq (caar exp) 'lambda) (evlambda exp env))
  )
)

(defun evcon (c env)
  (cond ((eval (caar c) env)
         (eval (cadar c) env))
        (t (evcon (cdr c) env))))

(defun evlis (m env)
  (cond ((null m) nil)
        (t (cons (eval  (car m) env)
                  (evlis (cdr m) env)))))

(defun evlambda (exp env)
  (eval (caddar exp)
        (append (zip (cadar exp) (evlis (cdr exp) env))
                 env))
)

(defun bind (exp env)
  (eval (cons (caddar exp) (cdr exp))
        (cons (pair (cadar exp) (car exp)) env))
)

(eval '(cons x '(b c)) '((x a) (y b)))
(eval '(f '(bar baz)) '((f (lambda (x) (cons 'foo x)))))
(eval '(f '(one two)) '((f (lambda (x) (print x)))))
