% % Logarithmic Plot
% subplot(2,1,1)
% semilogy(input);
% hold on
% semilogy(pre_level_det);
% %plot(post_ld_buffer,'-*');
% semilogy(ones(1,length(input))*threshold);
% semilogy(output);
% grid on
% hold off
% legend('Input','Pre LD','Threshold','Output');
% 
% subplot(2,1,2)
% semilogy(post_ld_buffer);
% title('Post Level Detection');
% grid on

%% Linear Plot
% subplot(4,1,1)
% plot(input);
% hold on
% plot(ones(1,length(input))*threshold);
% hold off
% title(['Input + Threshold, Threshold: ', num2str(threshold),...
%     ', Ratio: 1:', num2str(ratio), ', Attack: ', num2str(attack'), ...
%     ', Release: ', num2str(release)]);
% grid on
% 
% subplot(4,1,2)
% plot(pre_level_det);
% title('Pre Level detector')
% grid on
% 
% subplot(4,1,3)
% plot(post_ld_buffer);
% title('Post Level Detection');
% grid on
% 
% subplot(4,1,4)
% plot(output);
% title('Output')
% grid on

%% Test Plot
figure(1)
%subplot(2,1,1)
plot(input)
hold on
plot(output)
plot(ones(1,length(input))*(1-10^(threshold/20)));
hold off
grid on
legend('Input', 'Output','Threshold')

