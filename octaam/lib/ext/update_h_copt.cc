#include <octave/oct.h>
#include <octave/oct-map.h>

DEFUN_DLD(update_h_copt, args, nargout,
	  "")
{
  // sdi, n_geom + n_modes, oldhessian
  // -> hessian
  octave_value_list retval;

  NDArray sdi = args(0).array_value();
  int n = args(1).int_value();
  Matrix hessian = args(2).matrix_value();

  int x_res = sdi.rows();
  int y_res = sdi.cols();

  int n_total = sdi.dim3();
  //  std::cout << y_res << "x" << x_res << "x" << n_total << std::endl;

  Matrix hessian2(n_total, n_total);

  for (int i=0; i < n; i++) {
    for (int j=i; j < n_total; j++) {
      double sum = 0.0;
      for (int y=0; y < y_res; y++) {
	for (int x=0; x < x_res; x++) {
	  sum += sdi(y,x,i) * sdi(y,x,j);
	}
      }
      hessian2(i,j) = sum;
      hessian2(j,i) = sum;
    }
  }

  for (int i=n; i < n_total; i++) {
    for (int j=i; j < n_total; j++) {
      hessian2(i,j) = hessian(i,j);
      hessian2(j,i) = hessian(j,i);
    }
  }



  retval.append(hessian2);
  return retval;
}
