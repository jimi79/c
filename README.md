# c
some c stuff


##### cblk-split

*This program can split a file into pieces, with the help of a key*

> Usage : cblk-split  [if=source_file] of=dest_file [of=next_dest_file] [key=hexa value] [BS=block size]
> 
>  Read source_file, with key, to split into dest_files
> 
>  default values :
>    if : stdin
>          key : default key (not recommanded). If the key isn't set, then it's returned.
>          BS : 1

Example

<pre>echo "Ceci est un test, Ceci est un test, Ceci est un test, Ceci est un test, Ceci est un test, Ceci est un test, Ceci est un test" | cblk-split of=a of=b key=030405 BS=1</pre>

a's content :
> Cect un t, Cst est, i esteseci eun tCect un t, Cst est, i estes"

b's content :
> i esteseci eun tCect un t, Cst est, i esteseci eun tCect un t


We have key = 030405. We take the first two characters, which represent 03 in hexa.
We then take the first 3 characters, and put that into file a.
Then we take 4 chars, and we write that into b.
Then we take 5 chars, and we write that into a.
We're at the end of the key, so we restart.
Then we take 3 chars, and we write that into a.
etc.
If we had block=2, we would have multiplied all key values by 2. Process would have been faster, but files would have more characters readables.

So stdin is splitted into a and b.

##### cblk-regroup

> Usage : cblk-regroup if=source_file [if=next_source_file] [of=dest_file] [key=hexa value] [BS=block size]
> 
>  Read all source_file, with key, to create dest_file
> 
>  default values :
>    if : stdout
>          key : default key (not recommanded)
>          BS : 1

Example, following the cblk-split example
<pre>cblk-regroup if=a if=b key=020304 BS=1</pre>
will generate the original stdin.

##### cblk-genkey [value]

Generate a key that can be used with cblk-split and cblk-regroup, value being the size.
