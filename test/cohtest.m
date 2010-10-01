N=128;
tt=[1:N]/25;
xx=cos(2*pi*tt*3.9) + randn([1 N])*.5;
yy=sin(2*pi*tt*3.9) + randn([1 N])*.5;
fd=fopen('sig1.dat','wb');
fwrite(fd,xx,'double');
fclose(fd);
fd=fopen('sig2.dat','wb');
fwrite(fd,yy,'double');
fclose(fd);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
f_res_pow=.65;  % Hz
f_res_coh=4.1/(128*1/25);  % 0.8 Hz
[f_refx,P_refx]=pds_mtm0(tt,xx,f_res_pow);
[f_refy,P_refy]=pds_mtm0(tt,yy,f_res_pow);

cla; 
plot(f_refy,P_refy)
hold on;
plot(f_refx,P_refx,'r')

fprintf(1,'Power spectral densities\n');
[f_refx P_refx P_refy]

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
fprintf(1,'Coherence w/o conf.i.v.\n');

[f_o,C_mag,C_phase] = ...
    coh_mtm0(tt,yy,xx,f_res_coh);

[f_o C_mag C_phase]

[C_mag, C_phase] = coh_mtm0(tt,yy,xx,f_res_coh,1,4)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

fprintf(1,'Coherence with conf.i.v.\n');
alpha_ci = .31;
[f_o,C_mag,C_phase, Cm0, Cp0, Cm1, Cp1] = ...
    coh_mtm0(tt,yy,xx,f_res_coh, alpha_ci);

[f_o C_mag Cm0 Cm1 C_phase Cp0 Cp1]

[C_mag, C_phase, Cm0, Cp0, Cm1, Cp1] = ...
    coh_mtm0(tt,yy,xx,f_res_coh,alpha_ci,4);

[4 C_mag Cm0 Cm1 C_phase Cp0 Cp1]
