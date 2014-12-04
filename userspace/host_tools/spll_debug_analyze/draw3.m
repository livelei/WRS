% Maciej Lipinski / CERN / 2014-10-22
% 
% scripts to analyzer debugging messages from the SoftPLL of the switch
% 
% options:
% 
% 
function draw3(mpll, bpll, hpll, switchover, start, finish, option, backups_n)

unitScale = (1/((62.5-(62.5*((2^14)/(1+2^14))))*10^6)); % [s]
%  Xaxis     = (start:finish)*unitScale;
Xaxis     = (1:(finish-start+1))*unitScale;
to_ps     = 16000/(2^14); 

figure
subplot(4,1,1)
hold on
plot(Xaxis,mpll(start:finish,3)*to_ps,'b');
if(option == 3)
    plot(Xaxis,mpll(start:finish,5)*to_ps,'k');
end
plot(Xaxis,mpll(start:finish,7)*to_ps,'g');
plot(Xaxis,mpll(start:finish,8)*to_ps,'m');
plot(Xaxis,mpll(start:finish,6)*max(mpll(start:finish,3)),'r' );
if(option == 3)
   legend('corrected err (input to PI)','real error','long average','short average','switchover','Location','northwest');
else
   legend('err (input to PI)','long average','short average','switchover','Location','northwest');
end

title('mPLL');
xlabel('time [s]');
ylabel('phase [ps]');
axis tight

clr=['r','g','b','c']
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
subplot(4,1,2)
hold on
for nn=1:backups_n;
  plot(Xaxis,bpll(start:finish,3,nn)*to_ps,clr(nn));
end
if backups_n == 1
  legend('bPLL 0','Location','northwest');
elseif backups_n == 2
  legend('bPLL 0','bPLL 1','Location','northwest');
elseif backups_n == 3
  legend('bPLL 0','bPLL 1','bPLL 2','Location','northwest');
else
  legend('bPLL 0','bPLL 1','bPLL 2','bPLL 3','Location','northwest');
end
title('bPLLs: error');
xlabel('time [s]');
ylabel('phase [ps]');
axis tight
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
subplot(4,1,3)
hold on
plot(Xaxis,bpll(start:finish,3,nn)*to_ps,'k');
for nn=1:backups_n;
  plot(Xaxis,bpll(start:finish,7,nn)*to_ps,clr(nn));
end
if backups_n == 1
  legend('err', 'bPLL 0','Location','northwest');
elseif backups_n == 2
  legend('err', 'bPLL 0','bPLL 1','Location','northwest');
elseif backups_n == 3
  legend('err', 'bPLL 0','bPLL 1','bPLL 2','Location','northwest');
else
  legend('err', 'bPLL 0','bPLL 1','bPLL 2','bPLL 3','Location','northwest');
end
title('bPLLs: long average');
xlabel('time [s]');
ylabel('phase [ps]');
axis tight

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
subplot(4,1,4)
hold on
plot(Xaxis,bpll(start:finish,3,nn)*to_ps,'k');
for nn=1:backups_n;
  plot(Xaxis,bpll(start:finish,8,nn)*to_ps,clr(nn));
end
if backups_n == 1
  legend('err', 'bPLL 0','Location','northwest');
elseif backups_n == 2
  legend('err', 'bPLL 0','bPLL 1','Location','northwest');
elseif backups_n == 3
  legend('err', 'bPLL 0','bPLL 1','bPLL 2','Location','northwest');
else
  legend('err', 'bPLL 0','bPLL 1','bPLL 2','bPLL 3','Location','northwest');
end
title('bPLLs: short average');
xlabel('time [s]');
ylabel('phase [ps]');
axis tight

return