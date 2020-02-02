#Q1:
(1) Re-entrant means that the function can maintain context betwen successive
calls, rather than its output being corrupted by the fact that th function
was previously called in a different thread.
(2) strtok is not re-entrant because successive calls that parse the same
string are meant to pass in NULL in the "str" designated argument. So, if one
thread calls strtok on the same string that a different thread had most recently
passed into strtok, the function will fail in obtaining the proper next token.
In out case, repeated requests to access the same website by different
threads will fail. 