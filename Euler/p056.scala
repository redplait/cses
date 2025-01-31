@main def calc() = {
 // well, seems that there is no magic tricks so let just use crude bruteforce
 var len = 0
 for ( a <- (1 until 100);
       b <- (1 until 100) ) {
   val currLen = BigInt(a).pow(b).toString.toArray.map( _ - '0').sum
   if ( currLen > len ) then len = currLen
 }
 println(len)
}