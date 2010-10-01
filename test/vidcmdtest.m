foo=wavread('test/vidcmd.wav');
tt=[1:length(foo)]/48000;
plot(foo);

f0 = mean(foo(1:1e5,1));
f1 = std(foo(1:1e5,1));    
cmd=(foo(:,1)-f0)/f1;

plot(tt,cmd);

[iup,idn] = schmitt(-cmd,1000,-1000);
t0 = tt(iup(1))-.05;
t1 = tt(iup(end))+.05;

plot(tt(iup),[0; diff(iup)],'.');
axis([t0 t1 0 100]);

clf
plot(tt,cmd/100);
hold on
plot(tt(iup),0*iup-1000/100,'r+');
plot(tt(idn),0*idn+1000/100,'g+');

plot(tt(iup),[0; diff(iup)],'r.');
axis([tt(iup(1))-.0025 tt(iup(1))+.0325 -100 100]);

sgn=diff(iup);
sgn=sgn(sgn<100);
figure(2); clf
hist(sgn,[1:59]);
hold on
plot([12*2 30+12 30+30],[5 5 5],'r.');

ival=diff(iup);

msgs={};
msg=[];
chk=0;
bit=0;
byte=0;
inmsg=0;
wakeups=0;

for k=1:length(ival)
  dt=ival(k);
  if dt>=9 & dt<=15
    % wakeup bit
    wakeups=wakeups+1;
    if wakeups>8
      inmsg=1;
    end
    if inmsg
      if bit==8
        msg=[msg byte];
	if length(msg)<msg(1)
	  chk = bitand(hex2dec('017b')*chk + byte,2^24-1); % + floor(chk/2048);
	end
        byte=0;
        bit=0;
      elseif bit>0
        fprintf(1,'Communication error after %i bits. so far: 0x%02x\n', ...
  	  bit,byte);
        bit=0;
        byte=0;
      end
    end
  elseif dt>=27 & dt<=33
    % zero bit
    bit=bit+1;
    wakeups=0;
  elseif dt>=45 & dt<=51
    % one bit
    byte = byte + 2^bit;
    bit=bit+1;
    wakeups=0;
  elseif dt<100
    fprintf(1,'Communication error after %i bits. so far: 0x%02x\n', ...
	bit,byte);
  else
    if inmsg
      fprintf(1,'Message: ');
      for k=1:length(msg)
	fprintf(1,' %02x',msg(k));
      end
      fprintf(1,'. Check: %08x\n',int32(chk));
      msgs{end+1} = msg;
      msg=[];
      inmsg=0;
      chk=0;
    end
  end
end

if inmsg
  fprintf(1,'Message: ');
  for k=1:length(msg)
    fprintf(1,' %02x',msg(k));
  end
  fprintf(1,'\n');
      fprintf(1,'. Check: %08x\n',chk);
  msgs{end+1}=msg;
  inmsg=0;
  msg=[];
  chk=0;
end
