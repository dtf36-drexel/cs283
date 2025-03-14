#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

setup() {
  ./dsh -s -p 12345 > server.log 2>&1 &
  SERVER_PID=$!
  # Allow the server time to start.
  sleep 1
}

teardown() {
  kill $SERVER_PID 2>/dev/null
  rm -f server.log
}

@test "Remote cd command" {
  run ./dsh -c -p 12345 <<EOF
cd /tmp
pwd
exit
EOF
  # Strip whitespace from the output.
  stripped_output=$(echo "$output" | tr -d '[:space:]')
  # Expected output should include the directory change (adjust the prompt if needed).
  expected_output="/tmpdsh4>dsh4>dsh4>cmdloopreturned0"
  echo "Captured stdout:" 
  echo "Output: $output"
  echo "Exit Status: $status"
  echo "${stripped_output} -> ${expected_output}"
  # Check that /tmp appears in the output.
  [[ "$stripped_output" == *"/tmp"* ]]
  [ "$status" -eq 0 ]
}

@test "Remote echo command" {
  run ./dsh -c -p 12345 <<EOF
echo remote test
exit
EOF
  stripped_output=$(echo "$output" | tr -d '[:space:]')
  expected_output="remotetest"
  echo "Captured stdout:" 
  echo "Output: $output"
  echo "Exit Status: $status"
  echo "${stripped_output} -> ${expected_output}"
  [[ "$stripped_output" == *"remotetest"* ]]
  [ "$status" -eq 0 ]
}

@test "Remote dragon command" {
  run ./dsh -c -p 12345 <<EOF
dragon
exit
EOF
  stripped_output=$(echo "$output" | tr -d '[:space:]')
  expected_output="@%%%%"
  echo "Captured stdout:" 
  echo "Output: $output"
  echo "Exit Status: $status"
  echo "${stripped_output} -> ${expected_output}"
  [[ "$stripped_output" == *"@%%%%"* ]]
  [ "$status" -eq 0 ]
}

@test "Remote pipeline command" {
  run ./dsh -c -p 12345 <<EOF
echo pipeline test | tr a-z A-Z
exit
EOF
  stripped_output=$(echo "$output" | tr -d '[:space:]')
  expected_output="PIPELINETEST"
  echo "Captured stdout:" 
  echo "Output: $output"
  echo "Exit Status: $status"
  echo "${stripped_output} -> ${expected_output}"
  [[ "$stripped_output" == *"PIPELINETEST"* ]]
  [ "$status" -eq 0 ]
}

@test "Remote output redirection" {
  run ./dsh -c -p 12345 <<EOF
echo "remote redirection" > /tmp/remote_out.txt
cat /tmp/remote_out.txt
exit
EOF
  stripped_output=$(echo "$output" | tr -d '[:space:]')
  expected_output="remoteredirection"
  echo "Captured stdout:" 
  echo "Output: $output"
  echo "Exit Status: $status"
  echo "${stripped_output} -> ${expected_output}"
  [[ "$stripped_output" == *"remoteredirection"* ]]
  [ "$status" -eq 0 ]
  rm -f /tmp/remote_out.txt
}

@test "Remote append redirection" {
  run ./dsh -c -p 12345 <<EOF
echo "first line" > /tmp/remote_append.txt
echo "second line" >> /tmp/remote_append.txt
cat /tmp/remote_append.txt
exit
EOF
  stripped_output=$(echo "$output" | tr -d '[:space:]')
 
  expected_output="firstlinesecondline"
  echo "Captured stdout:" 
  echo "Output: $output"
  echo "Exit Status: $status"
  echo "${stripped_output} -> ${expected_output}"
  [[ "$stripped_output" == *"firstline"* ]]
  [[ "$stripped_output" == *"secondline"* ]]
  [ "$status" -eq 0 ]
  
rm -f /tmp/remote_append.txt
}

@test "Remote input redirection" {
  echo "input redirection test" > /tmp/remote_input.txt
  run ./dsh -c -p 12345 <<EOF
cat < /tmp/remote_input.txt
exit
EOF
  stripped_output=$(echo "$output" | tr -d '[:space:]')
  expected_output="inputredirectiontest"
  echo "Captured stdout:" 
  echo "Output: $output"
  echo "Exit Status: $status"
  echo "${stripped_output} -> ${expected_output}"
  [[ "$stripped_output" == *"inputredirectiontest"* ]]
  [ "$status" -eq 0 ]
  rm -f /tmp/remote_input.txt
}

@test "Remote stop-server command" {
  run ./dsh -c -p 12345 <<EOF
stop-server
EOF
  stripped_output=$(echo "$output" | tr -d '[:space:]')
  expected_output="clientrequestedservertostop"
  echo "Captured stdout:" 
  echo "Output: $output"
  echo "Exit Status: $status"
  echo "${stripped_output} -> ${expected_output}"
  [[ "$stripped_output" == *"clientrequestedservertostop"* ]]
  [ "$status" -eq 0 ]
}
