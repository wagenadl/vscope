clf
xx=[-1 1 2 1 -1 -2];
yy=[-1 -1 0 1 1 0];
plot([xx xx(1)],[yy yy(1)],'r.-');
hold on
for k=1:length(xx)
  text(xx(k),yy(k),sprintf('%i',k));
end
setappdata(gca,'polyx',xx);
setappdata(gca,'polyy',yy);
setappdata(gca,'hdel',[]);
axis([-3 3 -2 2]);
set(gca,'buttondownfcn',@polymod);
