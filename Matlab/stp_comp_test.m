clear all
clc
close all
%% Initialize

%Variables
makeup_gain = 0.0;
threshold = [-0.1, -2.5, -5, -10, -20, -40];
ratio = [1, 2, 4, 8, 10];
knee_width = [0, 20];
attack = [0.01, 0.1, 1, 2, 3];
release = [0.01, 0.1, 1, 2, 3];
sampling_rate = 48e3;

%% Ask if files should be saved
show_plots=0;
save_print=0;
save_csv=0;
%save to csv
prompt='Save to csv? (y/n)';
x = input(prompt,'s');
if x == 'y'
    save_csv=1;
else
    save_csv=0;
end

%want to see plots? remember 1500 plots at full capacity
prompt='Show plots? (y/n)';
x = input(prompt,'s');
if x == 'y'
    show_plots=1;
else
    show_plots=0;
end
%want to save them? 
if show_plots == 1
    prompt='Save images? (y/n)';
    x = input(prompt,'s');
    if x == 'y'
        save_print=1;
    else
        save_print=0;
    end
end
%Generate Number of solutions
total_solutions = (length(threshold)*length(ratio)*length(knee_width)...
                            *length(attack)*length(release));
%Generate test signal
%Duration in s
duration = 5;
%Frequency in Hz
f = 440;
%Generate Signal
val = 0:1/sampling_rate:duration;
y_in = sin(2*pi*f*val);
%tmp variables
a=1;
b=1;
c=1;
%initialize output matrix
out_mean_total = zeros(total_solutions, length(release)+2);

%% Send Testsignal to comp
%disp is for debugging
for i = 1:length(threshold)
    disp(['i: ', num2str(i)]);
    for j = 1:length(ratio)
        disp(['j: ', num2str(j)]);
        for k = 1:length(knee_width)
            disp(['k: ', num2str(k)]);
            for l = 1:length(attack)
                disp(['l: ', num2str(l)]);
                for m = 1:length(release)
                    disp(['m: ', num2str(m)]);
                    out = stp_comp(y_in, makeup_gain, threshold(i), ... 
                        ratio(j), knee_width(k), attack(l), release(m),...
                        sampling_rate);
                    %Output matrix
                    out_mean_total(b,:) = [threshold(i),...
                        ratio(j), ...
                        knee_width(k),...
                        attack(l),...
                        release(m),...
                        rms(y_in)-rms(out), ...
                        20*log10(rms(y_in))-20*log10(rms(out))];
                    
                    b=b+1;
                    
                    disp(' ');
                    
                    %% Plots
                    if show_plots == 1
                        h=figure(a);
                        a=a+1;
                        subplot(2,1,1)
                        %input
                        plot(y_in)
                        ylim([-1.2 1.2])
                        hold on
                        %threshold line
                        plot(ones(1,length(y_in))*10^(threshold(i)/20), 'Color', ...
                            [0.9290    0.6940    0.1250])
                        %rms line
                        plot(ones(1,length(y_in))*rms(y_in), 'Color', ...
                            [0.4940    0.1840    0.5560])
                        hold off
                        grid on
                        legend('Input', 'Threshold','RMS');
                        title(['Threshold: ', num2str(threshold(i)), 'dB, ', ...
                            'Ratio: ', num2str(ratio(j)), ', ' ...
                            'Knee Width: ', num2str(knee_width(k)), 'dB, ', ...
                            'Attack: ', num2str(attack(l)), 's, ', ...
                            'Release: ', num2str(release(m)), 's, '
                            ])

                        subplot(2,1,2)
                        %output
                        plot(out, 'Color', [0.8500    0.3250    0.0980])
                        hold on
                        %rms line
                        plot(ones(1,length(out))*rms(out), 'Color', ...
                            [0.4940    0.1840    0.5560])
                        hold off
                        grid on
                        legend('Output','RMS')
                        ylim([-1.2 1.2])
                        
                        %save image
                        set(h,'Units','Inches');
                        if save_print == 1
                            pos = get(h,'Position');
                            set(h,'PaperPositionMode','Auto','PaperUnits',...
                            'Inches','PaperSize',[pos(3), pos(4)])
                            print(h,['images/Sin440_t',num2str(threshold(i)),'_r',...
                                num2str(ratio(j)),'_k',num2str(knee_width(k)),'_a',...
                                num2str(attack(l)),'_r',num2str(release(m)),'.pdf'],...
                                '-dpdf','-r0')
                            close(h)
                        end
                    end
                end
            end
        end
    end
end
%Generate Header for csv file
header = {'Threshold [dB] ', 'Ratio', 'Knee Width', 'Attack [s]',...
    'Release [s]', 'Value', 'Value [dB]'};
out_mean_total = [header; num2cell(out_mean_total)];

%save csv
if save_csv == 1
    fileID = fopen('output.csv', 'w');
    formatSpec_header = '%s; %s; %s; %s; %s; %s; %s\n';
    formatSpec_data = '%f; %f; %f; %f; %f; %e; %f\n';
    fprintf(fileID,formatSpec_header,out_mean_total{1,:});

    [nrows,ncols] = size(out_mean_total);
    for row = 2:nrows
        fprintf(fileID,formatSpec_data,out_mean_total{row,:});
    end
    fclose(fileID);
end