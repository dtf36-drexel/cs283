1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets() reads input line-by-line, which means we don't need to parse separate lines manually.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  Malloc lets us allocate memory dynamically. This means we don't waste as much memory, and out program is overall more efficient.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Whitespace causes issues when trying to parse commands. It is simpler to just remove leading whitespace than trying to program functionality that determines "cmd" and "       cmd" are both supposed to be the same thing. Removing whitespace means we take no chances, and also save a little memory.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  "cmd > output.txt" redirects STDOUT to a file.
    "cmd < input.txt" uses a file as STDIN input.
    "cmd 2> error.txt" redirects STDERR to a log file

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection sends data to and from files. Piping sends data as the output of one command to the input of annother. 

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  Makes sure errors don't get treated as normal, intended outputs. Errors are wrong, we don't want to confuse them with intended behavior.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Error messages can be displayed while merging with STDOUT. Linux does this with "2>&1", so we should do that too I suppose.
