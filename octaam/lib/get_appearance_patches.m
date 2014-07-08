function patches = get_appearance_patches(data, psi_p, tmap, res)
  global warp_2d;

  x_res = res(1);
  y_res = res(2);

  nb_samples = size(data.shapes,3);
  patches = zeros(y_res, x_res, nb_samples);
  for i=1:nb_samples
    printf("\r\tGet appearances ...\t[%d/%d]", i, nb_samples);
    ST = warp_coeffs(psi_p, data.shapes(:,:,i), data.triangles);
    
    fimg = strcat(data.ImgDir,"/",data.Models(i,:),".",data.ImgExt);
    Im=imread2(fimg);
    I=rgb2gray(Im(:,:,1),Im(:,:,2),Im(:,:,2));


    P = warp_2d(I, ST, data.triangles, tmap, [y_res x_res]);
    P = P .* (tmap != 0);

    imagesc(P);
    display_2d_shape(psi_p, [x_res, y_res], data.triangles);  
drawnow;
    patches(:,:,i) = P;
  end
end
