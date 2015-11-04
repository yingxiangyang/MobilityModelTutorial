%%
%dt
dtReal = load('DTReal.txt');
dtSimu = load('DTSimuNoN1.txt');
dtSimuV2 = load('DTSimu.txt');
SemiLogXPlot(dtSimuV2(:,1)/3600,'s','r',900/3600,160000,12)
hold on
SemiLogXPlot(dtReal(:,1)/3600,'o','b',900/3600,160000,12)
SemiLogXPlot(dtSimu(:,1)/3600,'v','k',900/3600,160000,12)
set(gcf, 'Position', [100 100 350 350])
set(gca,'FontName','Times New Roman','FontSize',18)
xlabel('dt, [h]','FontName','Times New Roman','FontSize',18)
ylabel('P(dt)','FontName','Times New Roman','FontSize',18)
xlim([500/3600 10^6/3600])
set(gca,'XTick',[0.2 1 5 25 125])
ylim([0 0.4])
legend('Simu','Data','No beta 1','location','northeast')


index1=find(dtReal(:,1)>900 & dtReal(:,1)<160000);
index2=find(dtSimuV2(:,1)>900 & dtSimuV2(:,1)<160000);
index3=find(dtSimu(:,1)>900 & dtSimu(:,1)<160000);
[h,p,KSSTAT] = kstest2(dtReal(index1,1),dtSimuV2(index2,1));
KSSTAT
[h,p,KSSTAT] = kstest2(dtReal(index1,1),dtSimu(index3,1));
KSSTAT

%%
%P(N)
load pnavg
dailyActivityReal = load('DNReal.txt');
dailyActivitySimu = load('DNSimu.txt');
dailyActivitySimuNoN2 = load('DNNoN2.txt');
[number_count,number]=hist(dailyActivitySimu(:,2),unique(dailyActivitySimu(:,2)));
[number_count1,number1]=hist(dailyActivitySimuNoN2(:,2),unique(dailyActivitySimuNoN2(:,2)));
[number_count2,number2]=hist(dailyActivityReal(:,2),unique(dailyActivityReal(:,2)));
semilogy(number(2:15),number_count(2:15)/sum(number_count(2:15)),'rs-','markersize',8)
hold on
semilogy(number2(2:15),number_count2(2:15)/sum(number_count2(2:15)),'bo-','markersize',8)
semilogy(number1(2:9),number_count1(2:9)/sum(number_count1(2:9)),'kv-','markersize',8)
semilogy(2:15,pnavg(2:15)/sum(pnavg(2:15)),'g-','markersize',8)
set(gcf, 'Position', [100 100 350 350])
set(gca,'FontName','Times New Roman','FontSize',18)
xlabel('N','FontName','Times New Roman','FontSize',18)
ylabel('P(N)','FontName','Times New Roman','FontSize',18)
xlim([2 15])
ylim([10^-5 10^0])
set(gca,'YTick',[10^-7,10^-6,10^-5,10^-4,10^-3,10^-2,10^-1,1])
set(gca,'XTick',[0 5 10 15 20])
legend('Simulation','Data','No beta 2','Analytic','location','southwest')
set(gca,'ticklength',3*get(gca,'ticklength'))
legend('boxoff')

index1=find(dailyActivityReal(:,2)>1 & dailyActivityReal(:,2)<20);
index2=find(dailyActivitySimu(:,2)>1 & dailyActivitySimu(:,2)<20);
index3=find(dailyActivitySimuNoN2(:,2)>1 & dailyActivitySimuNoN2(:,2)<20);
[h,p,KSSTAT] = kstest2(dailyActivityReal(index1,2),dailyActivitySimu(index2,2));
KSSTAT
[h,p,KSSTAT] = kstest2(dailyActivityReal(index1,2),dailyActivitySimuNoN2(index3,2));
KSSTAT

%beta 1 beta 2
Parameters=load('Parameters.txt');
b1b2=zeros(21);
m=size(Parameters,1);
for i=1:m
    b1b2(Parameters(i,1)+1,Parameters(i,2)+1)=b1b2(Parameters(i,1)+1,Parameters(i,2)+1)+1;
end
b1b2=b1b2./sum(sum(b1b2));
bar3(b1b2,0.99)
colormap(jet)
set(gca,'FontName','Times New Roman','FontSize',15)
xlabel('beta2','FontName','Times New Roman','FontSize',15)
ylabel('beta1','FontName','Times New Roman','FontSize',15)
xlim([0 21.5])
ylim([0 20.5])
set(gcf, 'Position', [100 100 400 400])

%nw
Parameters=load('Parameters.txt');
index=find(Parameters(:,3)<40);
[N,X]=hist(Parameters(index,3));
bar(X,N/sum(N));
set(gcf, 'Position', [100 100 300 300])
set(gca,'FontName','Times New Roman','FontSize',18)
xlabel('nw','FontName','Times New Roman','FontSize',18)
ylabel('f(nw)','FontName','Times New Roman','FontSize',18)
set(gca,'XTick',[0 10 20 30 40])


%P(k)
figure()
real = load('RP.txt');
m = max(size(real));
real = [1:m;real];
unitBin=10;
PowerLawPlotWeighted(real(:,:)','o','b',0,999999,unitBin);
xlim([10^1 10^5]);
%ylim([10^-6 10^-3])
set(gcf, 'Position', [100 100 300 300])
set(gca,'XTick',[10^-1,10^0,10^1,10^2,10^3,10^4,10^5])
xlabel('k','FontName','Times New Roman','FontSize',18)
ylabel('P(k)','FontName','Times New Roman','FontSize',18)
set(gca,'FontName','Times New Roman','FontSize',18)
set(gca,'ticklength',3*get(gca,'ticklength'))
legend('boxoff')

%P(r)
figure()
PrData=load('PrData.txt');
PrSimu=load('PrSimu.txt');
PrData=PrData(:,1);
PrSimu=PrSimu(:,1);
unitBin=12;
PowerLawPlot(PrSimu,'s','r',0.7,100,unitBin);
hold on
PowerLawPlot(PrData,'o','b',0.7,100,unitBin);
set(gcf, 'Position', [100 100 300 300])
xlim([10^-0.2 10^2])
ylim([10^-5 10^0.2])
set(gca,'XTick',[10^-1,10^0,10^1,10^2,10^3])
set(gca,'YTick',[10^-7,10^-6,10^-5,10^-4,10^-3,10^-2,10^-1,10^0,10^1,10^2,10^3])
set(gca,'FontName','Times New Roman','FontSize',18)
xlabel('r, km','FontName','Times New Roman','FontSize',18)
ylabel('P(r)','FontName','Times New Roman','FontSize',18)
set(gca,'ticklength',3*get(gca,'ticklength'))
legend('Simu','Data','location','southwest')
legend('boxoff')


%f(L)
load FL10to20.txt
load FL20to30.txt
loglog( FL10to20(1:10),'bo','markersize',8)
hold on
loglog(FL20to30(1:20),'bs','markersize',8)
set(gca,'FontName','Times New Roman','FontSize',18)
xlabel('L','FontName','Times New Roman','FontSize',18)
ylabel('f(L)','FontName','Times New Roman','FontSize',18)