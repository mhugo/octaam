# use this in octaam to convert the set for icaampp
#

load(strcat(sub_cache_temp, "/precompute.mat"));
triangle_map = data.tmap;
resY = size(data.tmap, 1);
resX = size(data.tmap, 2);
shapes = [shape_space.psi, shape_space.v];
textures = [tex_space.psi, tex_space.v];
triangulation = data.triangles;
save("-binary", "set.mat", "triangle_map", "resX", "resY", "shapes", "textures", "v_geom", "triangulation", "triangle_map", "GX", "GY", "jac_x", "jac_y", "nabla_a");
