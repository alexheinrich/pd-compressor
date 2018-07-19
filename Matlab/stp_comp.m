function [ out ] = stp_comp(in, makeup_gain, threshold, ratio, ...
    knee_width, attack, release, sampling_rate)
%stp_comp Simple compressor based on some math
%   Makeup_gain in dB, Threshold in dB, Ratio, knee_width in dB, 
%   Attack in s, Release in s, sampling rate in Hz

% makeup_gain = 0.0;
% threshold = -20.0;
% ratio = 4.0;
% knee_width = 1.0;
% attack = 0.01;
% release = 0.02;
% sampling_rate = 48e3;

post_level_detector = 0.0;

%Ratio exception handling
if ratio < 1.0
    ratio = 1.0;
end

%Level detector exception handling
if attack <= 0.0
   attack = 0.01;
end
if release <= 0.0
    release = 0.001;
end

%Calculate smooth detection filter coefficients eq. 7
a_attack = exp(-1./(attack * sampling_rate));
a_release = exp(-1./(release * sampling_rate));

%Generate the control voltage
    for i = 1:length(in)
        in_abs = abs(in(i));

        if in_abs ~= 0
            %Convert values in dB
            in_abs = 20*log10(in_abs);
        else
            in_abs = -100;
        end

        %Gain computer
        if (2.0 * (in_abs - threshold) < -1.0 * knee_width)
            buffer = in_abs;
        elseif (2.0 * abs(in_abs - threshold) <= knee_width)
            buffer = in_abs + (1.0 / ratio - 1.0) * ...
                (in_abs - threshold + knee_width / 2.0)^2 / (2.0*knee_width);
        else
            buffer = threshold + (in_abs - threshold) / ratio;
        end

        %Pre-level detector (feed forward loop)
        buffer = in_abs - buffer;

        %Level detector
        if (buffer > post_level_detector)
            post_level_detector = a_attack * post_level_detector + ...
                (1.0 - a_attack) * buffer;
        else 
            post_level_detector = a_release * post_level_detector + ...
                (1.0 - a_release) * buffer;
        end

        %Control voltage
        control_voltage = makeup_gain - post_level_detector;

        %Convert the control voltage to linear and apply it to the audio.
        out(i) = in(i).* (10.0.^(control_voltage / 20.0));
    end
end

