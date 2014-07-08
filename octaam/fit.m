1;

if exist("no_reload", "var") == 0 ||  no_reload == 0
  Im=imread(image_file) + 0.0;
  R=double(Im(:,:,1));
  G=double(Im(:,:,2));
  B=double(Im(:,:,3));
  input_image = rgb2gray(R,G,B);
  
  if exist("gen_occlusion", "var") == 0
    gen_occlusion = 0;
  end
  
  if gen_occlusion
    
    sc = 8;
    w = round(data.imageW / sc);
    h = round(data.imageH / sc);
    
    occl_image = kron(rand(h, w), ones(sc,sc)) > occlusion_rate;
    r_im = kron((80 + randn(h, w) .* 40), ones(sc,sc)) .* (1-occl_image);
    input_image = input_image .* occl_image + r_im;
    R = input_image;
    G = input_image;
    B = input_image;
  end
  
  figure(1);
clf;

imshow(Im);
end

wsize = [data.imageW data.imageH];
display_2d_shape(current_s, wsize);


err = [0];
dist = [];
residuals = [];
outliers = [];

if exist("fitting_has_truth", "var") == 0
  fitting_has_truth = 0;
end
if exist("fitting_store_shapes", "var") == 0
  fitting_store_shapes = 0;
end
if exist("fitting_strategy", "var") == 0
  fitting_strategy = "iterate";
end
if exist("fitting_store_residuals", "var") == 0
  fitting_store_residuals = 0;
end
if exist("fitting_store_outliers", "var") == 0
  fitting_store_outliers = 0;
end


if fitting_has_truth
  shape_diff = s_opt(:,1:2) - current_s(:,1:2);
  shape_dist = shape_diff(:,1) .^ 2 + shape_diff(:,2) .^ 2;
  shape_rms = sqrt(mean(shape_dist));
  shape_mrs = mean(sqrt(shape_dist));
  shape_sse = sqrt(sum(shape_dist));
  dist=[dist; shape_rms, shape_mrs, shape_sse];

  shape_rms
  shape_mrs
  shape_sse
end

if fitting_store_shapes
  store_shapes = zeros(size(data.shapes,1),2,nb_it);
end

current_it = 1;
while 1

  printf("Iteration %d\n", current_it);

  figure(2);
  iteration;
  figure(1);
drawnow;


  if debug == 1

    figure(1); clf;
    imshow(Im);
    if exist("point_weight", "var")
      display_2d_points(new_s, wsize, point_weight);
    else
      display_2d_shape(new_s, wsize);
#      [R,G,B] = gcSave;
#      imwrite(sprintf("it_%03d.jpg",current_it), R,G,B);
    end
%    display_2d_shape(new_s, wsize);
  end

  err = [err; sum(vec(error .* error))];


  if fitting_store_shapes
    store_shapes(:,:,current_it) = current_s(:,1:2);
  end

  if fitting_store_residuals
    residuals = [residuals, vec(error)];
  end
  if fitting_store_outliers
    outliers = [outliers, out_m];
  end

  if fitting_has_truth
    shape_diff = s_opt(:,1:2) - new_s(:,1:2);
    shape_dist = shape_diff(:,1) .^ 2 + shape_diff(:,2) .^ 2;
    shape_rms = sqrt(mean(shape_dist));
    shape_mrs = mean(sqrt(shape_dist));
    shape_sse = sqrt(sum(shape_dist));
    dist = [dist; shape_rms, shape_mrs, shape_sse];

    shape_rms
    shape_mrs
    shape_sse
  end

  if current_it == nb_it
    break;
  end
  divergence = 0;
  switch fitting_strategy
    case "pixel_accuracy"
      d = (new_s - current_s) .^ 2;
      d = sqrt(d(:,1) + d(:,2));
      m = mean(d)
      shape_mrs
      shape_rms
      if m < 0.2
	break;
      end
    case "shape_bounding"
      printf("test shape\n");
      if n_modes > 0
	pp = inv_base * (vec(new_s(:,1:2)) - shape_space.psi);
	p = pp(n_geom+1:end);
        r = max(abs(p) ./ SL);
        r2 = mean(abs(p) ./ SL);
	r
	r2
%	if (current_it >= 5) && (r > 3)
%	  divergence = 1;
%	  break;
%	end
	if (current_it >= 10) && ((r > 7) || (r2 > 2.5))
	  printf("DIVERGENCE!!\n");
	  divergence = 1;
	  break;
	end
    end
  end

  current_l = new_l;
  current_s = new_s;

  current_it = current_it + 1;
end

