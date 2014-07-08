1;

% inputs :
%   input_image
%   current_s    = current shape estimation
%   current_l    = current appearance estimation
%
% outputs :
%   new_s = updated shape eestimation
%   new_l = updates appearance estimation

printf("Projection ...\n");

warp_c = warp_coeffs(psi_p, current_s, data.triangles);

% sum(vec(warp_c))

proj = warp_2d(input_image, warp_c, data.triangles, tmap, [y_res x_res]);

if size(current_l,1) > 0
  v_app = tex_space.psi + tex_space.v * current_l;
else
  v_app = tex_space.psi;
end

app = reshape(v_app, y_res, x_res);

proj_n = norm(vec(proj));
error_p = proj - app .* proj_n;

error = proj - app;

error_cum(:,:,current_it) = error_p;

if debug == 1
  figure(2);
colormap(gray);
  imagesc(app);

  figure(3);
colormap(gray);
%  imagesc(proj);
  errim=min(error.*error, ones(y_res,x_res).*1000) ./ 1000 .* 255;
  imagesc(errim);
end

%
% SDI updating
% Hessian updating
% dp computation
% delta_p, delta_q and delta_lambda
%
load_file = strcat("./trackers/", tracker_name,"/iteration_core.m");
if exist(load_file, "file")
  source(load_file)
end

if size(delta_p,1) > 0
  delta_s0 = - shape_space.v * delta_p - v_geom * delta_q;
else
  delta_s0 = - v_geom * delta_q;
end
delta_s0_2d = vec_to_shape(delta_s0);



printf("Composition ...\n");

new_s = warp_composition(psi_p, delta_s0_2d, current_s, triangles, sur_triangles);

printf("Projection ...\n");

n=vec(new_s);

new_p = inv_base*(n - shape_space.psi);
new_q = new_p(1:n_geom);
new_p = new_p(n_geom+1:n_geom+n_modes);


if size(new_p,1) > 0
  new_s=vec_to_shape3((shape_space.psi + shape_space.v*new_p + v_geom*new_q));
else
  new_s=vec_to_shape3((shape_space.psi + v_geom*new_q));
end

if exist("outlier_map", "var")

  printf("Weighted shape reprojection\n");

  ## weighted reprojection
  
  n_s = vec_to_shape(shape_space.v * new_p + shape_space.psi);
  
%  outlier_map = outlier_map > 0;
  point_weight = zeros(size(n_s,1),1);
  for i=1:size(n_s,1)
    point_weight(i) = sum(vec(outlier_map .* dxt(:,:,i))) ./ sum(vec(dxt(:,:,i)));
  end
  
  W = [point_weight; point_weight];
  
%  W = W ./ sum(W) .* length(W);
  
%  Z = diag(W) * shape_space.v;
%  PP = inv(Z'*Z) * Z';
%  n_vs = shape_space.psi + shape_space.v * (PP * ((vec(n_s) - shape_space.psi) .* W)) + v_geom * new_q;
%  new_s = vec_to_shape3(n_vs);
end

