% input args:
%
% yres
% xres
% dataset_name
% debug


cache_temp=sprintf("%s_%d_%d.cache",dataset_name(:),x_res,y_res);
mkdir(cache_temp);
cache_file=strcat(cache_temp,"/cset.mat");

%
% load dataset = 
% * normalized shapes
% * apparence patches
% * 2D mean matrices
% load from cache if present

  printf("Loading data ... \t\t");
if (exist("no_cset_cache","var") == 0 || no_cset_cache == 0) && exist(cache_file,"file") ~= 0
  if exist("inmem_cset","var") == 0
    load(cache_file);
    printf("[CACHE]\n");
  else
    printf("[MEM]\n");
  end
  inmem_cset=1;
else
  
  printf("\n");
  printf("\tLoading data ...");
  load(strcat(dataset_name,".set"));
  
  nb_samples = size(data.shapes,3);
  if exist("nb_images", "var") == 0
    nb_images = nb_samples;
  end
  nb_images;
  data.shapes=data.shapes(:,:,1:nb_images);
  data.Models=data.Models(1:nb_images,:);

  printf("\t[OK]\n");
  
  res=[x_res y_res];
  
  X=reshape(data.shapes(:,1:2,:),size(data.shapes,1)*2,size(data.shapes,3));
  printf("\tAligning shapes ...");
  [shapes_n, psi_n] = align_2d_set(X);
  printf("\n");
  
  psi_p = rescale_shape(psi_n,x_res-1,y_res-1);
  printf("\tTriangle map ...");

  tmap = get_tmap(psi_p, data.triangles, res);
  printf("\t[OK]\n");
  if debug == 1
    T = tmap + 255 .* (tmap == 0);
    imagesc(T);
    display_2d_shape(psi_p, [y_res, x_res], data.triangles);
  end
  
  printf("\tGet appearances ...");
  data.patches = get_appearance_patches(data, psi_p, tmap, res);
  data.patches_n = data.patches;
  printf("\n");
  for i=1:size(data.shapes,3)
    data.patches_n(:,:,i) = data.patches(:,:,i) ./ norm(vec(data.patches(:,:,i)));
  end
  
  data.shapes_n = shapes_n;
  data.res = res;
  data.tmap = tmap;
  data.psi_p = psi_p;
  data.psi_n = psi_n;
%  data.mean_tex = reshape(mean(reshape(data.patches_n,x_res*y_res,size(data.patches_n,3))')',y_res,x_res);
  data.mean_tex = reshape(mean(reshape(data.patches,x_res*y_res,size(data.patches,3))')',y_res,x_res);
  imagesc(data.mean_tex);
  
  save(cache_file,"data");
  inmem_cset = 1;
end
