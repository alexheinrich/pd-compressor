%Create a white noise burst file
fs = 48e3;

y_burst = wgn(fs,1,0);

%Define bursts and silence
k=4800;
m=4800;

y_in = zeros(m,1);
y_in(k+1:k+m) = y_burst(k+1:k+m);
k=k+m;
y_in(k+1:k+m) = zeros(m,1);
k=k+m;
y_in(k+1:k+m) = y_burst(k+1:k+m);
k=k+m;
y_in(k+1:k+m) = zeros(m,1);
k=k+m;
y_in(k+1:k+m) = y_burst(k+1:k+m);
k=k+m;
y_in(k+1:k+m) = zeros(m,1);
k=k+m;
y_in(k+1:k+m) = y_burst(k+1:k+m);
k=k+m;
y_in(k+1:k+m) = zeros(m,1);
k=k+m;
y_in(k+1:k+m) = y_burst(k+1:k+m);
%Max Val is 1
y_in = y_in./max(abs(y_in));
%plot(y_in);















