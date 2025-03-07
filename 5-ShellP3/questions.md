1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

The shell waits for each child process by looping over their PIDs and calling waitpid(). Without this, finished children would become zombies and system resources could leak.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

Closing unused pipe ends prevents file descriptor leaks and ensures EOF is signaled correctly. Leaving them open may cause processes to hang waiting for input, leading to deadlocks. Deadlocks are yucky. We do not like deadlocks.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

The cd command must be built-in because it changes the shell’s own working directory. If it were external, the directory change would occur only in the child process and not affect the parent shell.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

We could use dynamic memory allocation like realloc for command and pipe arrays. The trade-offs include added complexity in memory management and potential performance overhead due to dynamic allocation.
