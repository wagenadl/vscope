function x = algcsqr(y)

% all elements of y must be in the interval (0,1)

z=y.^2;
dims=size(z);
lower_bound=repmat(1e-15,dims);
upper_bound=repmat(1-1e-15,dims);
z=max(z,lower_bound);
z=min(z,upper_bound);
x = -log(z.^(-1)-1);
