1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  Fork creates a separate child process so that the shell can continue running while the child executes a new program via execvp. Without fork, calling execvp directly would replace the shell itself.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork fails, it returns a negative value. My program checks for this, prints perror, and returns an appropriate error code so that the shell can handle it.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  It searches for executables in directories in the PATH environment variable.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  Wait makes the parent pause until the child finishes. This makes sure the shell can capture the child's exit status. If we don't call it, the shell might continue early or leave defunct processes.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  It takes the exit status from the status code returned by wait. This gives us the actual return code from the child process. This is important cause it tells us whether or not a command executeed correctly or if there was an error.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  It detects an opening quote and preserves all chars including spaces until it reaches a closing quote. It's important for passing args with multiple words, such as "hello world".

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  First of all we got rid of the pipe splitting. I also didn't bother with checking for commands/arguments that were too large, because that wasn't part of the "errors returned" section of the block comment. There wasn't anything I would necessarily consider challenging. The biggest hurdle was probably handling whitespace aside from leading and trailing spaces. I accidentally had my code ignoring all chars except for spaces at first, so every command was just returning nothing.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals let processes be notified of events asynchronously. They differe from other IPC methods because they're simple and can interupt processes immediatly.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**: SIGKILL: Immediately terminates a process and cannot be caught or ignored.
                    SIGTERM: Requests a graceful shutdown, allows the process to clean up resources. Can be caught.
                    SIGINT: Interrupts a process, and can be handled by the process for graceful termination.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  The process in question is suspended by the system. It cannot be caught or ignored because it is enforced by the kernel.

