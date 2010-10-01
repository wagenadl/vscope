function polymod(h,x)
xx=getappdata(gca,'polyx');
yy=getappdata(gca,'polyy');
delete(getappdata(gca,'hdel'));

xy = get(gca,'currentpoint');
x = xy(1,1);
y = xy(1,2);
xy = [x,y];

dd = (x-xx).^2 + (y-yy).^2;
[d,ix] = min(dd); % find nearest point

xy_near = [xx(ix),yy(ix)];
if ix<length(xx)
  xy_next = [xx(ix+1),yy(ix+1)];
else
  xy_next = [xx(1),yy(1)];
end
if ix>1
  xy_prev = [xx(ix-1),yy(ix-1)];
else
  xy_prev = [xx(end),yy(end)];
end

% Should we add before or after ix?
dxy_next = xy_near - xy_next;
dxy_prev = xy_near - xy_prev;
dr_next = sqrt(sum(dxy_next.^2));
dr_prev = sqrt(sum(dxy_prev.^2));
u_next = dxy_next ./ dr_next;
u_prev = dxy_prev ./ dr_prev;
u_crit = u_next + u_prev; u_crit = u_crit ./sqrt(sum(u_crit.^2));
theta_crit = atan2(u_crit(2),u_crit(1));
theta_p = atan2(y-xy_near(2),x-xy_near(1));
dtheta = theta_p - theta_crit;
% I don't think this is quite the right criterion. Theta_crit itself is fine though.
% Perhaps, I should somehow ensure that u_crit points toward my point and thennot use 2*dthetabut theta itself.
if sin(2*dtheta)>0
  % use next
  fprintf(1,'use %i and next\n',ix);
else
  % use prev
  fprintf(1,'use %i and prev\n',ix);
end
h=plot(xy_near(1)+[0 1]*u_crit(1),xy_near(2)+[0 1]*u_crit(2),'b');
setappdata(gca,'hdel',h);

