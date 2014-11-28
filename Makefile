all: lbfgs newton gd

gd: src/dataset.cc src/gradient_calc.cc src/linear_search.cc src/optimizer.cc src/gradient_descent.cc src/gd_train.cc src/vec_op.cc src/config.h
	/usr/bin/g++ src/dataset.cc src/gradient_calc.cc src/linear_search.cc src/optimizer.cc src/gradient_descent.cc src/gd_train.cc src/vec_op.cc -Isrc -o gd

lbfgs: src/dataset.cc src/gradient_calc.cc src/linear_search.cc src/optimizer.cc src/lbfgs.cc src/lbfgs_train.cc src/vec_op.cc src/gradient_descent.cc src/config.h
	/usr/bin/g++ src/dataset.cc src/gradient_calc.cc src/linear_search.cc src/optimizer.cc src/vec_op.cc src/lbfgs.cc src/lbfgs_train.cc src/gradient_descent.cc -Isrc -o lbfgs

newton: src/dataset.cc src/gradient_calc.cc src/linear_search.cc src/optimizer.cc src/vec_op.cc src/inexact_newton.cc src/conjugate_gradient.cc src/newton_train.cc src/hessian_vec_product.cc src/config.h
	/usr/bin/g++ src/dataset.cc src/gradient_calc.cc src/linear_search.cc src/optimizer.cc src/vec_op.cc src/inexact_newton.cc src/conjugate_gradient.cc src/newton_train.cc src/hessian_vec_product.cc -Isrc -o newton
