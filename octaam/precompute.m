1;


global warp_2d;
global update_h;
global update_h_rob;

if USE_C_OPTIMIZATION == 1
  update_h = @update_h_copt;
  update_h_rob = @update_h_rob_copt;
  warp_2d = @warp_2d_copt;
else
  update_h = @update_h_octave;
  update_h_rob = @update_h_rob_octave;
  warp_2d = @warp_2d_octave;
end


printf("Dataset: \t\t\t%s\n", dataset_name);


load_cset;

printf("Computing eigen spaces ...");

load_space;

sub_cache_temp = sprintf("%s/%f_%f",cache_temp,shape_dim,tex_dim);
mkdir(sub_cache_temp);


cache_file=strcat(sub_cache_temp, "/precompute.mat");
if (exist("no_cache","var") == 0 || no_cache == 0) && exist(cache_file, "file") ~= 0
  if exist("mask","var") == 0
    printf("%s\n", cache_file);
    load(cache_file);
  end
else
  %  printf("Computing triangle map ... ");
  %  if exist("tmap", "var")
    %    printf("\t[MEM]\n");
    %  else
    %    tmap = get_tmap(psi_p-1, triangles, res);  
    %    if debug == 1
      %      imagesc(tmap);
      %    end
    %    printf("\n");
    %  end
  tmap = data.tmap;  
  
  %mask = (tmap > 0);
  mask = erode((tmap > 0), [1 1 1;1 1 1; 1 1 1]);

  Psi=psi_p;
  
  %% compute a look up table : foreach point, list the surrounding triangles
  
  if exist("sur_triangles", "var") == 0
    l=num2cell(1:nb_v);
    for i=1:nb_v
      [r,c] = find(triangles==i);
      l(i)=r;
    end;
    sur_triangles = l;
  end
  
  
  printf("Computing dW / dx ... \t\t");
  if exist("dxt", "var")
    printf("[MEM]\n");
  else
    dxt = zeros(y_res, x_res, nb_v);
    
    %
    % foreach vertex
    %
    for v=1:nb_v
      
      printf("\rComputing dW / dx ... \t\t[%d/%d]", v, nb_v);
      
      
      lv = nth(sur_triangles, v);
      
      %
      % foreach surrounding triangle of this vertex
      %
      for i=1:length(lv)
	
	t=lv(i);
	
	%
	% sort the vertex list in order to have
	% v0 in correspondance
	%
	v0=v;
	tt=triangles(t,:);
	if tt(1) == v0
	  v1 = tt(2);
	  v2 = tt(3);
	end
	if tt(2) == v0
	  v1 = tt(1);
	  v2 = tt(3);
	end
	if tt(3) == v0
	  v1 = tt(1);
	  v2 = tt(2);
	end
	
	dx=zeros(y_res, x_res);
	dy=zeros(y_res, x_res);
	
	%
	% foreach pixel belonging the the triangle t
	%
	
	[cy,cx] = find(tmap == t); 
	for p=1:length(cx)
	  x=cx(p);
	  y=cy(p);
	  
	  v0x = Psi(v0,1);
	  v0y = Psi(v0,2);
	  v1x = Psi(v1,1);
	  v1y = Psi(v1,2);
	  v2x = Psi(v2,1);
	  v2y = Psi(v2,2);
	  
	  denum = (v1x - v0x) * (v2y - v0y) - (v1y - v0y) * (v2x - v0x);
	  a = (x - v0x) * (v2y - v0y) - (y - v0y) * (v2x - v0x);
	  b = (y - v0y) * (v1x - v0x) - (x - v0x) * (v1y - v0y);
	  
	  %
	  % compute dW / dx(i)
	  %
	  
	  dx(y,x) = (1 - a/denum - b/denum);
	end
	
	dxt(:,:,v) = dxt(:,:,v) + dx;
      end
      
      if debug == 1
	imagesc(dxt(:,:,v));
	drawnow;
      end
      
    end
    
    printf("\n");
  end

  
  printf("Computing jacobians ... \t");
  if exist("jac_x", "var")
    printf("[MEM]\n");
  else
    %
    % Compute the jacobians from dW / dx
    %
    
    n_jac = n_geom + n_modes;
    var=zeros(nb_v,2,n_jac);
    
    for i=1:n_geom
      var(:,:,i) = vec_to_shape(v_geom(:,i));
    end;
    for i=1:n_modes
      var(:,:,n_geom+i) = vec_to_shape(shape_space.v(:,i));
    end;
    
    jac_x=zeros(y_res, x_res, n_jac);
    jac_y=zeros(y_res, x_res, n_jac);
    for i=1:n_jac
      printf("\rComputing jacobians ... \t[%d/%d]", i, n_jac);
      for j=1:nb_v
	gx = dxt(:,:,j) * var(j,1,i);
	gy = dxt(:,:,j) * var(j,2,i);
	jac_x(:,:,i) = jac_x(:,:,i) + gx;
	jac_y(:,:,i) = jac_y(:,:,i) + gy;
      end
      if debug == 1
	imagesc(jac_x(:,:,i));
	drawnow;
      end
    end
    printf("\n");
  end
  
  
  
  
  
  %
  %             Template gradient
  %
  
  
  if exist("GX", "var") == 0
    I=reshape(tex_space.psi, y_res, x_res);
    
    [GX,GY] = im_grad(I);
    GX = GX .* mask;
    GY = GY .* mask;
    
    if debug == 1
      imagesc(GY);
      drawnow;
    end
  end
  
  
  printf("Computing nabla_a ... \t\t");
  if exist("nabla_a", "var")
    printf("[MEM]\n");
  else
    nabla_a=zeros(y_res,x_res,2, n_tex_modes);
    for j=1:n_tex_modes
      
      I=reshape(tex_space.v(:,j), y_res, x_res);
      
      [AX, AY] = im_grad(I);
      AX = AX .* mask;
      AY = AY .* mask;
      
      nabla_a(:,:,1,j) = AX;
      nabla_a(:,:,2,j) = AY;
      if debug == 1
	imagesc(nabla_a(:,:,1,j))
	drawnow;
      end
      
    end;
    
    printf("[OK]\n");
  end
  
  
  W = [v_geom, shape_space.v];
  %% when gram schmidt is OK
  %%inv_base = W';
  inv_base = inv(W'*W)*W';
  
  
  printf("Save in %s\n", cache_file);
  save("-binary",cache_file, "dxt", "jac_x", "jac_y", "tmap", "mask", "sur_triangles", "GX", "GY", "nabla_a", "inv_base", "shape_space", "tex_space", "n_modes", "n_tex_modes", "n_total");
  
end

%====================================================
%
% tracker specific precomputations
%
%====================================================

%
%             Steepest descent images
%

printf("Tracker specific\t\t%s\n",tracker_name);
if exist("precomputed", "var")==0 || precomputed != tracker_name
  t_cache_file=strcat(sub_cache_temp, "/precompute_",tracker_name,".mat");
  if exist(t_cache_file, "file")
    load(t_cache_file)
    printf("\t\t\t\t[CACHE]\n");
  else
    precomp_file = strcat("./trackers/",tracker_name,"/precompute.m");
    source(precomp_file);
    printf("Saved in %s\n", t_cache_file);
  end
  precomputed = tracker_name;
else
  printf("\t\t\t\t[MEM]\n");
end

