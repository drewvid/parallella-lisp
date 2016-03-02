(setyc)

(defun 1- (x) (- x 1))

(defun factorial (number)
  (if (eq number 0) 1
    (* number (funcall factorial (1- number)))))

(define proto-factorial 
	(lambda (fun)
    	(lambda (number)
        	(if (eq number 0)
            	1
            	(* number (funcall fun (1- number)))
            )
        )
    )
)
(funcall (proto-factorial factorial) 3)

(lambda (fun)
	(lambda (number)
    	(if (eq number 0)
        	1
      		(* number (funcall (funcall fun fun) (1- number))))
	)
)

(define meta-factorial	
	(lambda (fun)
		(lambda (number)
	  		(if (eq number 0)
	      		1
	    		(* number (funcall (funcall fun fun) (1- number)))
	    	)
	  	)
	)
)

(funcall 
	(funcall meta-factorial meta-factorial) 
	4
)

(funcall 
	(funcall 
		(lambda (fun)
         	(lambda (number)
            	(if (eq number 0) 1
                    (* number (funcall (funcall fun fun) (1- number))))
           	)
        )
        (lambda (fun)
        	(lambda (number)
            	(if (eq number 0)
                	1
                	(* number (funcall (funcall fun fun) (1- number)))
                )
            )
    	)
    )
	5
)

(defun YCombinator (f)
	(funcall 	(lambda (x)
            		(funcall f 	(lambda (y)
                            		(funcall (funcall x x) y)
                            	)
            		)
            	)
           		(lambda (x)
               		(funcall f 	(lambda (y)
                    				(funcall (funcall x x) y)
                    			)
               		)
               	)
	)
)

(funcall
	(YCombinator
		(lambda (f)
			(lambda (n)
				(if (eq n 0)
              		1
            		(* n (funcall f (1- n)))
            	)
            )
        )
    )
 	6
)

(funcall
	(YCombinator
		(lambda (f)
			(lambda (n)
                (if (< n 2)
                    n
                    (+ 	(funcall f (- n 1))
                    	(funcall f (- n 2))
                    )
            	)
            )
        )
    )
    10
)

(unsetyc)
