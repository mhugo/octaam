1;

oVersion = str2double(substr(OCTAVE_VERSION,1,3));

if oVersion < 2.9
  LOADPATH = ".:./lib/:./lib/ext/::";
else
  addpath("lib/");
  addpath("lib/ext/");
end;
%__gnuplot_set__ mouse;
more off;
%figure(0);

colormap(gray);

function S=rescale_shape(S, w, h)
  min_x = min(S(:,1));
  max_x = max(S(:,1));
  min_y = min(S(:,2));
  max_y = max(S(:,2));
  S(:,1) = (S(:,1) - min_x) ./ (max_x - min_x) .* w;
  S(:,2) = (S(:,2) - min_y) ./ (max_y - min_y) .* h;
end

function S=vec_to_shape(v)
  l=length(v) / 2;
  S=reshape(v,l,2);
end

% gradient
function [GX,GY]=im_grad(I)
    w=size(I,2);
    h=size(I,1);
    GX=zeros(size(I));
    GY=zeros(size(I));
    for x=2:w-1
      GX(:,x) = (I(:,x+1) - I(:,x-1)) ./ 2;
    end;
    for y=2:h-1
      GY(y,:) = (I(y+1,:) - I(y-1,:)) ./ 2;
    end
  [GX,GY] = gradient(I);
end

function S=vec_to_shape3(v)
  l=length(v) / 2;
  S = [reshape(v,l,2), zeros(l,1)];
end

function display_2d_shape(s, win_size, triangles)
  %%% input
  %%% s is 0-based

%  s(:,1) = s(:,1) ./ win_size(1) .* 2 - 1;
%  s(:,2) = s(:,2) ./ win_size(2) .* 2 - 1;

  if nargin >= 3
    tri = triangles;
  else
    tri = delaunay(s(:,1), s(:,2));
  end

  for i=1:size(tri,1)
	  P1=s(tri(i,1),:);
	  P2=s(tri(i,2),:);
	  P3=s(tri(i,3),:);

	  PP=[P1',P2', P3', P1'];
	  line(PP(1,:), PP(2,:));
  end
  

end


function z = triangle_dir(vx,vy)
nx = norm(vx);
ny = norm(vy);
alpha = acos((vx * vy' ) / (nx * ny));
z = (sin(alpha) * nx * ny) > 0;
end

% A is a system of free vectors
% B is the corresponding orthonormal basis
function B=gram_schmidt(A)
  n = size(A,2);
  B=zeros(size(A));

  for i=1:n
    b = A(:,i);
    a = zeros(size(B,1),1);
    if i>1
      for j=1:i-1
	a = a - B(:,j)*(B(:,j)'*A(:,i));
      end
    end
    b = b + a;
    sum(b)

    B(:,i) = b ./ norm(b);
  end
end


function new_s = warp_composition(base, delta, compose_to, triangles, sur_triangles)
  new_s = zeros(size(base));
  for i=1:size(base,1)
    tx = base(i,1) + delta(i,1);
    ty = base(i,2) + delta(i,2);
    
    tri=nth(sur_triangles,i);
    
    if (length(tri) == 0)
      continue;
    endif
    
    v=zeros(length(tri),2);
    for j=1:length(tri)
      trij=tri(j);
      v0x=base(triangles(trij,1),1);
      v0y=base(triangles(trij,1),2);
      v1x=base(triangles(trij,2),1);
      v1y=base(triangles(trij,2),2);
      v2x=base(triangles(trij,3),1);
      v2y=base(triangles(trij,3),2);
      
      vt0=compose_to(triangles(trij,1),1:2);
      vt1=compose_to(triangles(trij,2),1:2);
      vt2=compose_to(triangles(trij,3),1:2);
      
      denum = (v1x - v0x) * (v2y - v0y) - (v1y - v0y) * (v2x - v0x);
      
      a = ((tx - v0x) * (v2y - v0y) - (ty - v0y) * (v2x - v0x)) / denum;
      b = ((ty - v0y) * (v1x - v0x) - (tx - v0x) * (v1y - v0y)) / denum;
      
      v(j,:) = vt0 + a * (vt1 - vt0) + b * (vt2 - vt0);
    end
    new_s(i,:) = mean(v);
    
  end
  
end

function display_2d_points(s, wsize, weight)
  if nargin < 3
    weight = ones(size(s,1)*2,1);
  end

  cs = 5;
  for i=1:size(s,1)
    l1 = [s(i,1) - cs, s(i,2); s(i,1) + cs, s(i,2)];
    l2 = [s(i,1), s(i,2)-cs; s(i,1), s(i,2)+cs];
    if weight(i) > 1
      weight(i) = 1;
    end
    c = [0; 0; weight(i);];

    line(l1(:,1), l1(:,2), "color", c);
    line(l2(:,1), l2(:,2), "color", c);
  end

end



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%       OBSOLETE Functions
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function display_shape_full(s)
%  gcClear;
  s=vec_to_shape3(vec(s));
  s=rescale_shape(s, 2, 2) - 1;
  s(:,2) = - s(:,2);

  obj.vertices = s;
  obj.base = s;
  obj.triangles=delaunay(s(:,1), s(:,2)) - 1;
  obj.projection=eye(4);

  gcDisplayObject(obj,0);
  gcRefresh;
end

function display_shape_full_3d(s,T)
  gcClear;
  s(:,1) = (s(:,1) - min(s(:,1))) ./ (max(s(:,1)) - min(s(:,1))) .* 2 - 1;
  s(:,2) = (s(:,2) - min(s(:,2))) ./ (max(s(:,2)) - min(s(:,2))) .* 2 - 1;
  s(:,3) = (s(:,3) - min(s(:,3))) ./ (max(s(:,3)) - min(s(:,3))) .* 2 - 1;
  s(:,2) = - s(:,2);

  obj.vertices = s;
  obj.base = s;
  obj.triangles=T - 1;
  obj.projection=eye(4);

  gcDisplayObject(obj,0);
  gcRefresh;
end

function A = write_image(im, fn)
  minI = min(vec(im));
  maxI = max(vec(im));

  A = (im - minI) ./ (maxI - minI) .* 255;
  imwrite(fn, A);
end