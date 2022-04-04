close all
clear, clc

% Init variables
x_axis = [0];
speed_A_axis = [0];
speed_B_axis = [0];
speed_C_axis = [0];
update_period = 10;         % How often to update plot, points
time_window = 10;           % Time window to be shown on figure, sec
counter = 0;

% Figure settings
figure(1);
set(gcf,'CurrentCharacter','@');                            % Set to a dummy character
%set(gcf, 'MenuBar', 'none', 'maximized');    % Set figure to fullscreen and make none interactible
plotHandle = plot(NaN, NaN, '-b',...
                  NaN, NaN, '-g',...
                  NaN, NaN, '-m');                          % Make initial empty plot
legend('Motor A', 'Motor B', 'Motor C');
xlim([0 time_window]);
ylim([-4 4]);
ylabel('Speed, rps');
xlabel('Time');
grid on
pause(0.5);
% Usart settings
port = serial('COM3', 'BaudRate', 115200);
port.InputBufferSize = 4096;
fopen(port);
set(port, 'ByteOrder', 'littleEndian');
disp('Connection is ready!');

new_values = [];
start_time = clock;

while 1
    counter = counter + 1;
    new_values = fread(port, 1, 'int16');

    % Break from the loop if timeout occured
    if (strcmp(lastwarn,'Unsuccessful read: A timeout occurred before the Terminator was reached or SIZE values were available..'))
        disp('Timeout');
        break;
    end

    % Add new points to the plot and forget last one if necessary
    if x_axis(end) >= time_window
        x_axis = [x_axis(2:end) etime(clock, start_time)];
        speed_A_axis = [speed_A_axis(2:end) new_values ];
    else
        x_axis = [x_axis etime(clock, start_time)];
        speed_A_axis = [speed_A_axis new_values];
    end
    % Update plot
    if counter == update_period
        set(plotHandle(1), 'XData', x_axis, 'YData', speed_A_axis);
        if x_axis(end) >= time_window
            xlim([x_axis(1) x_axis(end)]);
        end
        drawnow;
        counter = 0;
    end

    % Stop loop from keyboard
    k = get(gcf,'CurrentCharacter');
    if k ~= '@'
        disp('Stopped by user');
        break;
    end

end

fclose(port);
disp('Connection is closed!');
