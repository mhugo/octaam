# convert Octave file to binary file

load("set.mat");

fp = fopen("set.dat", "w+");

resX = size(triangle_map, 2);
resY = size(triangle_map, 1);

fwrite(fp, resX, "int16");
fwrite(fp, resY, "int16");

fwrite(fp, size(shapes, 2), "int16");
fwrite(fp, size(shapes, 1), "int16");
fwrite(fp, shapes, "double");

fwrite(fp, v_geom, "double");

fwrite(fp, size(textures, 2), "int16");
fwrite(fp, size(textures, 1), "int16");
fwrite(fp, textures, "double");

fwrite(fp, size(triangulation, 1), "int16");
fwrite(fp, triangulation-1, "char");

fwrite(fp, triangle_map, "char");

fwrite(fp, GX, "double");
fwrite(fp, GY, "double");

#// size(shapes, 2) + 4
fwrite(fp, jac_x, "double");
fwrite(fp, jac_y, "double");

fwrite(fp, nabla_a, "double");

fclose(fp);
