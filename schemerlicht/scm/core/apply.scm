(define apply %apply) ; We want to treat apply always as a primitive object call. This triggers the correct cps conversion for when the argument of apply is not a primitive, but a closure.