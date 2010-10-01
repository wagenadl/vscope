K = length(omega);

for k=1:K
  fprintf(1,'%2i  C: %.3f. matlab: %.3f\n',...
    k, omega{k},elfind_nearest(elbuild_xyrra(xyrra{k}),xy{k})); 
end

figure(1); clf
N=ceil(sqrt(K));
M=ceil(K/N);
for k=1:K
  subplot(M,N,k);
  axis([-300 600 -300 600]);
  hold on
  elplot_xyrra(xyrra{k});
  plot(xy{k}(1),xy{k}(2),'r.');
  [om, zz, qq] = elfind_nearest(elbuild_xyrra(xyrra{k}),xy{k});
  xy1=real(qq.xy1);
  plot(xy1(:,1),xy1(:,2),'b.');
  plot(zz(1),zz(2),'g.');
  title(k);
  set(gca,'xtick',[],'ytick',[]);
end
