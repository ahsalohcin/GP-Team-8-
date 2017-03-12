M = csvread('cam_sample02.csv',0,1);
imagesc(M(80:160,1:128));
colorbar;
colormap(bone);
for i=1:size(M,1)
    E(i,1:128) = edge(M(i,1:128));
    F(i,1:128) = abs(filter([-1 0 1], 1, M(i,1:128)));
end
figure;
imagesc(E);
colorbar;

figure;
imagesc(F);
colorbar;
figure;
imagesc(E);
colorbar;
