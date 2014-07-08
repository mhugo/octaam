#include <octave/oct.h>
#include <octave/oct-map.h>

DEFUN_DLD(warp_2d_copt, args, nargout,
	  "")
{
  // input image, warp coeffs, triangles, trimap, [height width]
  
  octave_value_list retval;

  Matrix input = args(0).matrix_value();
  Matrix wcoeffs = args(1).matrix_value();
  Matrix triangles = args(2).matrix_value();
  Matrix tMap = args(3).matrix_value();
  RowVector sizem = args(4).row_vector_value();
  int width = int(sizem(1));
  int height = int(sizem(0));

  Matrix J(height, width);

  for (int y=0; y < height; y++) {
    for (int x=0; x < width; x++) {
      J(y,x) = 0.0;
    }
  }

  int w = input.dim2();
  int h = input.dim1();

  for (int y=0; y < height; y++) {
    for (int x=0; x < width; x++) {

      int t=int(tMap(y,x)) - 1;
      if (t == -1)
	continue;

      double a1 = wcoeffs(t,0);
      double a2 = wcoeffs(t,1);
      double a3 = wcoeffs(t,2);
      double a4 = wcoeffs(t,3);
      double a5 = wcoeffs(t,4);
      double a6 = wcoeffs(t,5);
  
      int cx = int(a1 + a2 * x + a3 * y);
      int cy = int(a4 + a5 * x + a6 * y);

      //      std::cout << cx << "," << cy << std::endl;
      if ((cx >=0) && (cy >= 0) && (cx < w) && (cy < h)) {
	J(y,x) = input(cy,cx);
      }

    }
  }

  retval.append(J);
  return retval;
}
