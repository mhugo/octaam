function [patchesR, patchesG, patchesB]=get_color_appearance_patches(data, psi_p, tmap, res)
  x_res = res(1);
  y_res = res(2);

  nb_samples = size(data.shapes,3);
  patchesR=zeros(y_res, x_res, nb_samples);
  patchesG=zeros(y_res, x_res, nb_samples);
  patchesB=zeros(y_res, x_res, nb_samples);
  for i=1:nb_samples
    printf("\r\tGet appearances ...\t[%d/%d]", i, nb_samples);
    ST = warp_coeffs(psi_p, data.shapes(:,:,i), data.triangles);
    
    fimg = strcat(data.ImgDir,"/",data.Models(i,:),".",data.ImgExt);
    Im=imread2(fimg);
    R = double(Im(:,:,1));
    G = double(Im(:,:,2));
    B = double(Im(:,:,3));
    figure(2);
%    imagesc(I);
%    gcBackground(I,I,I);
%    display_2d_shape(data.shapes(:,:,i), [data.imageW, data.imageH], data.triangles);
    

    PR=warp_2d(R, ST, data.triangles, tmap, [y_res x_res]);
    PG=warp_2d(G, ST, data.triangles, tmap, [y_res x_res]);
    PB=warp_2d(B, ST, data.triangles, tmap, [y_res x_res]);
    PR = PR .* (tmap != 0);
    PG = PG .* (tmap != 0);
    PB = PB .* (tmap != 0);
    figure(0);
    imshow(PR,PG,PB);
    display_2d_shape(psi_p, [x_res, y_res], data.triangles);  
    patchesR(:,:,i) = PR;
    patchesG(:,:,i) = PG;
    patchesB(:,:,i) = PB;
  end
end
