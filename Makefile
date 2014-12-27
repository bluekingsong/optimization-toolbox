all: lbfgs newton gd sbfgs utest tron sgd

gd: src/dataset.cc src/gradient_calc.cc src/linear_search.cc src/optimizer.cc src/gradient_descent.cc src/gd_train.cc src/vec_op.cc src/config.h src/cpp_common.cc
	/usr/bin/g++ src/dataset.cc src/gradient_calc.cc src/linear_search.cc src/optimizer.cc src/gradient_descent.cc src/gd_train.cc src/vec_op.cc src/cpp_common.cc -Isrc -o gd

sgd: src/dataset.cc src/gradient_calc.cc  src/optimizer.cc src/stochastic_gradient_descent.cc src/sgd_train.cc src/vec_op.cc src/config.h src/cpp_common.cc
	/usr/bin/g++  src/dataset.cc src/gradient_calc.cc  src/optimizer.cc src/stochastic_gradient_descent.cc src/sgd_train.cc src/vec_op.cc src/cpp_common.cc -Isrc -o sgd

lbfgs: src/dataset.cc src/gradient_calc.cc src/linear_search.cc src/optimizer.cc src/lbfgs.cc src/lbfgs_train.cc src/vec_op.cc src/gradient_descent.cc src/config.h src/cpp_common.cc
	/usr/bin/g++ src/dataset.cc src/gradient_calc.cc src/linear_search.cc src/optimizer.cc src/vec_op.cc src/lbfgs.cc src/lbfgs_train.cc src/gradient_descent.cc src/cpp_common.cc -Isrc -o lbfgs

newton: src/dataset.cc src/gradient_calc.cc src/linear_search.cc src/optimizer.cc src/vec_op.cc src/inexact_newton.cc src/conjugate_gradient.cc src/newton_train.cc src/hessian_vec_product.cc src/config.h src/cpp_common.cc
	/usr/bin/g++ src/dataset.cc src/gradient_calc.cc src/linear_search.cc src/optimizer.cc src/vec_op.cc src/inexact_newton.cc src/conjugate_gradient.cc src/newton_train.cc src/hessian_vec_product.cc src/cpp_common.cc -Isrc -o newton

sbfgs: src/dataset.cc src/gradient_calc.cc src/linear_search.cc src/optimizer.cc src/vec_op.cc src/conjugate_gradient.cc src/hessian_vec_product.cc src/config.h src/sample_lbfgs.cc src/lbfgs.cc src/sample_lbfgs_train.cc src/gradient_descent.cc src/cpp_common.cc
	/usr/bin/g++ src/dataset.cc src/gradient_calc.cc src/linear_search.cc src/optimizer.cc src/vec_op.cc src/conjugate_gradient.cc src/sample_lbfgs.cc src/sample_lbfgs_train.cc src/hessian_vec_product.cc src/lbfgs.cc src/gradient_descent.cc src/cpp_common.cc -Isrc -o sbfgs

utest: src/conjugate_gradient.cc src/gradient_descent.cc src/optimizer.cc src/vec_op.cc src/dataset.cc src/hessian_vec_product.cc src/linear_search.cc src/sample_lbfgs.cc src/inexact_newton.cc src/mat_vec_product.cc src/gradient_calc.cc src/lbfgs.cc src/unittest.cc src/cpp_common.cc
	/usr/bin/g++ src/conjugate_gradient.cc src/gradient_descent.cc src/optimizer.cc src/vec_op.cc src/dataset.cc src/hessian_vec_product.cc src/linear_search.cc src/sample_lbfgs.cc src/inexact_newton.cc src/mat_vec_product.cc src/gradient_calc.cc src/lbfgs.cc src/unittest.cc src/cpp_common.cc -Isrc -o utest

tron: src/dataset.cc src/gradient_calc.cc src/linear_search.cc src/optimizer.cc src/vec_op.cc src/inexact_newton.cc src/conjugate_gradient.cc src/tron_train.cc src/hessian_vec_product.cc src/config.h src/cpp_common.cc src/trust_region.cc
	/usr/bin/g++ src/dataset.cc src/gradient_calc.cc src/linear_search.cc src/optimizer.cc src/vec_op.cc src/inexact_newton.cc src/conjugate_gradient.cc src/tron_train.cc src/hessian_vec_product.cc src/cpp_common.cc src/trust_region.cc -Isrc -o tron
