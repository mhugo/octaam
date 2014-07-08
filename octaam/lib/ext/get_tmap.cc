#include <octave/oct.h>
#include <octave/oct-map.h>

DEFUN_DLD(get_tmap, args, nargout,
	  "")
{
  // vertices, triangles, [height width]
  
  octave_value_list retval;

  Matrix vertices = args(0).matrix_value();
  Matrix triangles = args(1).matrix_value();
  RowVector sizem = args(2).row_vector_value();
  int width = int(sizem(1));
  int height = int(sizem(0));

  Matrix J(height, width);

  for (int y=0; y < height; y++) {
    for (int x=0; x < width; x++) {
      J(y,x) = 0.0;
    }
  }

  int nbt = triangles.dim1();
  
  for (int i=0; i < nbt; i++) {
    int vx[3], vy[3];
    vx[0] = int(vertices(int(triangles(i,0))-1,0));
    vy[0] = int(vertices(int(triangles(i,0))-1,1));
    vx[1] = int(vertices(int(triangles(i,1))-1,0));
    vy[1] = int(vertices(int(triangles(i,1))-1,1));
    vx[2] = int(vertices(int(triangles(i,2))-1,0));
    vy[2] = int(vertices(int(triangles(i,2))-1,1));

    int min_idx;
    int second_idx;
    int third_idx;
    //    std::cout << vy[0] << " " << vy[1] << " " << vy[2] << std::endl;
    if ((vy[0] <= vy[1]) && (vy[1] <= vy[2])) {
      min_idx = 0;
      second_idx = 1;
      third_idx = 2;
    }
    if ((vy[2] <= vy[0]) && (vy[0] <= vy[1])) {
      min_idx = 2;
      second_idx = 0;
      third_idx = 1;
    }
    if ((vy[1] <= vy[0]) && (vy[0] <= vy[2])) {
      min_idx = 1;
      second_idx = 0;
      third_idx = 2;
    }
    if ((vy[2] <= vy[1]) && (vy[1] <= vy[0])) {
      min_idx = 2;
      second_idx = 1;
      third_idx = 0;
    }
    if ((vy[0] <= vy[2]) && (vy[2] <= vy[1])) {
      min_idx = 0;
      second_idx = 2;
      third_idx = 1;
    }
    if ((vy[1] <= vy[2]) && (vy[2] <= vy[0])) {
      min_idx = 1;
      second_idx = 2;
      third_idx = 0;
    }

    float x1,x2;
    int y;

    if (vy[min_idx] == vy[second_idx]) {
      x1 = vx[min_idx];
      x2 = vx[second_idx];
      if (vx[min_idx] > vx[second_idx]) {
	x1 = vx[second_idx];
	x2 = vx[min_idx];
      }
      y = vy[min_idx];

      goto next;
    }

    //    std::cout << min_idx << " " << second_idx << " " << third_idx << std::endl;

    int xx = vx[min_idx] + int(floor(float(vx[third_idx] - vx[min_idx]) / (vy[third_idx] - vy[min_idx]) * float(vy[second_idx] - vy[min_idx])));

    int left_idx = second_idx;
    int right_idx = third_idx;
    if (vx[second_idx] > xx) {
      left_idx = third_idx;
      right_idx = second_idx;
    }

    //    std::cout << left_idx << " " << right_idx << std::endl;

    float alpha_left, alpha_right;
    alpha_left = float(vx[left_idx] - vx[min_idx]) / (vy[left_idx] - vy[min_idx]);
    alpha_right = float(vx[right_idx] - vx[min_idx]) / (vy[right_idx] - vy[min_idx]);

    //    std::cout << alpha_left << " " << alpha_right << std::endl;

    x1 = x2 = vx[min_idx];
    for (y=vy[min_idx]; y < vy[second_idx]; y++) {
      for (int x=int(round(x1)); x < int(round(x2)); x++) {
	J(y,x) = i+1;
      }
      x1 = x1 + alpha_left;
      x2 = x2 + alpha_right;
      //      std::cout << y << "," << x1 << "-" << x2 << std::endl;
    }

  next:
    if (y != vy[third_idx]) {


      alpha_left = float(vx[third_idx] - x1) / (vy[third_idx] - y);
      alpha_right = float(vx[third_idx] - x2) / (vy[third_idx] - y);
      
      while (y <= vy[third_idx]) {
	for (int x=int(round(x1)); x < int(round(x2)); x++) {
	  J(y,x) = i+1;
	}
	x1 = x1 + alpha_left;
	x2 = x2 + alpha_right;
	y++;
      }
    }
  }

  retval.append(J);
  return retval;
}
