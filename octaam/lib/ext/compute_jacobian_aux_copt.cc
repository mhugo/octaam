#include <octave/oct.h>
#include <octave/oct-map.h>

DEFUN_DLD(compute_jacobian_aux_copt, args, nargout,
	  "")
{
  // res, tMap, Psi, Tri, Var
  // -> Jac
  octave_value_list retval;

  //  RowVector win_size = args(0).row_vector_value();
  RowVector res = args(0).row_vector_value();
  Matrix tMap = args(1).matrix_value();
  Matrix Psi = args(2).matrix_value();
  Matrix Tri = args(3).matrix_value();
  Matrix tVar = args(4).matrix_value();
  int x_res = int(res(0));
  int y_res = int(res(1));
  //  int x_size = int(win_size(0));
  //int y_size = int(win_size(1));

  Matrix JacX(x_res, y_res);
  Matrix JacY(x_res, y_res);

  for (int y=0; y < y_res; y++) {
    for (int x=0; x < x_res; x++) {
      JacX(x,y) = 0.0;
      JacY(x,y) = 0.0;
      //      std::cout << tMap(x,y) << std::endl;
    }
  }

  for (int y=0; y < y_res; y++) {
    for (int x=0; x < x_res; x++) {
      double tx = double(x);// / x_res * x_size;
      double ty = double(y);// / y_res * y_size;

      int t=int(tMap(x,y)) - 1;
      if (t == -1)
	continue;

//     for y=0:res-1
//       for x=0:res-1

// 	tx = x / res * wWidth;
// 	ty = y / res * wHeight;
// 	t = round(tMap(x+1,y+1));
// 	if t == 0
// 	  continue;
// 	end;

      for (int j=0; j < 3; j++) {

// 	for j=1:3
// 	  v0x = Psi(Tri(t,j),1);
// 	  v0y = Psi(Tri(t,j),2);
// 	  v1x = Psi(Tri(t,mod(j,3)+1),1);
// 	  v1y = Psi(Tri(t,mod(j,3)+1),2);
// 	  v2x = Psi(Tri(t,mod(j+1,3)+1),1);
// 	  v2y = Psi(Tri(t,mod(j+1,3)+1),2);


	double v0x = Psi(int(Tri(t,j))-1,0);
	double v0y = Psi(int(Tri(t,j))-1,1);
	double v1x = Psi(int(Tri(t,(j+1)%3))-1,0);
	double v1y = Psi(int(Tri(t,(j+1)%3))-1,1);
	double v2x = Psi(int(Tri(t,(j+2)%3))-1,0);
	double v2y = Psi(int(Tri(t,(j+2)%3))-1,1);

// 	  denum = (v1x - v0x) * (v2y - v0y) - (v1y - v0y) * (v2x - v0x);
// 	  a = (tx - v0x) * (v2y - v0y) - (ty - v0y) * (v2x - v0x);
// 	  b = (ty - v0y) * (v1x - v0x) - (tx - v0x) * (v1y - v0y);

	double denum = (v1x - v0x) * (v2y - v0y) - (v1y - v0y) * (v2x - v0x);
	double vtx = tVar(int(Tri(t,j))-1,0);
	double vty = tVar(int(Tri(t,j))-1,1);

	if (denum != 0.0) {
	  double a = (tx - v0x) * (v2y - v0y) - (ty - v0y) * (v2x - v0x);
	  double b = (ty - v0y) * (v1x - v0x) - (tx - v0x) * (v1y - v0y);
	  
	  // 	  vtx = tvar(Tri(t,j),1);
	  // 	  vty = tvar(Tri(t,j),2);
	  
	  
	  // 	  Jac(x+1,y+1,1,i) = Jac(x+1,y+1,1,i) + (1 - a/denum - b/denum) * vtx;
	  // 	  Jac(x+1,y+1,2,i) = Jac(x+1,y+1,2,i) + (1 - a/denum - b/denum) * vty;
	  
	  double ax = (1.0 - a/denum - b/denum) * vtx;
	  double ay = (1.0 - a/denum - b/denum) * vty;
	  JacX(x,y) += ax;
	  JacY(x,y) += ay;
	} /*else {
	  JacX(x,y) += vtx;
	  JacY(x,y) += vty;
	  }*/
// 	end;
//       end;
//     end;  
  
      }
    }
  }

//   for (int y=0; y < res; y++) {
//     for (int x=0; x < res; x++) {
//       std::cout << JacX(x,y) << std::endl;
//     }
//   }

//  std::cout << tmin << "<" << tmax << std::endl;

  retval.append(JacX);
  retval.append(JacY);
  return retval;
}
