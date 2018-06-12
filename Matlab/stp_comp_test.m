clear vars
clc
close all
input = linspace(0.01,1,48e3);%[linspace(50,30,15),linspace(30,55,15)];
in_abs = abs(input);
buffer = zeros(1,length(input));
post_ld_buffer = zeros(1,length(input));
threshold = .2;
knee_width = .05;
ratio = 8;
attack = 0.01;
release = 0.1;
fs = 48e3;
post_level_det = 1;

% Gain Computer
for i = 1: length(input);
    if 2*(in_abs(i) - threshold) < -1 * knee_width
        buffer(i) = in_abs(i);
        disp([num2str(i), ': Links']);
    elseif 2 * abs(in_abs(i) - threshold) <= knee_width
        buffer(i) = in_abs(i) + ((1 / ratio - 1) *(in_abs(i) - threshold...
            + knee_width / 2)^2 )/ (2 * knee_width);
        disp([num2str(i), ': Knie']);
    else
        buffer(i) = threshold + (in_abs(i) - threshold) / ratio;
        disp([num2str(i), ': Rechts']);
    end
end

pre_level_det = buffer ./ in_abs;
% Zero Crossing gibt NaN

a_attack = exp(-1/(attack*fs));
a_release = exp(-1/(release*fs));

%Level Detector
for i = 1: length(pre_level_det)
    if pre_level_det(i) > post_level_det
        post_ld_buffer(i) = (a_attack*post_level_det)+((1-a_attack)*...
            pre_level_det(i));
        post_level_det = post_ld_buffer(i);
        disp([num2str(i), ': Attack'])
    else
        post_ld_buffer(i) = (a_release*post_level_det)+((1-a_release)*...
            pre_level_det(i));
        post_level_det = post_ld_buffer(i);
        disp([num2str(i), ': Release'])
    end
end

output = post_ld_buffer.*input;

figure(1)
subplot(2,1,1)
plot(input);
hold on
plot(pre_level_det);
%plot(post_ld_buffer,'-*');
plot(ones(1,length(input))*threshold);
plot(output);
grid on
hold off
legend('Input','Pre LD','Threshold','Output');

subplot(2,1,2)
plot(post_ld_buffer);
title('Post Level Detection');
grid on


