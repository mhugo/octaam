##
##
##    AAM Tracking sample
##
## Hugo Mercier <hugo.mercier@laposte.net> 10/2007 - 10/2008
##
##
source("./lib/include.m");

###################################################
##               AAM parameters
###################################################

## name of the dataset
dataset_name="35_Hugo";
## number of shape and appearance components to retain (<1 => percentage)
shape_dim = 0.95;
tex_dim = 0.95;
shape_dim = 3;
tex_dim = 9;
## name of the tracker to use
## normal => Normalizing
## pout => Project-out
## simult => Simultaneous
## rob_simult => Simultaneous with robust function
## rob_normal => Normalizing with robust function
tracker_name = "pout";

## appearance patches resolution
x_res = 64;
y_res = 64;

# use C optimization or not ?
USE_C_OPTIMIZATION = 1;

# outlier map computation function
function out_m = compute_map(e_p)
  # very dummy function
  out_m = abs(e_p) < 80;
end


####################################################
##               Tracking parameters
####################################################

## video directory
video_dir = "./img/test/";

## image list
#im_list=[932:1000];
im_list=[5:60];



## number of fitting iterations per frame
init_nb_it = 10;


## size of the display window
win_size = [640 480];
## debug mode
debug = 1;

n_geom = 5;

res = [x_res, y_res];


figure(2);
colormap(gray);
precompute;
figure(1);


N_I = size(data.Models,1);
P = zeros(n_modes,N_I);
Q = zeros(n_geom,N_I);

for i=1:N_I
  sh=vec(data.shapes(:,1:2,i));
  q = inv_base * (sh - shape_space.psi);
  P(:,i) = q(n_geom+1:end);
  Q(:,i) = q(1:n_geom);
end
SL = sqrt(sum(P'.^2)./N_I)';


	      pause;

shape_dim = 0;
tex_dim = 0;
precompute_new_aam;



## starting shape and appearance configurations
## appearance = 0
current_l = zeros(n_tex_modes,1);
## shape = the first shape of the training set
current_s = data.shapes(:,1:2,1);
current_s(:,2) = current_s(:,2) + 10;
#current_s(:,1) = current_s(:,1) - 7;
current_l

## used only for the regularized tracker
eta_inc = 1.2;

## start with the rigid model ?
rigid_mode = 1;

if rigid_mode == 0
  ns = 0.95;
  nt = 0.95;
  load_aam;
  current_l = zeros(n_tex_modes,1);
end


nb_it = init_nb_it;

fitting_store_residuals = 1;
%fitting_store_outliers = 1;

fitting_strategy = "shape_bounding";
#fitting_strategy = "iterate";
good_s = current_s;


%outlier_map = ones(y_res, x_res);

for im=im_list

  im

  if (im == im_list(1)) & (rigid_mode == 0);
    nb_it = init_nb_it * 2;
  else
    nb_it = init_nb_it;
  end

  
  image_file = strcat(video_dir,sprintf("%08d.jpg",im));

  fit;


  if rigid_mode
    ns = 0.95;
    nt = 0.95;
    load_aam;

    current_l = zeros(n_tex_modes,1);
    nb_it = init_nb_it;
    rigid_mode = 0;
#    compute_map_file = "rob_map.m";
    continue;
  end

  if divergence
    ns = 0;
    nt = 0;
    load_aam;

    current_l = zeros(n_tex_modes,1);
    nb_it = 5;
    rigid_mode = 1;
#    compute_map_file = "rob_map_r.m";
    current_s = good_s;
  else
    good_s = new_s;
  end
  
end
