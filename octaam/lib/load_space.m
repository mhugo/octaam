if exist("shape_space", "var")
  printf("\t[MEM]\n");
else
  printf("\n");
  nb_samples = size(data.patches, 3);

  vec_tex = reshape(data.patches, x_res*y_res, nb_samples);
  
  if shape_dim
    [shape_space.v shape_space.l, nb, var] = compute_pca(data.shapes_n, vec(data.psi_n), shape_dim);
  else
    shape_space.v = [];
    shape_space.l = [];
    nb = 0;
    var = 0;
  end
  printf("\tShape space\t\t[%d / %3.2f%%]\n",nb, var*100);

  
  if tex_dim
    [tex_space.v tex_space.l, nb, var] = compute_pca(vec_tex, vec(data.mean_tex), tex_dim);

%    t_gain = vec(data.mean_tex);
%    t_gain = t_gain ./ norm(t_gain);

%    t_gain1 = vec([data.mean_tex(:,1:x_res/2), zeros(y_res, x_res / 2)]);
%    t_gain1 = t_gain1 ./ norm(t_gain1);
%    t_gain2 = vec([zeros(y_res, x_res /2), data.mean_tex(:,x_res/2+1:x_res)]);
%    t_gain2 = t_gain2 ./ norm(t_gain2);
%    t_offset1 = vec([ones(y_res, x_res / 2), zeros(y_res, x_res / 2)]);
%    t_offset1 = t_offset1 ./ norm(t_offset1);
%    t_offset2 = vec([zeros(y_res, x_res / 2), ones(y_res, x_res / 2)]);
%    t_offset2 = t_offset2 ./ norm(t_offset2);

%    tex_space.v = [t_gain1, t_gain2, t_offset1, t_offset2, tex_space.v];
  else
    tex_space.v = [];
    tex_space.l = [];
    nb=0;
    var=0;
  end
  printf("\tAppearance space\t[%d / %3.2f%%]\n",nb, var*100);

  
  % 1-D mean vectors
  shape_space.psi = vec(data.psi_n);
%  shape_space.psi = vec(data.psi_p);
  tex_space.psi = vec(data.mean_tex);
  
  % similarity vectors
  
  nb_v = size(data.psi_n,1);
  v_geom = zeros(nb_v*2,n_geom);
  
  v_geom(:,1) = [data.psi_n(:,1); zeros(nb_v,1)];
  v_geom(:,2) = [-data.psi_n(:,2); data.psi_n(:,1)];
  v_geom(:,3) = [ones(nb_v, 1); zeros(nb_v, 1)];
  v_geom(:,4) = [zeros(nb_v, 1); ones(nb_v, 1)];
  v_geom(:,5) = [zeros(nb_v,1); data.psi_n(:,2)];
  
  for i=1:n_geom
    v_geom(:,i) = v_geom(:,i) ./ norm(v_geom(:,i));
  end

  n_geom = size(v_geom,2);
  n_modes = size(shape_space.v, 2);

  %% Problem with gram schmidt !!
  W = [v_geom, shape_space.v];
  Z=gram_schmidt(W);
  v_geom = Z(:,1:n_geom);
  shape_space.v = Z(:,n_geom+1:n_geom+n_modes);


end

triangles = data.triangles;


%
% input parameters
%
% shape_space
% tex_space
% v_geom
% triangles
% res = [x_res, y_res]
% tempDir

n_geom = size(v_geom,2);
n_modes = size(shape_space.v, 2);
n_tex_modes = size(tex_space.v,2);
n_total = n_geom + n_modes + n_tex_modes;

x_res = res(1);
y_res = res(2);

% normalised space
psi_n=vec_to_shape(shape_space.psi);
% image space [1-based]
%psi_i=rescale_shape(psi_n, win_size(1)-1, win_size(2)-1)+1;
% patch space [1-based]
%psi_p=rescale_shape(psi_i, x_res-1, y_res-1)+1;
psi_p=rescale_shape(psi_n, x_res-1, y_res-1);

nb_v = length(psi_p);

