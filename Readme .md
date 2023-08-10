In the given program we implemented 
cd
    cd .
    opens the current directory again
    cd ..
    opens the previous directory
    cd ~
    opens the home directory
    cd -
    prints the previous directory path with out entering into it
    cd .name.
    opens the directory if exits
    ls, ls .
    prints the contents of the current directory
    ls ..
    prints the content of the previous directory
    ls ~
    prints the content of the home directory
    la -a
    prints the content of the current directory along with hidden files
    ls -l
    prints the content along with the file info of the current directory
    uses the print_ls function
    ls -l -a, ls - -l, ls -al, ls -la
    prints the content along with the file info of the current directory
    along with hidden files uses the print_ls function
    ls .flags. .directory name/path/file name/path.
    prints the info of the corresponding directory name/path/file name/path
    based on the flags.
pinfo
    prints the information regarding the processID, status, memory, exectuable path
pwd
    prints the path of curren working directory
echo
    prints the argument on the terminal
history
the functions history_storage were defined to use this function
    shows the commands that we used before
exec
the function handle_sigchld is used to print done statement after background process
    creates a child process and execute it with execvp() and after the completion of the process returns a statement if its a background process, else
    returns a statement includes the time taken
discover
the functions traverse, traverse_dir, traverse_file were the main functions containg the implmentation of the function
    discover, discover .
    prints the contents of the current directory
    discover ..
    prints the content of the previous directory
    discover ~
    prints the content of the home directory
    discover -d
    prints the data of the directory in the current directory
    discover -f
    prints the data of the files in the current directory
exit
    exits the terminal
