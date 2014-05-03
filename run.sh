nohup ./ai-sensor localhost 6001 &> /dev/null &
nohup ./ai-control 6001 localhost 6002 &> /dev/null &
nohup ./ai-actuator 6002 &> /dev/null &
