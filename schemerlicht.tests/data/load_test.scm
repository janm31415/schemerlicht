(define make-cons 
  (lambda (m n) (cons m n))
)

(define combine 
  (lambda (f) 
    (lambda (x y) 
      (if (null? x) 
        (quote ()) 
        (f (list (car x) (car y)) ((combine f) (cdr x) (cdr y)))
      )
    )
  )
) 

(define zip (combine make-cons))
